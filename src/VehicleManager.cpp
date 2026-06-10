#include "VehicleManager.h"
#include "InputValidator.h"
#include "TimeUtils.h"
#include "VehicleFactory.h"

#include <iostream>
#include <iomanip>

VehicleManager::VehicleManager(SlotManager& slots, BillingManager& billing)
    : slotManager(slots), billingManager(billing) {}

bool VehicleManager::registerEntry(const std::string& plateNumber, VehicleType vehicleType,
                                   const std::chrono::system_clock::time_point& entryTime) {
    std::string error;
    if (!InputValidator::isValidPlateNumber(plateNumber, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }
    if (activeSessions.find(plateNumber) != activeSessions.end()) {
        std::cout << "Error: Vehicle '" << plateNumber
                  << "' is already parked. A vehicle cannot be parked twice.\n";
        return false;
    }
    if (entryTime > std::chrono::system_clock::now()) {
        std::cout << "Error: Entry time cannot be in the future.\n";
        return false;
    }
    if (!slotManager.hasAvailableSlot(vehicleType)) {
        std::cout << "Sorry: No available " << vehicleTypeToString(vehicleType)
                  << " parking slot. Please try again later.\n";
        return false;
    }

    // O(log n) slot allocation via SlotManager availability index.
    std::string slotId = slotManager.allocateSlot(vehicleType);
    if (slotId.empty()) {
        std::cout << "Sorry: No available " << vehicleTypeToString(vehicleType)
                  << " parking slot. Please try again later.\n";
        return false;
    }

    // Polymorphic vehicle object — fee logic delegated to calculateFee().
    auto vehicle = VehicleFactory::create(vehicleType, plateNumber, entryTime, slotId);
    if (!vehicle) {
        slotManager.releaseSlot(slotId);
        std::cout << "Error: Failed to create vehicle record.\n";
        return false;
    }

    std::cout << "Entry registered successfully.\n";
    std::cout << "  Plate:      " << plateNumber << "\n";
    std::cout << "  Type:       " << vehicleTypeToString(vehicleType) << "\n";
    std::cout << "  Slot:       " << slotId << " (Zone: " << slotManager.getZone(slotId) << ")\n";
    std::cout << "  Entry Time: " << formatDateTime(entryTime) << "\n";

    activeSessions.emplace(plateNumber, std::move(vehicle));
    return true;
}

bool VehicleManager::processExit(const std::string& plateNumber,
                                 const std::chrono::system_clock::time_point& exitTime,
                                 const std::string& paymentMethod, bool paymentReceived) {
    std::string error;
    if (!InputValidator::isValidPlateNumber(plateNumber, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }

    auto vehicleIt = activeSessions.find(plateNumber);
    if (vehicleIt == activeSessions.end()) {
        std::cout << "Error: Vehicle '" << plateNumber << "' is not currently parked.\n";
        return false;
    }

    if (exitTime > std::chrono::system_clock::now()) {
        std::cout << "Error: Exit time cannot be in the future.\n";
        return false;
    }

    const Vehicle& vehicle = *vehicleIt->second;
    std::string zone = slotManager.getZone(vehicle.getAllocatedSlotId());

    ParkingTransaction transaction;
    if (!billingManager.recordExit(vehicle, zone, exitTime, paymentMethod, paymentReceived, transaction)) {
        return false;
    }

    if (!slotManager.releaseSlot(vehicle.getAllocatedSlotId())) {
        std::cout << "Warning: Slot '" << vehicle.getAllocatedSlotId()
                  << "' could not be released cleanly.\n";
    }

    std::cout << "\n--- Vehicle Exit Summary ---\n";
    std::cout << "Transaction ID:  " << transaction.getTransactionId() << "\n";
    std::cout << "Plate Number:    " << plateNumber << "\n";
    std::cout << "Vehicle Type:    " << vehicleTypeToString(vehicle.getType()) << "\n";
    std::cout << "Slot Released:   " << vehicle.getAllocatedSlotId() << " (Zone: " << zone << ")\n";
    std::cout << "Entry Time:      " << formatDateTime(vehicle.getEntryTime()) << "\n";
    std::cout << "Exit Time:       " << formatDateTime(exitTime) << "\n";
    std::cout << "Duration:        " << transaction.getDurationMinutes() << " minute(s)\n";
    std::cout << "Billed Hours:    " << transaction.getBilledHours()
              << " hour(s) (partial hours rounded up)\n";
    std::cout << "Hourly Rate:     " << transaction.getHourlyRate() << " RWF\n";
    std::cout << "Total Fee:       " << transaction.getTotalFee() << " RWF\n";
    std::cout << "Payment Method:  " << transaction.getPaymentMethod() << "\n";
    std::cout << "Payment Status:  "
              << (transaction.isPaymentReceived() ? "Received" : "Pending") << "\n";

    if (!paymentReceived) {
        std::cout << "Note: Payment is pending. Follow up before vehicle leaves the area.\n";
    }

    activeSessions.erase(vehicleIt);
    return true;
}

bool VehicleManager::isVehicleParked(const std::string& plateNumber) const {
    return activeSessions.find(plateNumber) != activeSessions.end();
}

std::chrono::system_clock::time_point VehicleManager::getEntryTime(
    const std::string& plateNumber) const {
    auto it = activeSessions.find(plateNumber);
    if (it == activeSessions.end()) {
        return std::chrono::system_clock::time_point{};
    }
    return it->second->getEntryTime();
}

void VehicleManager::displayParkedVehicles() const {
    if (activeSessions.empty()) {
        std::cout << "No vehicles currently parked.\n";
        return;
    }

    std::cout << "\n--- Currently Parked Vehicles ---\n";
    std::cout << std::left << std::setw(12) << "Plate"
              << std::setw(14) << "Type"
              << std::setw(10) << "Slot"
              << "Entry Time\n";
    std::cout << std::string(55, '-') << "\n";

    for (const auto& pair : activeSessions) {
        const Vehicle& vehicle = *pair.second;
        std::cout << std::left << std::setw(12) << vehicle.getPlateNumber()
                  << std::setw(14) << vehicleTypeToString(vehicle.getType())
                  << std::setw(10) << vehicle.getAllocatedSlotId()
                  << formatDateTime(vehicle.getEntryTime()) << "\n";
    }
}
