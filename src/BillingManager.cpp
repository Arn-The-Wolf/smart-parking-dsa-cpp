#include "BillingManager.h"
#include "InputValidator.h"
#include "TimeUtils.h"

#include <iostream>
#include <iomanip>
#include <limits>

BillingManager::BillingManager() : nextTransactionId(1) {}

int BillingManager::calculateBilledHours(int durationMinutes) {
    if (durationMinutes <= 0) {
        return 1;
    }
    return (durationMinutes + 59) / 60;
}

bool BillingManager::updateParkingPrice(VehicleType type, int newRate) {
    std::string error;
    if (!InputValidator::isValidHourlyRate(newRate, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }
    if (!tariffManager.updateRate(type, newRate)) {
        std::cout << "Error: Failed to update tariff.\n";
        return false;
    }
    std::cout << "Success: " << vehicleTypeToString(type)
              << " parking rate updated to " << newRate << " RWF/hour.\n";
    std::cout << "Note: This change applies only to future exits. Completed records are unchanged.\n";
    return true;
}

void BillingManager::displayCurrentTariffs() const {
    tariffManager.displayRates();
}

int BillingManager::getRate(VehicleType type) const {
    return tariffManager.getRate(type);
}

bool BillingManager::recordExit(
    const Vehicle& vehicle, const std::string& zone,
    const std::chrono::system_clock::time_point& exitTime,
    const std::string& paymentMethod, bool paymentReceived,
    ParkingTransaction& outTransaction) {

    if (exitTime < vehicle.getEntryTime()) {
        std::cout << "Error: Exit time cannot be earlier than entry time.\n";
        return false;
    }

    std::string error;
    if (!InputValidator::isValidPaymentMethod(paymentMethod, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }

    auto duration = std::chrono::duration_cast<std::chrono::minutes>(exitTime - vehicle.getEntryTime());
    int durationMinutes = static_cast<int>(duration.count());
    if (durationMinutes < 1) {
        durationMinutes = 1;
    }

    if (!InputValidator::isValidParkingMinutes(durationMinutes, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }

    int billedHours = calculateBilledHours(durationMinutes);
    int hourlyRate = tariffManager.getRate(vehicle.getType());
    if (hourlyRate <= 0) {
        std::cout << "Error: No valid tariff configured for "
                  << vehicleTypeToString(vehicle.getType()) << ". Update price first.\n";
        return false;
    }

    long long feeCalculation = static_cast<long long>(billedHours) * hourlyRate;
    if (feeCalculation > std::numeric_limits<int>::max()) {
        std::cout << "Error: Calculated fee exceeds system limits. Contact administrator.\n";
        return false;
    }
    int totalFee = static_cast<int>(feeCalculation);

    ParkingTransaction transaction(
        nextTransactionId++,
        vehicle.getPlateNumber(),
        vehicle.getType(),
        vehicle.getAllocatedSlotId(),
        zone,
        vehicle.getEntryTime(),
        exitTime,
        durationMinutes,
        billedHours,
        hourlyRate,
        totalFee,
        paymentMethod,
        paymentReceived
    );

    size_t index = transactionHistory.size();
    transactionHistory.push_back(transaction);
    plateHistoryIndex[vehicle.getPlateNumber()].push_back(index);

    std::string dateKey = formatDateKey(exitTime);
    dailyRevenueIndex[dateKey] += totalFee;
    dailyTransactionCount[dateKey] += 1;

    outTransaction = transaction;
    return true;
}

void BillingManager::displayVehicleHistory(const std::string& plateNumber) const {
    auto it = plateHistoryIndex.find(plateNumber);
    if (it == plateHistoryIndex.end() || it->second.empty()) {
        std::cout << "No completed parking records found for '" << plateNumber << "'.\n";
        return;
    }

    std::cout << "\n--- Parking History for '" << plateNumber << "' ---\n";
    for (size_t index : it->second) {
        const ParkingTransaction& tx = transactionHistory[index];
        std::cout << "TX#" << tx.getTransactionId()
                  << " | Slot: " << tx.getSlotId()
                  << " | Zone: " << tx.getZone()
                  << " | Entry: " << formatDateTime(tx.getEntryTime())
                  << " | Exit: " << formatDateTime(tx.getExitTime())
                  << " | Fee: " << tx.getTotalFee() << " RWF"
                  << " | Payment: " << tx.getPaymentMethod()
                  << (tx.isPaymentReceived() ? " (Received)" : " (Pending)") << "\n";
    }
}

void BillingManager::displayAllHistory() const {
    if (transactionHistory.empty()) {
        std::cout << "No completed parking transactions yet.\n";
        return;
    }

    std::cout << "\n--- All Parking Transaction History ---\n";
    std::cout << std::left << std::setw(6) << "TX#"
              << std::setw(12) << "Plate"
              << std::setw(8) << "Slot"
              << std::setw(8) << "Hours"
              << std::setw(10) << "Rate"
              << std::setw(12) << "Payment"
              << "Fee (RWF)\n";
    std::cout << std::string(72, '-') << "\n";

    for (const ParkingTransaction& tx : transactionHistory) {
        std::cout << std::left << std::setw(6) << tx.getTransactionId()
                  << std::setw(12) << tx.getPlateNumber()
                  << std::setw(8) << tx.getSlotId()
                  << std::setw(8) << tx.getBilledHours()
                  << std::setw(10) << tx.getHourlyRate()
                  << std::setw(12) << tx.getPaymentMethod()
                  << tx.getTotalFee() << "\n";
    }
}

void BillingManager::displayDailyRevenue() const {
    std::string today = formatDateKey(std::chrono::system_clock::now());
    long long revenue = getRevenueForDate(today);
    int count = 0;
    auto countIt = dailyTransactionCount.find(today);
    if (countIt != dailyTransactionCount.end()) {
        count = countIt->second;
    }

    std::cout << "\n--- Daily Revenue Report ---\n";
    std::cout << "Date:              " << today << "\n";
    std::cout << "Transactions:      " << count << "\n";
    std::cout << "Total Revenue:     " << revenue << " RWF\n";
}

void BillingManager::clearAll() {
    transactionHistory.clear();
    plateHistoryIndex.clear();
    dailyRevenueIndex.clear();
    dailyTransactionCount.clear();
    nextTransactionId = 1;
    tariffManager = TariffManager();
}

void BillingManager::restoreTransaction(const ParkingTransaction& transaction) {
    size_t index = transactionHistory.size();
    transactionHistory.push_back(transaction);
    plateHistoryIndex[transaction.getPlateNumber()].push_back(index);
}

void BillingManager::restoreDailyRevenue(const std::string& dateKey, long long revenue, int count) {
    dailyRevenueIndex[dateKey] = revenue;
    dailyTransactionCount[dateKey] = count;
}

void BillingManager::setNextTransactionId(int id) {
    nextTransactionId = id;
}

TariffManager& BillingManager::getTariffManager() {
    return tariffManager;
}

long long BillingManager::getRevenueForDate(const std::string& dateKey) const {
    auto it = dailyRevenueIndex.find(dateKey);
    if (it == dailyRevenueIndex.end()) {
        return 0;
    }
    return it->second;
}
