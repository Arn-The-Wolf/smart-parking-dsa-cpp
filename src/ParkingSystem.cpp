#include "ParkingSystem.h"
#include "ConsoleStyle.h"

#include <chrono>
#include <iostream>

// ParkingSystem is the façade: it wires managers together and exposes one API to main.cpp.
ParkingSystem::ParkingSystem()
    : vehicleManager(slotManager, billingManager),
      reportManager(slotManager, vehicleManager, billingManager) {
    if (loadFromFile(getDefaultDataFile())) {
        ConsoleStyle::printInfo("Restored previous session from " + getDefaultDataFile() + ".");
    }
}

std::string ParkingSystem::getDefaultDataFile() {
    return DataPersistence::kDefaultFile;
}

bool ParkingSystem::saveToFile(const std::string& filepath) const {
    return DataPersistence::save(slotManager, vehicleManager, billingManager, filepath);
}

bool ParkingSystem::loadFromFile(const std::string& filepath) {
    return DataPersistence::load(slotManager, vehicleManager, billingManager, filepath);
}

bool ParkingSystem::addSlot(const std::string& slotId, VehicleType supportedType,
                            const std::string& zone) {
    return slotManager.addSlot(slotId, supportedType, zone);
}

bool ParkingSystem::removeSlot(const std::string& slotId) {
    return slotManager.removeSlot(slotId);
}

void ParkingSystem::displayAllSlots() const {
    slotManager.displayAllSlots();
}

void ParkingSystem::displayAvailableSlots() const {
    reportManager.displayAvailableSlotsReport();
}

void ParkingSystem::displaySlotsByZone(const std::string& zone) const {
    slotManager.displaySlotsByZone(zone);
}

bool ParkingSystem::registerEntry(const std::string& plateNumber, VehicleType vehicleType) {
    return registerEntry(plateNumber, vehicleType, std::chrono::system_clock::now());
}

bool ParkingSystem::registerEntry(const std::string& plateNumber, VehicleType vehicleType,
                                  const std::chrono::system_clock::time_point& entryTime) {
    return vehicleManager.registerEntry(plateNumber, vehicleType, entryTime);
}

bool ParkingSystem::processExit(const std::string& plateNumber, const std::string& paymentMethod,
                                bool paymentReceived) {
    return processExit(plateNumber, std::chrono::system_clock::now(), paymentMethod, paymentReceived);
}

bool ParkingSystem::processExit(const std::string& plateNumber,
                                const std::chrono::system_clock::time_point& exitTime,
                                const std::string& paymentMethod, bool paymentReceived) {
    return vehicleManager.processExit(plateNumber, exitTime, paymentMethod, paymentReceived);
}

void ParkingSystem::displayParkedVehicles() const {
    reportManager.displayParkedVehiclesReport();
}

bool ParkingSystem::updateParkingPrice(VehicleType type, int newRate) {
    return billingManager.updateParkingPrice(type, newRate);
}

void ParkingSystem::displayCurrentTariffs() const {
    billingManager.displayCurrentTariffs();
}

void ParkingSystem::displayVehicleHistory(const std::string& plateNumber) const {
    billingManager.displayVehicleHistory(plateNumber);
}

void ParkingSystem::displayAllHistory() const {
    billingManager.displayAllHistory();
}

void ParkingSystem::displayDailyRevenue() const {
    reportManager.displayDailyRevenueReport();
}

void ParkingSystem::displayAvailableSlotsReport() const {
    reportManager.displayAvailableSlotsReport();
}

void ParkingSystem::displayParkedVehiclesReport() const {
    reportManager.displayParkedVehiclesReport();
}

void ParkingSystem::displayDailyRevenueReport() const {
    reportManager.displayDailyRevenueReport();
}

void ParkingSystem::displayVehicleHistoryReport(const std::string& plateNumber) const {
    reportManager.displayVehicleHistoryReport(plateNumber);
}

std::unique_ptr<Report> ParkingSystem::createAvailableSlotsReport() const {
    return reportManager.createAvailableSlotsReport();
}

std::unique_ptr<Report> ParkingSystem::createParkedVehiclesReport() const {
    return reportManager.createParkedVehiclesReport();
}

std::unique_ptr<Report> ParkingSystem::createDailyRevenueReport() const {
    return reportManager.createDailyRevenueReport();
}

bool ParkingSystem::slotExists(const std::string& slotId) const {
    return slotManager.slotExists(slotId);
}

bool ParkingSystem::hasAvailableSlot(VehicleType type) const {
    return slotManager.hasAvailableSlot(type);
}

bool ParkingSystem::isVehicleParked(const std::string& plateNumber) const {
    return vehicleManager.isVehicleParked(plateNumber);
}

std::chrono::system_clock::time_point ParkingSystem::getVehicleEntryTime(
    const std::string& plateNumber) const {
    return vehicleManager.getEntryTime(plateNumber);
}

