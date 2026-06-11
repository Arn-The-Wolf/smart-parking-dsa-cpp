#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include "BillingManager.h"
#include "DataPersistence.h"
#include "Enums.h"
#include "Report.h"
#include "ReportManager.h"
#include "SlotManager.h"
#include "VehicleManager.h"

#include <chrono>
#include <memory>
#include <string>

class ParkingSystem {
private:
    SlotManager slotManager;
    BillingManager billingManager;
    VehicleManager vehicleManager;
    ReportManager reportManager;

public:
    ParkingSystem();

    bool addSlot(const std::string& slotId, VehicleType supportedType, const std::string& zone);
    bool removeSlot(const std::string& slotId);
    void displayAllSlots() const;
    void displayAvailableSlots() const;
    void displaySlotsByZone(const std::string& zone) const;

    bool registerEntry(const std::string& plateNumber, VehicleType vehicleType);
    bool registerEntry(const std::string& plateNumber, VehicleType vehicleType,
                       const std::chrono::system_clock::time_point& entryTime);
    bool processExit(const std::string& plateNumber, const std::string& paymentMethod,
                     bool paymentReceived);
    bool processExit(const std::string& plateNumber,
                     const std::chrono::system_clock::time_point& exitTime,
                     const std::string& paymentMethod, bool paymentReceived);
    void displayParkedVehicles() const;

    bool updateParkingPrice(VehicleType type, int newRate);
    void displayCurrentTariffs() const;

    void displayVehicleHistory(const std::string& plateNumber) const;
    void displayAllHistory() const;
    void displayDailyRevenue() const;

    void displayAvailableSlotsReport() const;
    void displayParkedVehiclesReport() const;
    void displayDailyRevenueReport() const;
    void displayVehicleHistoryReport(const std::string& plateNumber) const;

    std::unique_ptr<Report> createAvailableSlotsReport() const;
    std::unique_ptr<Report> createParkedVehiclesReport() const;
    std::unique_ptr<Report> createDailyRevenueReport() const;

    void runBillingSelfTest();
    void runAutomatedDemo();

    bool slotExists(const std::string& slotId) const;
    bool hasAvailableSlot(VehicleType type) const;
    bool isVehicleParked(const std::string& plateNumber) const;
    std::chrono::system_clock::time_point getVehicleEntryTime(const std::string& plateNumber) const;

    bool saveToFile(const std::string& filepath = DataPersistence::kDefaultFile) const;
    bool loadFromFile(const std::string& filepath = DataPersistence::kDefaultFile);
    static std::string getDefaultDataFile();

    int getSlotCount() const { return slotManager.getSlotCount(); }
    int getActiveVehicleCount() const { return vehicleManager.getActiveVehicleCount(); }
    int getTransactionCount() const { return billingManager.getTransactionCount(); }
};

#endif
