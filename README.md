# Kigali Smart Parking Management System

In-memory C++17 parking system for Kigali City — slot configuration, vehicle entry/exit, billing, payment recording, and operational reports.

## Architecture

| Layer | Components |
|-------|------------|
| Presentation | `main.cpp`, `InputReader`, `InputValidator` |
| Façade | `ParkingSystem` |
| Managers | `SlotManager`, `VehicleManager`, `BillingManager`, `ReportManager` |
| Models | `ParkingSlot`, `Vehicle` hierarchy, `ParkingTransaction` |

See `smart_parking_system_architecture.svg` and `Core_compents_DSA_Justification.md`.

## Default Parking Rates

| Vehicle Type | Rate (RWF/hour) | Source |
|--------------|-----------------|--------|
| Motorcycle   | 500             | Assignment brief |
| Car          | 1,000           | Assignment brief |
| Truck        | 1,000           | System default (brief lists Motorcycle & Car only; update via menu 9) |

## Input Validation

All user input is validated before reaching business logic:

- Menu options restricted to **0–16**
- Slot ID: alphanumeric + `-`/`_`, max 10 chars
- Zone: alphanumeric + spaces/hyphens, max 30 chars
- Plate: 3–15 chars, alphanumeric + spaces/hyphens
- Hourly rate: 1 – 1,000,000 RWF
- Parking minutes: 1 – 525,600 (max 1 year)
- Entry/exit times cannot be in the future
- Exit cannot be before entry
- Payment method must be Cash, Mobile Money, or Card
- Pre-checks: no slots → blocks entry; no parked vehicles → blocks exit

## Compile and Run

**Important (MSYS2):** Use the **UCRT64** toolchain. If you see `clock_gettime64` errors, run `.\build.ps1` instead of raw `g++` — it pins `C:\msys64\ucrt64\bin`.

```powershell
cd "c:\Users\RUYANGE Arnold\NE PRACTICALS\DSA"
.\build.ps1
.\smart_parking.exe
```

Or use `.\build.bat` (also pins UCRT64).

## Menu Options

| Option | Action |
|--------|--------|
| 1 | Add parking slot |
| 2 | Remove unoccupied slot |
| 3 | View all slots |
| 4 | Available slots report (polymorphic) |
| 5 | Search slots by zone |
| 6 | Register entry (optional test entry time) |
| 7 | Exit + payment recording |
| 8 | Parked vehicles report (polymorphic) |
| 9 | View tariffs |
| 10 | Update parking price |
| 11 | Vehicle history |
| 12 | All transactions |
| 13 | Daily revenue report (polymorphic) |
| 14 | Load sample data |
| 15 | Automated demo |
| 16 | Billing edge-case self test |
| 0 | Exit (Y/N confirmation) |

## Formal Test Matrix

| # | Input / action | Expected result |
|---|----------------|-----------------|
| T1 | Add slot `C01`, Car, Zone-B | Success |
| T2 | Add duplicate `C01` | Error: already exists |
| T3 | Invalid slot ID `@@` | Validation error, re-prompt |
| T4 | Entry with 15 min test time, exit now | 1 billed hour |
| T5 | Duplicate entry same plate | Error: already parked |
| T6 | Entry when no matching slots | Graceful no-slot message |
| T7 | Exit 80 min after entry (test mode) | 2 billed hours |
| T8 | Update Car rate, new exit | New rate applied; old TX unchanged |
| T9 | Exit before entry (test mode) | Error rejected |
| T10 | Menu 16 self test | All PASS lines |
| T11 | Menu `abc` / `99` | Re-prompt, no crash |
| T12 | Exit unknown plate | Error: not parked |
| T13 | Rate `0` or `-5` | Validation error |
| T14 | Zone search `Zone-B` | Lists zone slots only |

## Tasks Covered

| Task | Feature |
|------|---------|
| 1 | Slot configuration (ID, type, zone, status) |
| 2 | Vehicle entry with duplicate prevention |
| 3 | Duration, fees, tariff updates, ceiling billing |
| 4 | Exit, slot release, payment recording, transactions |
| 5 | Reports: available slots, parked vehicles, history, daily revenue |
