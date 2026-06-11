#include "SlotManager.h"
#include "InputValidator.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

void SlotManager::addToAvailabilityIndex(const std::string& slotId, VehicleType type) {
    availableByType[type].insert(slotId);
}

void SlotManager::removeFromAvailabilityIndex(const std::string& slotId, VehicleType type) {
    auto it = availableByType.find(type);
    if (it != availableByType.end()) {
        it->second.erase(slotId);
    }
}

bool SlotManager::addSlot(const std::string& slotId, VehicleType supportedType,
                          const std::string& zone) {
    std::string error;
    if (!InputValidator::isValidSlotId(slotId, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }
    if (!InputValidator::isValidZone(zone, error)) {
        std::cout << "Error: " << error << "\n";
        return false;
    }
    if (slots.find(slotId) != slots.end()) {
        std::cout << "Error: Slot ID '" << slotId << "' already exists.\n";
        return false;
    }

    slots.emplace(slotId, ParkingSlot(slotId, supportedType, zone));
    slotsByZone[zone].push_back(slotId);
    addToAvailabilityIndex(slotId, supportedType);

    std::cout << "Success: Slot '" << slotId << "' configured in zone '" << zone
              << "' for " << vehicleTypeToString(supportedType) << ".\n";
    return true;
}

bool SlotManager::removeSlot(const std::string& slotId) {
    auto it = slots.find(slotId);
    if (it == slots.end()) {
        std::cout << "Error: Slot '" << slotId << "' not found.\n";
        return false;
    }
    if (it->second.getStatus() == SlotStatus::Occupied) {
        std::cout << "Error: Cannot remove occupied slot '" << slotId << "'.\n";
        return false;
    }

    VehicleType type = it->second.getSupportedType();
    const std::string& zone = it->second.getZone();
    removeFromAvailabilityIndex(slotId, type);

    auto& zoneSlots = slotsByZone[zone];
    zoneSlots.erase(std::remove(zoneSlots.begin(), zoneSlots.end(), slotId), zoneSlots.end());

    slots.erase(it);
    std::cout << "Success: Slot '" << slotId << "' removed.\n";
    return true;
}

std::string SlotManager::allocateSlot(VehicleType vehicleType) {
    auto it = availableByType.find(vehicleType);
    if (it == availableByType.end() || it->second.empty()) {
        return "";
    }

    auto slotIt = it->second.begin();
    std::string slotId = *slotIt;
    it->second.erase(slotIt);

    auto slotRecord = slots.find(slotId);
    if (slotRecord != slots.end()) {
        slotRecord->second.setStatus(SlotStatus::Occupied);
    }
    return slotId;
}

bool SlotManager::releaseSlot(const std::string& slotId) {
    auto it = slots.find(slotId);
    if (it == slots.end()) {
        return false;
    }

    it->second.setStatus(SlotStatus::Available);
    addToAvailabilityIndex(slotId, it->second.getSupportedType());
    return true;
}

bool SlotManager::slotExists(const std::string& slotId) const {
    return slots.find(slotId) != slots.end();
}

bool SlotManager::hasAvailableSlot(VehicleType vehicleType) const {
    auto it = availableByType.find(vehicleType);
    return it != availableByType.end() && !it->second.empty();
}

std::string SlotManager::getZone(const std::string& slotId) const {
    auto it = slots.find(slotId);
    if (it == slots.end()) {
        return "Unknown";
    }
    return it->second.getZone();
}

void SlotManager::displayAllSlots() const {
    if (slots.empty()) {
        std::cout << "No parking slots configured.\n";
        return;
    }

    std::cout << "\n--- All Parking Slots ---\n";
    std::cout << std::left << std::setw(10) << "Slot ID"
              << std::setw(14) << "Vehicle Type"
              << std::setw(12) << "Zone"
              << "Status\n";
    std::cout << std::string(50, '-') << "\n";

    for (const auto& pair : slots) {
        const ParkingSlot& slot = pair.second;
        std::cout << std::left << std::setw(10) << slot.getSlotId()
                  << std::setw(14) << vehicleTypeToString(slot.getSupportedType())
                  << std::setw(12) << slot.getZone()
                  << slotStatusToString(slot.getStatus()) << "\n";
    }
}

void SlotManager::displayAvailableSlots() const {
    bool found = false;
    std::cout << "\n--- Available Parking Slots ---\n";
    std::cout << std::left << std::setw(10) << "Slot ID"
              << std::setw(14) << "Vehicle Type"
              << "Zone\n";
    std::cout << std::string(40, '-') << "\n";

    for (const auto& pair : slots) {
        const ParkingSlot& slot = pair.second;
        if (slot.isAvailable()) {
            found = true;
            std::cout << std::left << std::setw(10) << slot.getSlotId()
                      << std::setw(14) << vehicleTypeToString(slot.getSupportedType())
                      << slot.getZone() << "\n";
        }
    }

    if (!found) {
        std::cout << "No available slots at the moment.\n";
    }
}

void SlotManager::clearAll() {
    slots.clear();
    slotsByZone.clear();
    availableByType.clear();
}

void SlotManager::restoreSlot(const std::string& slotId, VehicleType supportedType,
                              const std::string& zone, SlotStatus status) {
    slots.emplace(slotId, ParkingSlot(slotId, supportedType, zone, status));
    slotsByZone[zone].push_back(slotId);
    if (status == SlotStatus::Available) {
        addToAvailabilityIndex(slotId, supportedType);
    }
}

void SlotManager::displaySlotsByZone(const std::string& zone) const {
    auto it = slotsByZone.find(zone);
    if (it == slotsByZone.end() || it->second.empty()) {
        std::cout << "No slots found in zone '" << zone << "'.\n";
        return;
    }

    std::cout << "\n--- Slots in Zone '" << zone << "' ---\n";
    std::cout << std::left << std::setw(10) << "Slot ID"
              << std::setw(14) << "Vehicle Type"
              << "Status\n";
    std::cout << std::string(40, '-') << "\n";

    for (const std::string& slotId : it->second) {
        auto slotIt = slots.find(slotId);
        if (slotIt == slots.end()) {
            continue;
        }
        const ParkingSlot& slot = slotIt->second;
        std::cout << std::left << std::setw(10) << slot.getSlotId()
                  << std::setw(14) << vehicleTypeToString(slot.getSupportedType())
                  << slotStatusToString(slot.getStatus()) << "\n";
    }
}
