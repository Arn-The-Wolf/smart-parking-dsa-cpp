#ifndef SLOT_MANAGER_H
#define SLOT_MANAGER_H

#include "Enums.h"
#include "ParkingSlot.h"

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// Manages parking slot configuration, zone indexing, and O(log n) slot allocation.
class SlotManager {
private:
    std::unordered_map<std::string, ParkingSlot> slots;    std::unordered_map<std::string, std::vector<std::string>> slotsByZone;
    std::map<VehicleType, std::set<std::string>> availableByType;

    void addToAvailabilityIndex(const std::string& slotId, VehicleType type);
    void removeFromAvailabilityIndex(const std::string& slotId, VehicleType type);

public:
    bool addSlot(const std::string& slotId, VehicleType supportedType, const std::string& zone);
    bool removeSlot(const std::string& slotId);
    std::string allocateSlot(VehicleType vehicleType);
    bool releaseSlot(const std::string& slotId);
    bool slotExists(const std::string& slotId) const;
    bool hasAvailableSlot(VehicleType vehicleType) const;
    std::string getZone(const std::string& slotId) const;

    void displayAllSlots() const;
    void displayAvailableSlots() const;
    void displaySlotsByZone(const std::string& zone) const;

    void clearAll();
    void restoreSlot(const std::string& slotId, VehicleType supportedType,
                     const std::string& zone, SlotStatus status);

    const std::unordered_map<std::string, ParkingSlot>& getSlots() const { return slots; }
    int getSlotCount() const { return static_cast<int>(slots.size()); }
};

#endif
