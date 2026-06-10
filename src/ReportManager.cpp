#include "ReportManager.h"
#include "TimeUtils.h"

#include <chrono>
#include <iostream>
#include <sstream>

namespace {

class SlotsReport : public Report {
private:
    const SlotManager& slotManager;

public:
    explicit SlotsReport(const SlotManager& slots) : slotManager(slots) {}

    std::string getTitle() const override { return "Available Parking Slots Report"; }

    std::string generate() const override {
        std::ostringstream out;
        int count = 0;
        for (const auto& pair : slotManager.getSlots()) {
            if (pair.second.isAvailable()) {
                out << pair.second.getSlotId() << " ["
                    << vehicleTypeToString(pair.second.getSupportedType()) << "] - "
                    << pair.second.getZone() << "\n";
                count++;
            }
        }
        if (count == 0) {
            out << "No available slots.\n";
        } else {
            out << "Total available: " << count << "\n";
        }
        return out.str();
    }
};

class ParkedVehiclesReport : public Report {
private:
    const VehicleManager& vehicleManager;

public:
    explicit ParkedVehiclesReport(const VehicleManager& vehicles) : vehicleManager(vehicles) {}

    std::string getTitle() const override { return "Currently Parked Vehicles Report"; }

    std::string generate() const override {
        std::ostringstream out;
        const auto& sessions = vehicleManager.getActiveSessions();
        if (sessions.empty()) {
            out << "No vehicles currently parked.\n";
            return out.str();
        }
        for (const auto& pair : sessions) {
            const Vehicle& vehicle = *pair.second;
            out << vehicle.getPlateNumber() << " | "
                << vehicleTypeToString(vehicle.getType()) << " | Slot: "
                << vehicle.getAllocatedSlotId() << " | Entry: "
                << formatDateTime(vehicle.getEntryTime()) << "\n";
        }
        out << "Total parked: " << sessions.size() << "\n";
        return out.str();
    }
};

class DailyRevenueReport : public Report {
private:
    const BillingManager& billingManager;

public:
    explicit DailyRevenueReport(const BillingManager& billing) : billingManager(billing) {}

    std::string getTitle() const override { return "Daily Revenue Report"; }

    std::string generate() const override {
        std::ostringstream out;
        std::string today = formatDateKey(std::chrono::system_clock::now());
        long long revenue = billingManager.getRevenueForDate(today);
        int count = 0;
        for (const ParkingTransaction& tx : billingManager.getTransactionHistory()) {
            if (isSameCalendarDay(tx.getExitTime(), std::chrono::system_clock::now())) {
                count++;
            }
        }
        out << "Date: " << today << "\n";
        out << "Transactions today: " << count << "\n";
        out << "Revenue today: " << revenue << " RWF\n";
        return out.str();
    }
};

class VehicleHistoryReport : public Report {
private:
    const BillingManager& billingManager;
    std::string plateNumber;

public:
    VehicleHistoryReport(const BillingManager& billing, const std::string& plate)
        : billingManager(billing), plateNumber(plate) {}

    std::string getTitle() const override { return "Vehicle History Report"; }

    std::string generate() const override {
        std::ostringstream out;
        bool found = false;
        for (const ParkingTransaction& tx : billingManager.getTransactionHistory()) {
            if (tx.getPlateNumber() == plateNumber) {
                found = true;
                out << "TX#" << tx.getTransactionId() << " | "
                    << formatDateTime(tx.getEntryTime()) << " -> "
                    << formatDateTime(tx.getExitTime()) << " | "
                    << tx.getTotalFee() << " RWF | "
                    << tx.getPaymentMethod() << "\n";
            }
        }
        if (!found) {
            out << "No history for plate " << plateNumber << ".\n";
        }
        return out.str();
    }
};

void printReport(const Report& report) {
    std::cout << "\n=== " << report.getTitle() << " ===\n";
    std::cout << report.generate();
}

}  // namespace

ReportManager::ReportManager(const SlotManager& slots, const VehicleManager& vehicles,
                             const BillingManager& billing)
    : slotManager(slots), vehicleManager(vehicles), billingManager(billing) {}

void ReportManager::displayAvailableSlotsReport() const {
    printReport(*createAvailableSlotsReport());
}

void ReportManager::displayParkedVehiclesReport() const {
    printReport(*createParkedVehiclesReport());
}

void ReportManager::displayDailyRevenueReport() const {
    printReport(*createDailyRevenueReport());
}

void ReportManager::displayVehicleHistoryReport(const std::string& plateNumber) const {
    printReport(VehicleHistoryReport(billingManager, plateNumber));
}

std::unique_ptr<Report> ReportManager::createAvailableSlotsReport() const {
    return std::make_unique<SlotsReport>(slotManager);
}

std::unique_ptr<Report> ReportManager::createParkedVehiclesReport() const {
    return std::make_unique<ParkedVehiclesReport>(vehicleManager);
}

std::unique_ptr<Report> ReportManager::createDailyRevenueReport() const {
    return std::make_unique<DailyRevenueReport>(billingManager);
}
