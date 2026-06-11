#include "ConsoleStyle.h"
#include "InputReader.h"
#include "InputValidator.h"
#include "ParkingSystem.h"
#include "TimeUtils.h"

#include <chrono>
#include <iostream>
#include <string>

using InputReader::pauseBeforeReturning;
using InputReader::readIntInRange;
using InputReader::readLine;
using InputReader::readRequiredLine;
using InputReader::readYesNo;

namespace {

constexpr int kMinMenuOption = 0;
constexpr int kMaxMenuOption = 16;
constexpr const char* kAuthorName = "Author: RUYANGE Arnold";

bool saveAllData(ParkingSystem& system) {
    const std::string dataFile = ParkingSystem::getDefaultDataFile();
    if (system.saveToFile(dataFile)) {
        ConsoleStyle::printSuccess("All parking data saved to file: " + dataFile);
        return true;
    }
    ConsoleStyle::printError("Failed to save data to file: " + dataFile);
    return false;
}

bool selectVehicleType(VehicleType& type) {
    std::cout << "Select vehicle type:\n";
    std::cout << "  1. Motorcycle\n";
    std::cout << "  2. Car\n";
    std::cout << "  3. Truck\n";
    std::cout << "  0. Cancel\n";

    while (true) {
        std::string choice;
        if (!readLine("Choice: ", choice)) {
            return false;
        }
        if (choice == "0") {
            ConsoleStyle::printInfo("Operation cancelled.");
            return false;
        }
        if (parseVehicleType(choice, type)) {
            return true;
        }
        ConsoleStyle::printError("Invalid choice. Enter 1, 2, 3, or 0 to cancel.");
    }
}

bool readValidatedPlate(const std::string& prompt, std::string& out) {
    while (true) {
        if (!readRequiredLine(prompt, out, "Plate number")) {
            return false;
        }
        std::string error;
        if (InputValidator::isValidPlateNumber(out, error)) {
            return true;
        }
        ConsoleStyle::printError("Invalid plate: " + error);
    }
}

bool readValidatedSlotId(const std::string& prompt, std::string& out) {
    while (true) {
        if (!readRequiredLine(prompt, out, "Slot ID")) {
            return false;
        }
        std::string error;
        if (InputValidator::isValidSlotId(out, error)) {
            return true;
        }
        ConsoleStyle::printError("Invalid slot ID: " + error);
    }
}

bool readValidatedZone(const std::string& prompt, std::string& out) {
    while (true) {
        if (!readRequiredLine(prompt, out, "Zone")) {
            return false;
        }
        std::string error;
        if (InputValidator::isValidZone(out, error)) {
            return true;
        }
        ConsoleStyle::printError("Invalid zone: " + error);
    }
}

bool readValidatedRate(int& out) {
    while (true) {
        if (!readIntInRange("Enter new hourly rate (RWF): ", 1,
                            InputValidator::kMaxHourlyRate, out)) {
            return false;
        }
        std::string error;
        if (InputValidator::isValidHourlyRate(out, error)) {
            return true;
        }
        ConsoleStyle::printError(error);
    }
}

bool readValidatedParkingMinutes(const std::string& prompt, int& out) {
    while (true) {
        if (!readIntInRange(prompt, 1, InputValidator::kMaxParkingMinutes, out)) {
            return false;
        }
        std::string error;
        if (InputValidator::isValidParkingMinutes(out, error)) {
            return true;
        }
        ConsoleStyle::printError(error);
    }
}

bool selectPaymentMethod(std::string& method) {
    std::cout << "Payment method:\n";
    std::cout << "  1. Cash\n";
    std::cout << "  2. Mobile Money\n";
    std::cout << "  3. Card\n";

    while (true) {
        std::string choice;
        if (!readLine("Choice: ", choice)) {
            return false;
        }
        if (choice == "1" || choice == "Cash") {
            method = "Cash";
        } else if (choice == "2" || choice == "Mobile Money") {
            method = "Mobile Money";
        } else if (choice == "3" || choice == "Card") {
            method = "Card";
        } else {
            ConsoleStyle::printError("Invalid choice. Enter 1, 2, or 3.");
            continue;
        }
        std::string error;
        if (InputValidator::isValidPaymentMethod(method, error)) {
            return true;
        }
        ConsoleStyle::printError(error);
    }
}

bool confirmExit() {
    bool confirmed = false;
    if (!readYesNo("Are you sure you want to exit? (y/n): ", confirmed)) {
        return false;
    }
    if (!confirmed) {
        ConsoleStyle::printInfo("Exit cancelled. Returning to menu.");
        return false;
    }
    return true;
}

void loadSampleData(ParkingSystem& system) {
    struct SampleSlot { const char* id; VehicleType type; const char* zone; };
    const SampleSlot samples[] = {
        {"M01", VehicleType::Motorcycle, "Zone-A"},
        {"M02", VehicleType::Motorcycle, "Zone-A"},
        {"C01", VehicleType::Car, "Zone-B"},
        {"C02", VehicleType::Car, "Zone-B"},
        {"C03", VehicleType::Car, "Zone-C"},
        {"T01", VehicleType::Truck, "Zone-D"},
        {"T02", VehicleType::Truck, "Zone-D"},
    };

    int added = 0;
    int skipped = 0;
    for (const SampleSlot& sample : samples) {
        if (system.slotExists(sample.id)) {
            skipped++;
            continue;
        }
        if (system.addSlot(sample.id, sample.type, sample.zone)) {
            added++;
        }
    }
    ConsoleStyle::printSuccess("Sample data: " + std::to_string(added) + " slot(s) added, "
                               + std::to_string(skipped) + " skipped (duplicates).");
}

void printMainMenu() {
    std::cout << "\n";
    ConsoleStyle::printBannerLine('=');
    ConsoleStyle::printTitle("   KIGALI SMART PARKING MANAGEMENT");
    ConsoleStyle::printAuthor(kAuthorName);
    ConsoleStyle::printBannerLine('=');

    ConsoleStyle::printSection("SLOT MANAGEMENT");
    ConsoleStyle::printMenuItem(1, "Add parking slot");
    ConsoleStyle::printMenuItem(2, "Remove parking slot");
    ConsoleStyle::printMenuItem(3, "View all slots");
    ConsoleStyle::printMenuItem(4, "Available slots report (polymorphic)");
    ConsoleStyle::printMenuItem(5, "Search slots by zone");

    std::cout << "\n";
    ConsoleStyle::printSection("VEHICLE OPERATIONS");
    ConsoleStyle::printMenuItem(6, "Register vehicle entry");
    ConsoleStyle::printMenuItem(7, "Process vehicle exit + payment");
    ConsoleStyle::printMenuItem(8, "Parked vehicles report (polymorphic)");

    std::cout << "\n";
    ConsoleStyle::printSection("PRICING");
    ConsoleStyle::printMenuItem(9, "View current tariffs");
    ConsoleStyle::printMenuItem(10, "Update parking price");

    std::cout << "\n";
    ConsoleStyle::printSection("REPORTS (Task 5)");
    ConsoleStyle::printMenuItem(11, "Vehicle parking history");
    ConsoleStyle::printMenuItem(12, "All transaction history");
    ConsoleStyle::printMenuItem(13, "Daily revenue report (polymorphic)");

    std::cout << "\n";
    ConsoleStyle::printSection("TESTING / DEMO");
    ConsoleStyle::printMenuItem(14, "Load sample data");
    ConsoleStyle::printMenuItem(15, "Run automated demo");
    ConsoleStyle::printMenuItem(16, "Run billing edge-case self test");

    std::cout << "\n";
    ConsoleStyle::printMenuItem(0, "Exit");
    ConsoleStyle::printBannerLine('=');
}

bool handleAddSlot(ParkingSystem& system) {
    std::string slotId;
    std::string zone;
    VehicleType type{};
    if (!readValidatedSlotId("Enter Slot ID: ", slotId)) return false;
    if (!selectVehicleType(type)) return true;
    if (!readValidatedZone("Enter Zone: ", zone)) return false;
    system.addSlot(slotId, type, zone);
    return true;
}

bool handleRemoveSlot(ParkingSystem& system) {
    if (system.getSlotCount() == 0) {
        ConsoleStyle::printWarning("No slots configured. Add slots first (option 1 or 14).");
        return true;
    }
    std::string slotId;
    if (!readValidatedSlotId("Enter Slot ID to remove: ", slotId)) return false;
    system.removeSlot(slotId);
    return true;
}

bool handleVehicleEntry(ParkingSystem& system) {
    if (system.getSlotCount() == 0) {
        ConsoleStyle::printWarning("No parking slots configured. Use option 1 or 14 first.");
        return true;
    }

    std::string plate;
    VehicleType type{};
    if (!readValidatedPlate("Enter plate number: ", plate)) return false;
    if (!selectVehicleType(type)) return true;

    if (!system.hasAvailableSlot(type)) {
        ConsoleStyle::printWarning("No available " + std::string(vehicleTypeToString(type))
                                   + " slots. Check option 4 or add more slots.");
        return true;
    }

    bool useTestTime = false;
    if (!readYesNo("Use test entry time? (y/n): ", useTestTime)) return false;

    if (useTestTime) {
        int minutesAgo = 0;
        if (!readValidatedParkingMinutes("How many minutes ago did the vehicle enter? ", minutesAgo)) {
            return false;
        }
        auto entryTime = std::chrono::system_clock::now() - std::chrono::minutes(minutesAgo);
        system.registerEntry(plate, type, entryTime);
    } else {
        system.registerEntry(plate, type);
    }
    return true;
}

bool handleVehicleExit(ParkingSystem& system) {
    if (system.getActiveVehicleCount() == 0) {
        ConsoleStyle::printWarning("No vehicles are currently parked.");
        return true;
    }

    std::string plate;
    std::string paymentMethod;
    bool paymentReceived = false;

    if (!readValidatedPlate("Enter plate number: ", plate)) return false;

    if (!system.isVehicleParked(plate)) {
        ConsoleStyle::printError("Vehicle '" + plate + "' is not in the active parking list.");
        return true;
    }

    if (!selectPaymentMethod(paymentMethod)) return false;
    if (!readYesNo("Payment received? (y/n): ", paymentReceived)) return false;

    bool useTestTime = false;
    if (!readYesNo("Use test exit time offset? (y/n): ", useTestTime)) return false;

    if (useTestTime) {
        int minutesAgo = 0;
        if (!readValidatedParkingMinutes("Exit how many minutes ago? ", minutesAgo)) return false;

        auto exitTime = std::chrono::system_clock::now() - std::chrono::minutes(minutesAgo);
        auto entryTime = system.getVehicleEntryTime(plate);

        if (exitTime < entryTime) {
            ConsoleStyle::printError("Exit time (" + formatDateTime(exitTime)
                                     + ") is before entry time (" + formatDateTime(entryTime) + ").");
            return true;
        }
        system.processExit(plate, exitTime, paymentMethod, paymentReceived);
    } else {
        system.processExit(plate, paymentMethod, paymentReceived);
    }
    return true;
}

bool handleUpdatePrice(ParkingSystem& system) {
    VehicleType type{};
    int newRate = 0;
    if (!selectVehicleType(type)) return true;
    if (!readValidatedRate(newRate)) return false;
    system.updateParkingPrice(type, newRate);
    return true;
}

bool handleVehicleHistory(ParkingSystem& system) {
    std::string plate;
    if (!readValidatedPlate("Enter plate number: ", plate)) return false;
    bool useReport = false;
    if (!readYesNo("Use polymorphic report format? (y/n): ", useReport)) return false;
    if (useReport) {
        system.displayVehicleHistoryReport(plate);
    } else {
        system.displayVehicleHistory(plate);
    }
    return true;
}

bool handleZoneSearch(ParkingSystem& system) {
    if (system.getSlotCount() == 0) {
        ConsoleStyle::printWarning("No slots configured yet.");
        return true;
    }
    std::string zone;
    if (!readValidatedZone("Enter zone name: ", zone)) return false;
    system.displaySlotsByZone(zone);
    return true;
}

bool readMenuChoice(int& choice) {
    return readIntInRange("Select option: ", kMinMenuOption, kMaxMenuOption, choice);
}

}  // namespace

