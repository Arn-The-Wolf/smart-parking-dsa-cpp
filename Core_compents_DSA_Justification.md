# Core Components and DSA Justification

## Diagram

See `smart_parking_system_architecture.svg` — titled **Kigali Smart Parking Management System** layered architecture.

## Three-layer structure

### Presentation layer (`main.cpp`)
Console menu with validated input (options 0–16), payment method capture, optional test entry/exit times for billing verification, and demo/self-test runners. No business logic.

### Business logic layer — `ParkingSystem` façade + four managers

| Manager | Responsibility | Key operations |
|---------|----------------|----------------|
| **SlotManager** | Slot CRUD, zone search, availability index | `addSlot`, `removeSlot`, `allocateSlot`, `releaseSlot`, `displaySlotsByZone` |
| **VehicleManager** | Active session lifecycle | `registerEntry`, `processExit`, polymorphic `Vehicle` storage |
| **BillingManager** | Tariffs, fee calculation, payment records | `recordExit`, `updateParkingPrice`, history & revenue indexes |
| **ReportManager** | Polymorphic reporting | `create*Report`, `display*Report` via `Report` interface |

### Domain models
- `ParkingSlot` — slot configuration and status
- `Vehicle` hierarchy — `MotorcycleVehicle`, `CarVehicle`, `TruckVehicle` with virtual `calculateFee()`
- `ParkingTransaction` — immutable record with transaction ID, fee snapshot, payment method, payment status
- `TariffManager` — current hourly rates
- `Report` — abstract report interface

---

## Data structure choices (6 structures)

| Structure | Location | Operations | Justification |
|-----------|----------|------------|---------------|
| `unordered_map<string, ParkingSlot>` | SlotManager | O(1) lookup/insert/delete | Unique slot IDs, fast status updates |
| `map<VehicleType, set<string>>` | SlotManager | O(log n) min-slot pick | Allocates lowest slot ID without full scan |
| `unordered_map<string, vector<string>>` | SlotManager | O(1) zone lookup | Zone-based slot search (menu 15) |
| `unordered_map<string, unique_ptr<Vehicle>>` | VehicleManager | O(1) session lookup | Duplicate-entry prevention; polymorphic vehicles |
| `vector<ParkingTransaction>` + `unordered_map<string, vector<size_t>>` | BillingManager | O(1) append; O(1) plate history | Immutable history; indexed plate lookup |
| `map<string, long long>` daily revenue + `unordered_map<VehicleType, int>` tariffs | BillingManager | O(log n) date revenue; O(1) rate | Fast daily reports; decoupled tariff updates |

---

## OOP design

| Principle | Implementation |
|-----------|----------------|
| **Encapsulation** | All model and manager classes hide internal state |
| **Abstraction** | `ParkingSystem` exposes a simple API; `Report` and `Vehicle` abstract interfaces |
| **Inheritance** | `Vehicle` → `MotorcycleVehicle`, `CarVehicle`, `TruckVehicle` |
| **Polymorphism** | `calculateFee()` on vehicles; `generate()` / `getTitle()` on `Report` subclasses |

---

## Key algorithms

- **Ceiling-hour billing:** `(durationMinutes + 59) / 60`
- **Slot allocation:** `set<string>::begin()` per vehicle type — O(log n)
- **Duplicate check:** hash lookup on plate — O(1)
- **Daily revenue:** pre-aggregated in `dailyRevenueIndex` on each exit — O(1) read
- **Plate history:** `plateHistoryIndex` — O(1) to find indices, O(k) to print k records

---

## Default tariffs

| Type | Rate (RWF/hr) | Source |
|------|---------------|--------|
| Motorcycle | 500 | Assignment brief |
| Car | 1,000 | Assignment brief |
| Truck | 1,000 | System default (brief specifies Motorcycle & Car only); updatable via menu 9 |

Price updates apply only at exit time. Completed `ParkingTransaction` records store the rate and fee permanently.

---

## Input validation

| Module | Role |
|--------|------|
| `InputReader` | Safe console reads with EOF handling and range checks |
| `InputValidator` | Reusable rules for slot ID, zone, plate, rate, minutes, payment |

Validation runs in both the presentation layer (`main.cpp`) and business layer (managers), so invalid data never corrupts in-memory structures.

---

## Payment recording

Each exit stores:
- `transactionId` (auto-increment)
- `paymentMethod` (Cash / Mobile Money / Card)
- `paymentReceived` (bool)

---

## Source file map

```
include/  → headers (models, managers, façade)
src/      → implementations
main.cpp  → presentation layer
```

Managers: `SlotManager`, `VehicleManager`, `BillingManager`, `ReportManager`  
Façade: `ParkingSystem`  
Supporting: `VehicleFactory`, `TimeUtils`, `TariffManager`
