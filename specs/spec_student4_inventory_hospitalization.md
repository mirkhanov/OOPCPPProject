# Student 4 — Inventory & Hospitalization Module

## CRITICAL — Exact Signatures You Must Match

These are copied directly from `ClinicService.cpp`. If your names or signatures differ even slightly, the project will not compile.

### InventoryItem — must have exactly these methods and operators:
```cpp
int         getId()        const;   // from ISerializable
std::string getName()      const;
int         getQuantity()  const;

InventoryItem& operator+=(int qty);
InventoryItem& operator-=(int qty);
```

### InventoryItem constructor must match:
```cpp
InventoryItem(int id, const std::string& name, int quantity,
              double unitPrice, const std::string& category);
```

### HospitalizationRecord — must have exactly these methods:
```cpp
int                     getId()            const;   // from ISerializable
int                     getAnimalId()      const;
std::string             getAdmitDate()     const;
std::string             getDischargeDate() const;
double                  getDailyRate()     const;
const std::vector<int>& getServiceIds()    const;
bool                    isActive()         const;   // returns dischargeDate.empty()

void setDischargeDate(const std::string& date);
void setTotalBill(double bill);
void addServiceId(int serviceId);
```

### HospitalizationRecord constructor must match:
```cpp
HospitalizationRecord(int id, int animalId, const std::string& ward,
                      const std::string& admitDate, double dailyRate);
```

## Your Role
You implement stock management and animal hospitalization records.
Your classes are stored by Student 1's `Repository<T>` and called through `ClinicService`.

---

## Dependencies (wait for Student 1 to share these first)
- `core/ISerializable.h` — your classes must inherit this
- `core/exceptions.h` — especially `InsufficientStockException`
- `core/Repository.h` — use directly (no polymorphism needed here, no factory required)

Do not write your own file I/O. Only implement `serialize` / `deserialize`.

---

## Folder Structure

```
inventory/
├── InventoryItem.h / InventoryItem.cpp
├── HospitalizationRecord.h / HospitalizationRecord.cpp
```

That's it — no factory, no abstract base, no subclasses needed for this module.

---

## Part 1 — InventoryItem

**Files:** `inventory/InventoryItem.h`, `inventory/InventoryItem.cpp`

### Inherits
`ISerializable`

### Fields
```
int         _id
std::string _name
int         _quantity
double      _unitPrice
std::string _category    // e.g. "Medicine", "Equipment", "Consumable"
```

### Constructors
`InventoryItem(int id, std::string name, int quantity, double unitPrice, std::string category)`

Default constructor `InventoryItem()` also required (for Repository deserialization).

### Methods

| Method | Returns | Notes |
|---|---|---|
| `getId()` | `int` | ISerializable requirement |
| `getName()` | `std::string` | |
| `getQuantity()` | `int` | Used by ClinicService::stockOut check |
| `getUnitPrice()` | `double` | |
| `getCategory()` | `std::string` | |
| `setName(std::string)` | `void` | |
| `setUnitPrice(double)` | `void` | |
| `setCategory(std::string)` | `void` | |
| `serialize(std::ostream&)` | `void` | |
| `deserialize(std::istream&)` | `void` | |

### Operator Overloads (required for OOP grade)

**operator+=(int qty)**
- `_quantity += qty`
- Return `InventoryItem&`
- Used by `ClinicService::stockIn`

**operator-=(int qty)**
- `_quantity -= qty`
- Return `InventoryItem&`
- Used by `ClinicService::stockOut` (ClinicService checks before calling, so no need to validate here)

**operator<(const InventoryItem& other)**
- Compare by `_name` alphabetically
- Used when sorting inventory list for display

**operator==(const InventoryItem& other)**
- Compare by `_id`

**operator<<(std::ostream&, const InventoryItem&)**
- Print: `[id] name | qty: X | price: Y | category: Z`
- Used for debug and display

### Serialization Format
```
serialize writes:
  _id\n
  _name\n
  _quantity\n
  _unitPrice\n
  _category\n
  ---\n

deserialize reads in same order, reads and discards "---"
```

---

## Part 2 — HospitalizationRecord

**Files:** `inventory/HospitalizationRecord.h`, `inventory/HospitalizationRecord.cpp`

### Inherits
`ISerializable`

### Fields
```
int                  _id
int                  _animalId
std::string          _ward           // e.g. "Ward A", "ICU"
std::string          _admitDate      // "YYYY-MM-DD"
std::string          _dischargeDate  // "" means still active
double               _dailyRate
std::vector<int>     _serviceIds     // additional services during stay
double               _totalBill      // set on discharge, 0.0 while active
```

