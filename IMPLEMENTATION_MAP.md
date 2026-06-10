# Implementation Map — Prompt Requirements & Extra Features

This document maps **every assignment requirement** to the exact file(s) and class(es) that implement it, plus **features added beyond the minimum prompt**.

---

## Integrated situation — business capabilities

| Requirement | Implementation | File(s) |
|-------------|----------------|---------|
| Parking slot configuration & availability | `SlotManager` | `include/SlotManager.h`, `src/SlotManager.cpp` |
| Vehicle entry registration | `VehicleManager::registerEntry` | `src/VehicleManager.cpp` |
| Duration tracking & fee calculation | `BillingManager::recordExit`, `calculateBilledHours` | `src/BillingManager.cpp` |
| Vehicle exit & payment recording | `VehicleManager::processExit`, `ParkingTransaction` | `src/VehicleManager.cpp`, `include/ParkingTransaction.h` |
| Reports (available, parked, history, revenue) | `ReportManager` + `Report` hierarchy | `src/ReportManager.cpp`, `include/Report.h` |
| In-memory only (no database) | All storage in STL containers | Managers (see DSA section) |
| Real-time slot visibility | Query `availableByType` index on demand | `SlotManager::allocateSlot`, menu **4** |

---

## Task 1 — Parking Slot Configuration

| Requirement | Implementation | File(s) |
|-------------|----------------|---------|
| Slot ID (unique) | `unordered_map<string, ParkingSlot>` keys | `SlotManager::slots` |
| Supported vehicle type | `VehicleType` enum on `ParkingSlot` | `include/ParkingSlot.h`, `include/Enums.h` |
| Zone | `ParkingSlot::zone`, `slotsByZone` index | `SlotManager` |
| Slot status Available/Occupied | `SlotStatus`, updated on entry/exit | `ParkingSlot`, `SlotManager` |
| Uniquely identified | Duplicate check in `addSlot` | `src/SlotManager.cpp` |
| Appropriate data structure | `unordered_map` O(1) lookup | `Core_compents_DSA_Justification.md` |
| Menu: add/remove/view | Options **1**, **2**, **3** | `src/main.cpp` |

---

## Task 2 — Vehicle Entry Management

| Requirement | Implementation | File(s) |
|-------------|----------------|---------|
| Plate number | Stored on `Vehicle` / session map key | `include/Vehicle.h`, `VehicleManager` |
| Vehicle type | `VehicleType` + `VehicleFactory` | `src/VehicleFactory.cpp` |
| Entry time | `chrono::system_clock::time_point` | `Vehicle::entryTime` |
| Allocated parking slot | `allocateSlot` → `allocatedSlotId` | `SlotManager`, `VehicleManager` |
| No double parking | `activeSessions.find(plate)` | `src/VehicleManager.cpp` |
| Graceful no-slot handling | Message when `allocateSlot` returns empty | `src/VehicleManager.cpp` |
| Data structures for records | `unordered_map<string, unique_ptr<Vehicle>>` | `VehicleManager` |
| Menu: register entry | Option **5** | `src/main.cpp` |

---

## Task 3 — Parking Duration & Fee Calculation

| Requirement | Implementation | File(s) |
|-------------|----------------|---------|
| Duration from entry/exit | `duration_cast<minutes>(exit - entry)` | `BillingManager::recordExit` |
| Rates by vehicle type | `TariffManager::getRate` | `src/TariffManager.cpp` |
| Total fee | `Vehicle::calculateFee` (polymorphic) | `src/Vehicle.cpp` |
| Update parking price | `updateParkingPrice`, menu **9** | `BillingManager`, `main.cpp` |
| Past records unaffected | Rate/fee snapshotted in `ParkingTransaction` | `include/ParkingTransaction.h` |
| Default Motorcycle 500 | `TariffManager` constructor | `src/TariffManager.cpp` |
| Default Car 1000 | `TariffManager` constructor | `src/TariffManager.cpp` |
| Fees only on exit | Fee logic only in `recordExit` | `BillingManager` |
| Partial hours → full hours | `(durationMinutes + 59) / 60` | `BillingManager::calculateBilledHours` |
| Menu: view tariffs | Option **8** | `src/main.cpp` |

---

## Task 4 — Vehicle Exit & Parking Update

| Requirement | Implementation | File(s) |
|-------------|----------------|---------|
| Release occupied slot | `SlotManager::releaseSlot` | `src/SlotManager.cpp` |
| Calculate & display fee | `recordExit` + exit summary print | `VehicleManager::processExit` |
| Update system records | Remove session, update slot, append TX | `VehicleManager`, `BillingManager` |
| Store transaction | `vector<ParkingTransaction>` + indexes | `BillingManager` |
| Menu: process exit | Option **6** | `src/main.cpp` |

---

## Task 5 — Operational Reports (implicit fifth task)

