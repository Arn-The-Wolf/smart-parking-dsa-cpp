#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>

// Centralized input validation used by the presentation layer and business layer.
// Every function returns true when the value is acceptable.
namespace InputValidator {

constexpr int kMaxSlotIdLength = 10;
constexpr int kMaxZoneLength = 30;
constexpr int kMaxPlateLength = 15;
constexpr int kMinPlateLength = 3;
constexpr int kMaxHourlyRate = 1000000;
constexpr int kMaxParkingMinutes = 525600;  // 365 days

bool isValidSlotId(const std::string& slotId, std::string& errorMessage);
bool isValidZone(const std::string& zone, std::string& errorMessage);
bool isValidPlateNumber(const std::string& plate, std::string& errorMessage);
bool isValidHourlyRate(int rate, std::string& errorMessage);
bool isValidParkingMinutes(int minutes, std::string& errorMessage);
bool isValidPaymentMethod(const std::string& method, std::string& errorMessage);

}  // namespace InputValidator

#endif