### Constructors
`HospitalizationRecord(int id, int animalId, std::string ward, std::string admitDate, double dailyRate)`

Default constructor `HospitalizationRecord()` also required.

### Methods

| Method | Returns | Notes |
|---|---|---|
| `getId()` | `int` | ISerializable requirement |
| `getAnimalId()` | `int` | ClinicService uses this for lookups |
| `getWard()` | `std::string` | |
| `getAdmitDate()` | `std::string` | |
| `getDischargeDate()` | `std::string` | Empty string = still active |
| `getDailyRate()` | `double` | |
| `getServiceIds()` | `const std::vector<int>&` | |
| `getTotalBill()` | `double` | |
| `isActive()` | `bool` | Returns `_dischargeDate.empty()` |
| `setDischargeDate(std::string)` | `void` | Called by ClinicService::dischargeAnimal |
| `setTotalBill(double)` | `void` | Called by ClinicService::dischargeAnimal |
| `addServiceId(int)` | `void` | Append to `_serviceIds` |
| `serialize(std::ostream&)` | `void` | |
| `deserialize(std::istream&)` | `void` | |

### Operator Overloads (required for OOP grade)

**operator<(const HospitalizationRecord& other)**
- Compare by `_admitDate` string (lexicographic, works for YYYY-MM-DD)
- Allows sorting records chronologically

**operator==(const HospitalizationRecord& other)**
- Compare by `_id`

**operator<<(std::ostream&, const HospitalizationRecord&)**
- Print: `[id] animalId=X | ward=Y | admitted=Z | discharged=W | bill=B`

### Serialization Format
```
serialize writes:
  _id\n
  _animalId\n
  _ward\n
  _admitDate\n
  _dischargeDate\n         ← write empty line if not discharged
  _dailyRate\n
  _totalBill\n
  _serviceIds.size()\n
  _serviceIds[0]\n
  _serviceIds[1]\n
  ...
  ---\n

deserialize reads in the same order.
```

For `_dischargeDate`: `std::getline(in, _dischargeDate)` — an empty line means not discharged, which is correct.

---

## OOP Requirements You Demonstrate

| Requirement | Where |
|---|---|
| Operator Overloading | `InventoryItem`: `+=`, `-=`, `<`, `==`, `<<` |
| Operator Overloading | `HospitalizationRecord`: `<`, `==`, `<<` |
| File Handling | `serialize`/`deserialize` on both classes |
| Exception Handling | `InsufficientStockException` thrown in `ClinicService::stockOut` — make sure your `getQuantity()` return is correct so the check works |

Note: Inheritance and polymorphism are demonstrated by other modules. Your contribution to OOP is primarily operator overloading and clean file I/O.

---

## What to Hand Off to Student 1

Once done, tell Student 1:
- `InventoryItem` and `HospitalizationRecord` are ready — `Repository<InventoryItem>` and `Repository<HospitalizationRecord>` work out of the box (no special repository or factory needed)
- Header paths: `#include "inventory/InventoryItem.h"`, `#include "inventory/HospitalizationRecord.h"`

---

## Compile & Test Without GUI

```cpp
// Test operator overloads
InventoryItem item(1, "Amoxicillin", 50, 12.99, "Medicine");
item += 20;
assert(item.getQuantity() == 70);
item -= 10;
assert(item.getQuantity() == 60);
std::cout << item << std::endl;

// Test sorting
InventoryItem a(2, "Bandages", 100, 2.50, "Consumable");
InventoryItem b(3, "Amoxicillin", 60, 12.99, "Medicine");
std::cout << (b < a) << std::endl;   // true — "Amoxicillin" < "Bandages"

// Test serialize round-trip
std::stringstream ss;
item.serialize(ss);
InventoryItem item2;
item2.deserialize(ss);
assert(item2.getId() == item.getId());
assert(item2.getQuantity() == 60);

// Test HospitalizationRecord
HospitalizationRecord rec(1, 5, "Ward A", "2024-03-01", 150.0);
assert(rec.isActive() == true);
rec.setDischargeDate("2024-03-05");
assert(rec.isActive() == false);

// Serialize round-trip
std::stringstream ss2;
rec.addServiceId(3);
rec.addServiceId(7);
rec.serialize(ss2);
HospitalizationRecord rec2;
rec2.deserialize(ss2);
assert(rec2.getServiceIds().size() == 2);
```
