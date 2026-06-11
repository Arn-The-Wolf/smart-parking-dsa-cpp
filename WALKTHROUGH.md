# Walkthrough — Verify All Assignment Tasks

Use this checklist after building with `.\build.ps1` or `.\build.bat`, then run `.\smart_parking.exe`.

---

## Before you start

1. Open PowerShell in the `DSA` folder.
2. Build:
   ```powershell
   .\build.ps1
   ```
3. Run:
   ```powershell
   .\smart_parking.exe
   ```

---

## Task 1 — Parking Slot Configuration

| Step | Menu | Input | Expected result |
|------|------|-------|-----------------|
| 1.1 | **14** | — | 7 sample slots loaded |
| 1.2 | **3** | — | Table shows Slot ID, Vehicle Type, Zone, **Status** (Available/Occupied) |
| 1.3 | **1** | Slot `X99`, type **2** (Car), Zone `Zone-X` | Success message |
| 1.4 | **1** | Slot `X99` again | Error: already exists (unique ID enforced) |
| 1.5 | **1** | Slot `@@` | Validation error, re-prompt |
| 1.6 | **5** | Zone `Zone-B` | Lists only Zone-B slots |
| 1.7 | **4** | — | Polymorphic available-slots report |

**Pass criteria:** Unique slot IDs, all four attributes visible, zone search works.

---

## Task 2 — Vehicle Entry Management

| Step | Menu | Input | Expected result |
|------|------|-------|-----------------|
| 2.1 | **6** | Plate `RAB100A`, type **2** (Car), test time **n** | Entry success with slot + entry time |
| 2.2 | **8** | — | `RAB100A` appears in parked vehicles report |
| 2.3 | **6** | Plate `RAB100A` again | Error: already parked |
| 2.4 | **6** | Fill all Car slots, then register another Car | Graceful "no available slot" message |
| 2.5 | **6** | Plate `BAD`, test time **y**, minutes `5` | Entry with custom past entry time |

**Pass criteria:** Plate, type, entry time, allocated slot recorded; no double parking; no crash when full.

---

## Task 3 — Parking Duration & Fee Calculation

| Step | Menu | Input | Expected result |
|------|------|-------|-----------------|
| 3.1 | **9** | — | Motorcycle **500**, Car **1000**, Truck **1000** RWF/hr |
| 3.2 | **6** | Plate `TEST15`, Car, test entry **y**, **15** min ago | — |
| 3.3 | **7** | Plate `TEST15`, Cash, paid **y**, test exit **n** | **1 billed hour**, fee = 1000 RWF |
| 3.4 | **6** | Plate `TEST80`, Car, test entry **y**, **80** min ago | — |
| 3.5 | **7** | Plate `TEST80`, Mobile Money, paid **y** | **2 billed hours**, fee = 2000 RWF |
| 3.6 | **10** | Car, new rate **1500** | Success; note about future exits only |
| 3.7 | **6** | Plate `TESTNEW`, Car, entry **n** | — |
| 3.8 | **7** | Plate `TESTNEW`, Card, paid **y** | Fee uses **1500** RWF/hr |
| 3.9 | **11** | Plate `TEST15` | Old transaction still shows **1000** RWF/hr rate |
| 3.10 | **16** | — | Self-test prints multiple **PASS** lines |

**Pass criteria:** Ceiling billing, fees only on exit, price updates don't alter old records.

---

## Task 4 — Vehicle Exit & Parking Update

| Step | Menu | Input | Expected result |
|------|------|-------|-----------------|
| 4.1 | **7** | Parked plate, payment method, paid **y** | Exit summary with TX#, fee, payment status |
| 4.2 | **4** | — | Slot now available again |
| 4.3 | **12** | — | Transaction stored with TX#, plate, fee |
| 4.4 | **7** | Unknown plate `NOPE123` | Error: not currently parked |
| 4.5 | **7** | Test exit before entry (exit minutes > entry minutes ago) | Error: exit before entry |

**Pass criteria:** Slot released, fee displayed, history updated, payment recorded.

---

## Task 5 — Operational Reports

| Step | Menu | Input | Expected result |
|------|------|-------|-----------------|
| 5.1 | **4** | — | Available slots (polymorphic `Report`) |
| 5.2 | **8** | — | Parked vehicles report |
| 5.3 | **11** | Any plate with history | Vehicle history |
| 5.4 | **13** | — | Daily revenue for today |
| 5.5 | **15** | — | Full automated demo runs without crash |

**Pass criteria:** All four report types work.

---

## Design & DSA requirements

| Requirement | How to verify |
|-------------|---------------|
| Architecture diagram | Open `smart_parking_system_architecture.svg` |
| DSA justification | Read `Core_compents_DSA_Justification.md` |
| Input validation | Try menu `abc`, rate `-5`, empty slot ID — no crash |
| OOP inheritance | Menu **16** → Truck surcharge **PASS** (25% extra via `TruckVehicle::calculateFee`) |
| OOP polymorphism | Menus **4**, **8**, **13** use `Report::generate()` |

---

## Quick 5-minute full pass

```
14 → 15 → 16 → 9 → 0 (confirm exit with y)
```

If options **15** and **16** complete with PASS lines and no crashes, all core tasks are met.

---

## Known assignment limitations (by design)

- Console only (not a web/multi-user system)
- In-memory only (data lost on exit)
- No separate C++ unit-test binary (menu 16 + this walkthrough instead)
