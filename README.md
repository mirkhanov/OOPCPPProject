# Veterinary Practice Management System (VPMS)

A desktop application for managing a veterinary clinic, built with **C++17** and **Qt 6 Widgets**.

## Features

- **Clients & Animals** — manage client records and their animals (Dog, Cat, Bird, Reptile)
- **Visits** — schedule visits, attach services, track total cost
- **Service Catalog** — Consultation, Vaccination, Surgery (with/without anesthesia), Grooming (with pricing levels)
- **Inventory** — stock management with low-stock highlighting
- **Hospitalization** — admit and discharge animals, track daily billing

## Architecture

```
vpms/
├── core/        # Repository<T>, IdGenerator, exception hierarchy
├── animals/     # Animal hierarchy + Owner (Factory pattern)
├── visits/      # Service hierarchy + Visit (Factory pattern)
├── inventory/   # InventoryItem, HospitalizationRecord
├── gui/         # Qt Widgets — 5-tab interface
└── data/        # File-based persistence (*.dat)

tests/           # Qt Test — 48 unit tests
docs/            # Project report (PDF)
```

**Key design decisions:**
- No database — all persistence via `std::fstream` and per-entity `serialize`/`deserialize`
- `ClinicService` acts as a Facade — GUI never touches repositories directly
- `Repository<T>` is a generic template that handles CRUD and file I/O for any `ISerializable` type

## OOP Concepts

| Concept | Where |
|---|---|
| Inheritance | `Animal` → Dog/Cat/Bird/Reptile; `Service` → Consultation/Vaccination/Surgery/Grooming |
| Polymorphism | `getFinalPrice()`, `calculateTreatmentCost()`, `serialize()`/`deserialize()` |
| Operator overloading | `+=`/`-=` (InventoryItem), `<`/`==` (Visit, Owner, InventoryItem) |
| Exception handling | Custom hierarchy thrown by service/repo layer, caught at GUI boundary |
| File I/O | `std::fstream`-based serialization, `---` sentinel delimiters |
| Design patterns | Facade (ClinicService), Factory Method (AnimalFactory, ServiceFactory) |

## Build

```bash
# Application
cd vpms/
qmake vpms.pro && make -j$(nproc)
open vpms.app        # macOS

# Tests
cd tests/
qmake tests.pro && make -j$(nproc)
./vpms_tests
```

**Requirements:** Qt 6, C++17 compiler (Clang / GCC), qmake

## Test Results

```
TestAnimals   — 15 passed
TestVisits    — 17 passed
TestInventory — 12 passed
TestCore      — 12 passed
─────────────────────────
Total: 48 passed, 0 failed
```
