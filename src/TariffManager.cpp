#include "TariffManager.h"
#include <iostream>

TariffManager::TariffManager() {
    // Default tariffs per assignment brief (Motorcycle & Car); Truck uses same starter rate until updated.
    hourlyRates[VehicleType::Motorcycle] = 500;
    hourlyRates[VehicleType::Car] = 1000;
    hourlyRates[VehicleType::Truck] = 1000;
}

int TariffManager::getRate(VehicleType type) const {
    auto it = hourlyRates.find(type);
    if (it != hourlyRates.end()) {
        return it->second;
    }
    return 0;
}

bool TariffManager::updateRate(VehicleType type, int newRate) {
    if (newRate <= 0) {
        return false;
    }
    hourlyRates[type] = newRate;
    return true;
}

void TariffManager::displayRates() const {
    std::cout << "Current Parking Tariffs (RWF per hour):\n";
    std::cout << "  Motorcycle: " << getRate(VehicleType::Motorcycle) << " RWF\n";
    std::cout << "  Car:        " << getRate(VehicleType::Car) << " RWF\n";
    std::cout << "  Truck:      " << getRate(VehicleType::Truck)
              << " RWF (default; update via menu 9 if needed)\n";
}
