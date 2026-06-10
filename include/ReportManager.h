#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include "BillingManager.h"
#include "Report.h"
#include "SlotManager.h"
#include "VehicleManager.h"

#include <memory>
#include <string>

class ReportManager {
private:
    const SlotManager& slotManager;
    const VehicleManager& vehicleManager;
    const BillingManager& billingManager;

public:
    ReportManager(const SlotManager& slots, const VehicleManager& vehicles,
                  const BillingManager& billing);

    void displayAvailableSlotsReport() const;
    void displayParkedVehiclesReport() const;
    void displayDailyRevenueReport() const;
    void displayVehicleHistoryReport(const std::string& plateNumber) const;

    std::unique_ptr<Report> createAvailableSlotsReport() const;
    std::unique_ptr<Report> createParkedVehiclesReport() const;
    std::unique_ptr<Report> createDailyRevenueReport() const;
};

#endif