void ParkingSystem::runBillingSelfTest() {
    std::cout << "\n=== BILLING EDGE-CASE SELF TEST ===\n";

    SlotManager testSlots;
    BillingManager testBilling;
    VehicleManager testVehicles(testSlots, testBilling);

    testSlots.addSlot("T-01", VehicleType::Car, "Test-Zone");

    auto entry = std::chrono::system_clock::now() - std::chrono::minutes(15);
    auto exit15 = entry + std::chrono::minutes(15);
    testVehicles.registerEntry("TEST15M", VehicleType::Car, entry);
    testVehicles.processExit("TEST15M", exit15, "Cash", true);

    entry = std::chrono::system_clock::now() - std::chrono::minutes(80);
    auto exit80 = entry + std::chrono::minutes(80);
    testVehicles.registerEntry("TEST80M", VehicleType::Car, entry);
    testVehicles.processExit("TEST80M", exit80, "Mobile Money", true);

    testBilling.updateParkingPrice(VehicleType::Car, 1500);
    entry = std::chrono::system_clock::now() - std::chrono::minutes(30);
    auto exit30 = entry + std::chrono::minutes(30);
    testVehicles.registerEntry("TESTPRICE", VehicleType::Car, entry);
    testVehicles.processExit("TESTPRICE", exit30, "Card", true);

    const auto& history = testBilling.getTransactionHistory();
    if (history.size() >= 3) {
        std::cout << "15 min billed hours: " << history[0].getBilledHours()
                  << " (expected 1) -> " << (history[0].getBilledHours() == 1 ? "PASS" : "FAIL") << "\n";
        std::cout << "80 min billed hours: " << history[1].getBilledHours()
                  << " (expected 2) -> " << (history[1].getBilledHours() == 2 ? "PASS" : "FAIL") << "\n";
        std::cout << "Old TX rate unchanged: " << history[0].getHourlyRate()
                  << " RWF (expected 1000) -> "
                  << (history[0].getHourlyRate() == 1000 ? "PASS" : "FAIL") << "\n";
        std::cout << "New TX uses updated rate: " << history[2].getHourlyRate()
                  << " RWF (expected 1500) -> "
                  << (history[2].getHourlyRate() == 1500 ? "PASS" : "FAIL") << "\n";
    }

    // Truck polymorphic surcharge: 1 hour at 1000 RWF base -> 1250 RWF total (25% extra).
    testSlots.addSlot("T-TRK", VehicleType::Truck, "Test-Zone");
    entry = std::chrono::system_clock::now() - std::chrono::minutes(60);
    auto exitTruck = entry + std::chrono::minutes(60);
    testVehicles.registerEntry("TESTTRK", VehicleType::Truck, entry);
    testVehicles.processExit("TESTTRK", exitTruck, "Cash", true);
    const ParkingTransaction& truckTx = testBilling.getTransactionHistory().back();
    std::cout << "Truck surcharge fee: " << truckTx.getTotalFee()
              << " RWF (expected 1250) -> "
              << (truckTx.getTotalFee() == 1250 ? "PASS" : "FAIL") << "\n";

    // Negative test: exit before entry must be rejected.
    entry = std::chrono::system_clock::now() - std::chrono::minutes(10);
    testVehicles.registerEntry("TESTBAD", VehicleType::Car, entry);
    auto badExit = entry - std::chrono::minutes(5);
    bool rejected = !testVehicles.processExit("TESTBAD", badExit, "Cash", true);
    std::cout << "Exit-before-entry rejected: " << (rejected ? "PASS" : "FAIL") << "\n";

    std::cout << "Self test complete (isolated in-memory session).\n";
}

void ParkingSystem::runAutomatedDemo() {
    std::cout << "\n=== AUTOMATED DEMO ===\n";

    if (getSlotCount() == 0) {
        addSlot("M01", VehicleType::Motorcycle, "Zone-A");
        addSlot("M02", VehicleType::Motorcycle, "Zone-A");
        addSlot("C01", VehicleType::Car, "Zone-B");
        addSlot("C02", VehicleType::Car, "Zone-B");
        addSlot("C03", VehicleType::Car, "Zone-C");
        addSlot("T01", VehicleType::Truck, "Zone-D");
        addSlot("T02", VehicleType::Truck, "Zone-D");
        std::cout << "Sample slots loaded.\n";
    }

    registerEntry("RAB123A", VehicleType::Car);
    registerEntry("RAC456B", VehicleType::Motorcycle);
    registerEntry("RAD789C", VehicleType::Truck);

    std::cout << "\n[Demo] Duplicate entry attempt...\n";
    registerEntry("RAB123A", VehicleType::Car);

    displayParkedVehiclesReport();
    displayAvailableSlotsReport();

    updateParkingPrice(VehicleType::Car, 1200);
    processExit("RAC456B", "Cash", true);
    processExit("RAB123A", "Mobile Money", true);

    displayAllHistory();
    displayDailyRevenueReport();

    std::cout << "\nDemo completed. RAD789C remains parked for manual exit testing.\n";
}