| Report | Implementation | Menu |
|--------|----------------|------|
| Available slots | `SlotsReport` | **4** |
| Parked vehicles | `ParkedVehiclesReport` | **7** |
| Vehicle history | `displayVehicleHistory` / `VehicleHistoryReport` | **10** |
| Daily revenue | `DailyRevenueReport` + `dailyRevenueIndex` | **12** |

---

## Design & DSA instructions

| Instruction | Implementation | File(s) |
|-------------|----------------|---------|
| System architecture | Three-layer design + 4 managers | `ParkingSystem`, diagram SVG |
| Core components identified | `IMPLEMENTATION_MAP.md`, `Core_compents_DSA_Justification.md` | This file |
| Linear / non-linear structures | `vector`, `unordered_map`, `map`, `set` | All managers |
| Justify structure choices | DSA justification doc | `Core_compents_DSA_Justification.md` |
| Insert / delete / update / traverse | Slot CRUD, session map, TX append, report traversal | Manager `.cpp` files |
| Menu-driven interface | `main.cpp` switch menu | `src/main.cpp` |
| Test inputs provided | Menu **13**, **14**, **16**, `README.md` test matrix | Multiple |
| README compile/run/rates/menu | `README.md` | Root |

---

## OOP principles — where implemented

| Principle | Example | File(s) |
|-----------|---------|---------|
| **Encapsulation** | Private fields + getters on `ParkingSlot`, `ParkingTransaction`, managers | All model/manager headers |
| **Abstraction** | `ParkingSystem` façade hides internal maps | `include/ParkingSystem.h` |
| **Inheritance** | `Vehicle` → `MotorcycleVehicle`, `CarVehicle`, `TruckVehicle` | `include/Vehicle.h`, `src/Vehicle.cpp` |
| **Polymorphism** | `virtual calculateFee()` — Truck adds 25% surcharge | `src/Vehicle.cpp` |
| **Polymorphism** | `Report::generate()` → `SlotsReport`, etc. | `src/ReportManager.cpp` |
| **Factory** | `VehicleFactory::create` by type | `src/VehicleFactory.cpp` |

---

## Extra features (beyond minimum prompt)

| Feature | Why added | File(s) | Menu |
|---------|-----------|---------|------|
| **Manager-based architecture** | Cleaner OOP / single responsibility | `SlotManager`, `VehicleManager`, `BillingManager`, `ReportManager` | — |
| **Zone search index** | Faster zone-based queries | `SlotManager::slotsByZone` | **15** |
| **O(log n) slot allocation** | `set` per vehicle type | `SlotManager::availableByType` | — |
| **Plate history index** | O(1) lookup of TX indices by plate | `BillingManager::plateHistoryIndex` | **10** |
| **Daily revenue index** | O(1) revenue read by date | `BillingManager::dailyRevenueIndex` | **12** |
| **Transaction ID** | Traceable payments | `ParkingTransaction::transactionId` | **6**, **11** |
| **Payment method & status** | Full payment recording | `ParkingTransaction` | **6** |
| **Truck 25% surcharge** | Demonstrates real polymorphic `calculateFee` | `TruckVehicle::calculateFee` | **16** |
| **InputValidator module** | Heavy validation, no crashes | `InputValidator.cpp` | All inputs |
| **InputReader module** | Safe console I/O + EOF handling | `InputReader.cpp` | All menus |
| **Test entry/exit times** | Billing edge-case testing without waiting | `main.cpp` handlers | **5**, **6** |
| **Billing self-test** | Automated PASS/FAIL checks | `ParkingSystem::runBillingSelfTest` | **16** |
| **Automated demo** | One-click full feature demo | `ParkingSystem::runAutomatedDemo` | **14** |
| **Sample data loader** | Quick setup, skips duplicates | `loadSampleData` in `main.cpp` | **13** |
| **Architecture SVG** | Visual design artifact | `smart_parking_system_architecture.svg` | — |
| **Walkthrough checklist** | Manual verification guide | `WALKTHROUGH.md` | — |
| **UCRT64 build scripts** | Fixes MSYS2 toolchain conflict | `build.bat`, `build.ps1` | — |

---

## File index (quick reference)

```
include/Enums.h              — VehicleType, SlotStatus
include/ParkingSlot.h        — Slot model
include/Vehicle.h            — Vehicle inheritance hierarchy
include/VehicleFactory.h     — Creates typed vehicles
include/ParkingTransaction.h — Completed parking records
include/TariffManager.h      — Hourly rates
include/SlotManager.h        — Slot CRUD + allocation
include/VehicleManager.h     — Entry/exit sessions
include/BillingManager.h     — Fees + history + revenue
include/ReportManager.h      — Polymorphic reports
include/ParkingSystem.h      — Façade API
include/Report.h               — Abstract report
include/InputValidator.h     — Validation rules
include/InputReader.h        — Safe console input
include/TimeUtils.h            — Date/time formatting
src/*.cpp                    — Implementations
src/main.cpp                 — Presentation layer / menu
```
