#include "Vehicle.h"

Vehicle::Vehicle(const std::string& plate,
                 const std::chrono::system_clock::time_point& entry,
                 const std::string& slotId)
    : plateNumber(plate), entryTime(entry), allocatedSlotId(slotId) {}

int Vehicle::calculateFee(int billedHours, int hourlyRate) const {
    return billedHours * hourlyRate;
}

int TruckVehicle::calculateFee(int billedHours, int hourlyRate) const {
    // Trucks pay base hourly rate plus a 25% space surcharge (polymorphic override).
    int baseFee = Vehicle::calculateFee(billedHours, hourlyRate);
    return baseFee + (baseFee / 4);
}
