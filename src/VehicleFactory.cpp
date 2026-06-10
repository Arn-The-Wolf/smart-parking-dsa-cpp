#include "VehicleFactory.h"

std::unique_ptr<Vehicle> VehicleFactory::create(
    VehicleType type,
    const std::string& plate,
    const std::chrono::system_clock::time_point& entry,
    const std::string& slotId) {
    switch (type) {
        case VehicleType::Motorcycle:
            return std::make_unique<MotorcycleVehicle>(plate, entry, slotId);
        case VehicleType::Car:
            return std::make_unique<CarVehicle>(plate, entry, slotId);
        case VehicleType::Truck:
            return std::make_unique<TruckVehicle>(plate, entry, slotId);
        default:
            return nullptr;
    }
}
