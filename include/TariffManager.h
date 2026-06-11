#ifndef TARIFF_MANAGER_H
#define TARIFF_MANAGER_H

#include "Enums.h"
#include <unordered_map>

class TariffManager {
private:
    std::unordered_map<VehicleType, int> hourlyRates;

public:
    TariffManager();

    int getRate(VehicleType type) const;
    bool updateRate(VehicleType type, int newRate);
    void setRate(VehicleType type, int rate);
    void displayRates() const;
};

#endif