int main() {
    ConsoleStyle::enableColors();
    ParkingSystem system;

    ConsoleStyle::printTitle("Welcome to Kigali Smart Parking Management System");
    ConsoleStyle::printAuthor(kAuthorName);
    ConsoleStyle::printSubtitle("In-memory DSA solution - real-time slot allocation and billing.");

    bool running = true;
    while (running) {
        printMainMenu();

        int choice = 0;
        if (!readMenuChoice(choice)) {
            saveAllData(system);
            ConsoleStyle::printWarning("Exiting program.");
            break;
        }

        bool continueRunning = true;

        switch (choice) {
            case 1:  continueRunning = handleAddSlot(system); break;
            case 2:  continueRunning = handleRemoveSlot(system); break;
            case 3:  system.displayAllSlots(); break;
            case 4:  system.displayAvailableSlotsReport(); break;
            case 5:  continueRunning = handleZoneSearch(system); break;
            case 6:  continueRunning = handleVehicleEntry(system); break;
            case 7:  continueRunning = handleVehicleExit(system); break;
            case 8:  system.displayParkedVehiclesReport(); break;
            case 9:  system.displayCurrentTariffs(); break;
            case 10: continueRunning = handleUpdatePrice(system); break;
            case 11: continueRunning = handleVehicleHistory(system); break;
            case 12: system.displayAllHistory(); break;
            case 13: system.displayDailyRevenueReport(); break;
            case 14: loadSampleData(system); break;
            case 15: system.runAutomatedDemo(); break;
            case 16: system.runBillingSelfTest(); break;
            case 0:
                if (confirmExit()) {
                    saveAllData(system);
                    running = false;
                    ConsoleStyle::printSuccess("Thank you for using Smart Parking System. Goodbye!");
                }
                break;
            default:
                ConsoleStyle::printError("Invalid option. Please try again.");
                break;
        }

        if (!continueRunning) {
            saveAllData(system);
            running = false;
            if (choice != 0) {
                ConsoleStyle::printWarning("Exiting program.");
            }
        } else if (running && choice != 0) {
            pauseBeforeReturning();
        }
    }

    return 0;
}
