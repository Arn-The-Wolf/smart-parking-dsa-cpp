#include "DataPersistence.h"

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr const char* kFormatVersion = "PARKING_SYSTEM_DATA_V1";

std::string vehicleTypeToken(VehicleType type) {
    switch (type) {
        case VehicleType::Motorcycle: return "MOTORCYCLE";
        case VehicleType::Car:        return "CAR";
        case VehicleType::Truck:      return "TRUCK";
        default:                      return "UNKNOWN";
    }
}

bool parseVehicleTypeToken(const std::string& token, VehicleType& out) {
    if (token == "MOTORCYCLE") { out = VehicleType::Motorcycle; return true; }
    if (token == "CAR") { out = VehicleType::Car; return true; }
    if (token == "TRUCK") { out = VehicleType::Truck; return true; }
    return false;
}

std::string slotStatusToken(SlotStatus status) {
    return status == SlotStatus::Occupied ? "OCCUPIED" : "AVAILABLE";
}

bool parseSlotStatusToken(const std::string& token, SlotStatus& out) {
    if (token == "OCCUPIED") { out = SlotStatus::Occupied; return true; }
    if (token == "AVAILABLE") { out = SlotStatus::Available; return true; }
    return false;
}

long long toEpoch(const std::chrono::system_clock::time_point& timePoint) {
    return std::chrono::duration_cast<std::chrono::seconds>(timePoint.time_since_epoch()).count();
}

std::chrono::system_clock::time_point fromEpoch(long long seconds) {
    return std::chrono::system_clock::time_point{std::chrono::seconds(seconds)};
}

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

}  // namespace

bool DataPersistence::save(const SlotManager& slots, const VehicleManager& vehicles,
                           const BillingManager& billing, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }

    out << kFormatVersion << "\n";

    out << "[TARIFFS]\n";
    out << vehicleTypeToken(VehicleType::Motorcycle) << " " << billing.getRate(VehicleType::Motorcycle) << "\n";
    out << vehicleTypeToken(VehicleType::Car) << " " << billing.getRate(VehicleType::Car) << "\n";
    out << vehicleTypeToken(VehicleType::Truck) << " " << billing.getRate(VehicleType::Truck) << "\n";
    out << "NEXT_TX_ID " << billing.getNextTransactionId() << "\n";

    out << "[SLOTS]\n";
    for (const auto& pair : slots.getSlots()) {
        const ParkingSlot& slot = pair.second;
        out << slot.getSlotId() << "|" << vehicleTypeToken(slot.getSupportedType()) << "|"
            << slot.getZone() << "|" << slotStatusToken(slot.getStatus()) << "\n";
    }

    out << "[ACTIVE]\n";
    for (const auto& pair : vehicles.getActiveSessions()) {
        const Vehicle& vehicle = *pair.second;
        out << vehicle.getPlateNumber() << "|" << vehicleTypeToken(vehicle.getType()) << "|"
            << toEpoch(vehicle.getEntryTime()) << "|" << vehicle.getAllocatedSlotId() << "\n";
    }

    out << "[TRANSACTIONS]\n";
    for (const ParkingTransaction& tx : billing.getTransactionHistory()) {
        out << tx.getTransactionId() << "|" << tx.getPlateNumber() << "|"
            << vehicleTypeToken(tx.getVehicleType()) << "|" << tx.getSlotId() << "|"
            << tx.getZone() << "|" << toEpoch(tx.getEntryTime()) << "|"
            << toEpoch(tx.getExitTime()) << "|" << tx.getDurationMinutes() << "|"
            << tx.getBilledHours() << "|" << tx.getHourlyRate() << "|" << tx.getTotalFee() << "|"
            << tx.getPaymentMethod() << "|" << (tx.isPaymentReceived() ? "1" : "0") << "\n";
    }

    out << "[DAILY_REVENUE]\n";
    for (const auto& pair : billing.getDailyRevenueIndex()) {
        int count = 0;
        auto countIt = billing.getDailyTransactionCount().find(pair.first);
        if (countIt != billing.getDailyTransactionCount().end()) {
            count = countIt->second;
        }
        out << pair.first << "|" << pair.second << "|" << count << "\n";
    }

    out << "[END]\n";
    return static_cast<bool>(out);
}

bool DataPersistence::load(SlotManager& slots, VehicleManager& vehicles,
                           BillingManager& billing, const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(in, line) || line != kFormatVersion) {
        return false;
    }

    slots.clearAll();
    vehicles.clearAll();
    billing.clearAll();

    enum class Section { None, Tariffs, Slots, Active, Transactions, DailyRevenue };
    Section section = Section::None;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (line == "[TARIFFS]") { section = Section::Tariffs; continue; }
        if (line == "[SLOTS]") { section = Section::Slots; continue; }
        if (line == "[ACTIVE]") { section = Section::Active; continue; }
        if (line == "[TRANSACTIONS]") { section = Section::Transactions; continue; }
        if (line == "[DAILY_REVENUE]") { section = Section::DailyRevenue; continue; }
        if (line == "[END]") { break; }

        if (section == Section::Tariffs) {
            if (line.rfind("NEXT_TX_ID", 0) == 0) {
                billing.setNextTransactionId(std::stoi(line.substr(11)));
                continue;
            }
            std::istringstream stream(line);
            std::string token;
            int rate = 0;
            stream >> token >> rate;
            VehicleType type{};
            if (parseVehicleTypeToken(token, type)) {
                billing.getTariffManager().setRate(type, rate);
            }
        } else if (section == Section::Slots) {
            auto parts = split(line, '|');
            if (parts.size() != 4) continue;
            VehicleType type{};
            SlotStatus status{};
            if (!parseVehicleTypeToken(parts[1], type)) continue;
            if (!parseSlotStatusToken(parts[3], status)) continue;
            slots.restoreSlot(parts[0], type, parts[2], status);
        } else if (section == Section::Active) {
            auto parts = split(line, '|');
            if (parts.size() != 4) continue;
            VehicleType type{};
            if (!parseVehicleTypeToken(parts[1], type)) continue;
            vehicles.restoreSession(parts[0], type, fromEpoch(std::stoll(parts[2])), parts[3]);
        } else if (section == Section::Transactions) {
            auto parts = split(line, '|');
            if (parts.size() != 13) continue;
            VehicleType type{};
            if (!parseVehicleTypeToken(parts[2], type)) continue;
            ParkingTransaction tx(
                std::stoi(parts[0]), parts[1], type, parts[3], parts[4],
                fromEpoch(std::stoll(parts[5])), fromEpoch(std::stoll(parts[6])),
                std::stoi(parts[7]), std::stoi(parts[8]), std::stoi(parts[9]),
                std::stoi(parts[10]), parts[11], parts[12] == "1");
            billing.restoreTransaction(tx);
        } else if (section == Section::DailyRevenue) {
            auto parts = split(line, '|');
            if (parts.size() != 3) continue;
            billing.restoreDailyRevenue(parts[0], std::stoll(parts[1]), std::stoi(parts[2]));
        }
    }

    return true;
}
