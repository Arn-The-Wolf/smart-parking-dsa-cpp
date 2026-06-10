#ifndef VEHICLE_FACTORY_H
#define VEHICLE_FACTORY_H

#include "Enums.h"
#include "Vehicle.h"

#include <chrono>
#include <memory>
#include <string>

class VehicleFactory {
public:
    static std::unique_ptr<Vehicle> create(
        VehicleType type,
        const std::string& plate,
        const std::chrono::system_clock::time_point& entry,
        const std::string& slotId);
};

#endif
