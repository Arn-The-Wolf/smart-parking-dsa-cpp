#ifndef BILLING_MANAGER_H
#define BILLING_MANAGER_H

#include "ParkingTransaction.h"
#include "TariffManager.h"
#include "Vehicle.h"

#include <chrono>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// Handles tariffs, fee calculation, immutable transaction history, and revenue indexes.
class BillingManager {private:
    TariffManager tariffManager;
    std::vector<ParkingTransaction> transactionHistory;
    std::unordered_map<std::string, std::vector<size_t>> plateHistoryIndex;
    std::map<std::string, long long> dailyRevenueIndex;
    std::map<std::string, int> dailyTransactionCount;
    int nextTransactionId;

    static int calculateBilledHours(int durationMinutes);

public:
    BillingManager();

    bool updateParkingPrice(VehicleType type, int newRate);
    void displayCurrentTariffs() const;
    int getRate(VehicleType type) const;

    bool recordExit(const Vehicle& vehicle, const std::string& zone,
                    const std::chrono::system_clock::time_point& exitTime,
                    const std::string& paymentMethod, bool paymentReceived,
                    ParkingTransaction& outTransaction);

    void displayVehicleHistory(const std::string& plateNumber) const;
    void displayAllHistory() const;
    void displayDailyRevenue() const;
    long long getRevenueForDate(const std::string& dateKey) const;

    const std::vector<ParkingTransaction>& getTransactionHistory() const { return transactionHistory; }
    const TariffManager& getTariffManager() const { return tariffManager; }
    int getTransactionCount() const { return static_cast<int>(transactionHistory.size()); }
};

#endif
