#ifndef PARKING_TRANSACTION_H
#define PARKING_TRANSACTION_H

#include "Enums.h"

#include <chrono>
#include <string>

class ParkingTransaction {
private:
    int transactionId;
    std::string plateNumber;
    VehicleType vehicleType;
    std::string slotId;
    std::string zone;
    std::chrono::system_clock::time_point entryTime;
    std::chrono::system_clock::time_point exitTime;
    int durationMinutes;
    int billedHours;
    int hourlyRate;
    int totalFee;
    std::string paymentMethod;
    bool paymentReceived;

public:
    ParkingTransaction() = default;

    ParkingTransaction(int id, const std::string& plate, VehicleType type, const std::string& slot,
                       const std::string& zoneName,
                       const std::chrono::system_clock::time_point& entry,
                       const std::chrono::system_clock::time_point& exit,
                       int minutes, int hours, int rate, int fee,
                       const std::string& method, bool paid)
        : transactionId(id), plateNumber(plate), vehicleType(type), slotId(slot), zone(zoneName),
          entryTime(entry), exitTime(exit), durationMinutes(minutes),
          billedHours(hours), hourlyRate(rate), totalFee(fee),
          paymentMethod(method), paymentReceived(paid) {}

    int getTransactionId() const { return transactionId; }
    const std::string& getPlateNumber() const { return plateNumber; }
    VehicleType getVehicleType() const { return vehicleType; }
    const std::string& getSlotId() const { return slotId; }
    const std::string& getZone() const { return zone; }
    const std::chrono::system_clock::time_point& getEntryTime() const { return entryTime; }
    const std::chrono::system_clock::time_point& getExitTime() const { return exitTime; }
    int getDurationMinutes() const { return durationMinutes; }
    int getBilledHours() const { return billedHours; }
    int getHourlyRate() const { return hourlyRate; }
    int getTotalFee() const { return totalFee; }
    const std::string& getPaymentMethod() const { return paymentMethod; }
    bool isPaymentReceived() const { return paymentReceived; }
};

#endif
