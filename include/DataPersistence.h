#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include "BillingManager.h"
#include "SlotManager.h"
#include "VehicleManager.h"

#include <string>

// Saves and loads all in-memory parking data to a local text file.
class DataPersistence {
public:
    static constexpr const char* kDefaultFile = "parking_data.txt";

    static bool save(const SlotManager& slots, const VehicleManager& vehicles,
                     const BillingManager& billing, const std::string& filepath);
    static bool load(SlotManager& slots, VehicleManager& vehicles,
                     BillingManager& billing, const std::string& filepath);
};

#endif
