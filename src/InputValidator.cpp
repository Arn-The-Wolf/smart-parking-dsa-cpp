#include "InputValidator.h"

#include <cctype>

namespace InputValidator {

bool isValidSlotId(const std::string& slotId, std::string& errorMessage) {
    if (slotId.empty()) {
        errorMessage = "Slot ID cannot be empty.";
        return false;
    }
    if (slotId.length() > kMaxSlotIdLength) {
        errorMessage = "Slot ID must be at most " + std::to_string(kMaxSlotIdLength) + " characters.";
        return false;
    }
    for (char ch : slotId) {
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != '_') {
            errorMessage = "Slot ID may only contain letters, digits, hyphen, or underscore.";
            return false;
        }
    }
    errorMessage.clear();
    return true;
}

bool isValidZone(const std::string& zone, std::string& errorMessage) {
    if (zone.empty()) {
        errorMessage = "Zone cannot be empty.";
        return false;
    }
    if (zone.length() > kMaxZoneLength) {
        errorMessage = "Zone must be at most " + std::to_string(kMaxZoneLength) + " characters.";
        return false;
    }
    for (char ch : zone) {
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != ' ') {
            errorMessage = "Zone may only contain letters, digits, spaces, or hyphens.";
            return false;
        }
    }
    errorMessage.clear();
    return true;
}

bool isValidPlateNumber(const std::string& plate, std::string& errorMessage) {
    if (plate.length() < kMinPlateLength) {
        errorMessage = "Plate number must be at least " + std::to_string(kMinPlateLength) + " characters.";
        return false;
    }
    if (plate.length() > kMaxPlateLength) {
        errorMessage = "Plate number must be at most " + std::to_string(kMaxPlateLength) + " characters.";
        return false;
    }
    for (char ch : plate) {
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '-' && ch != ' ') {
            errorMessage = "Plate may only contain letters, digits, spaces, or hyphens.";
            return false;
        }
    }
    errorMessage.clear();
    return true;
}

bool isValidHourlyRate(int rate, std::string& errorMessage) {
    if (rate <= 0) {
        errorMessage = "Hourly rate must be greater than zero.";
        return false;
    }
    if (rate > kMaxHourlyRate) {
        errorMessage = "Hourly rate cannot exceed " + std::to_string(kMaxHourlyRate) + " RWF.";
        return false;
    }
    errorMessage.clear();
    return true;
}

bool isValidParkingMinutes(int minutes, std::string& errorMessage) {
    if (minutes <= 0) {
        errorMessage = "Parking duration must be at least 1 minute.";
        return false;
    }
    if (minutes > kMaxParkingMinutes) {
        errorMessage = "Parking duration is unreasonably large. Check your input.";
        return false;
    }
    errorMessage.clear();
    return true;
}

bool isValidPaymentMethod(const std::string& method, std::string& errorMessage) {
    if (method != "Cash" && method != "Mobile Money" && method != "Card") {
        errorMessage = "Payment method must be Cash, Mobile Money, or Card.";
        return false;
    }
    errorMessage.clear();
    return true;
}

}  // namespace InputValidator
