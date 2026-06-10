#ifndef VEHICLE_H
#define VEHICLE_H

#include "Enums.h"

#include <chrono>
#include <string>

// Abstract vehicle — subclasses override calculateFee() for polymorphic billing.
class Vehicle {protected:
    std::string plateNumber;
    std::chrono::system_clock::time_point entryTime;
    std::string allocatedSlotId;

public:
    Vehicle(const std::string& plate,
            const std::chrono::system_clock::time_point& entry,
            const std::string& slotId);
    virtual ~Vehicle() = default;

    virtual VehicleType getType() const = 0;
    virtual int calculateFee(int billedHours, int hourlyRate) const;

    const std::string& getPlateNumber() const { return plateNumber; }
    const std::chrono::system_clock::time_point& getEntryTime() const { return entryTime; }
    const std::string& getAllocatedSlotId() const { return allocatedSlotId; }
};

class MotorcycleVehicle : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType getType() const override { return VehicleType::Motorcycle; }
};

class CarVehicle : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType getType() const override { return VehicleType::Car; }
};

class TruckVehicle : public Vehicle {
public:
    using Vehicle::Vehicle;
    VehicleType getType() const override { return VehicleType::Truck; }
    int calculateFee(int billedHours, int hourlyRate) const override;
};

#endif
