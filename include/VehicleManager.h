#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "BillingManager.h"
#include "Enums.h"
#include "SlotManager.h"
#include "Vehicle.h"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

// Tracks active parking sessions and coordinates slot allocation with billing on exit.
class VehicleManager {
private:
    SlotManager& slotManager;
    BillingManager& billingManager;
    std::unordered_map<std::string, std::unique_ptr<Vehicle>> activeSessions;

public:
    VehicleManager(SlotManager& slots, BillingManager& billing);

    bool registerEntry(const std::string& plateNumber, VehicleType vehicleType,
                       const std::chrono::system_clock::time_point& entryTime);
    bool processExit(const std::string& plateNumber,
                     const std::chrono::system_clock::time_point& exitTime,
                     const std::string& paymentMethod, bool paymentReceived);

    bool isVehicleParked(const std::string& plateNumber) const;
    std::chrono::system_clock::time_point getEntryTime(const std::string& plateNumber) const;

    void clearAll();
    bool restoreSession(const std::string& plateNumber, VehicleType vehicleType,
                        const std::chrono::system_clock::time_point& entryTime,
                        const std::string& slotId);

    void displayParkedVehicles() const;
    const std::unordered_map<std::string, std::unique_ptr<Vehicle>>& getActiveSessions() const {
        return activeSessions;
    }
    int getActiveVehicleCount() const { return static_cast<int>(activeSessions.size()); }
};

#endif
