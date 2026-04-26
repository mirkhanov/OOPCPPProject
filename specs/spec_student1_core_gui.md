# Student 1 — Core Architecture + GUI

## Your Role
You are the foundation of the entire project. Every other student depends on your code.
Build Core first, share the headers early, then build GUI while others finish their modules.

---

## Folder Structure

```
vpms/
├── vpms.pro
├── main.cpp
├── core/
│   ├── ISerializable.h
│   ├── Repository.h          (template — full implementation in .h)
│   ├── IdGenerator.h / .cpp
│   ├── ClinicService.h / .cpp
│   └── exceptions.h
├── animals/                  (Student 2 fills this)
├── visits/                   (Student 3 fills this)
├── inventory/                (Student 4 fills this)
├── gui/
│   ├── MainWindow.h / .cpp
│   ├── animals/
│   ├── visits/
│   └── inventory/
└── data/
    ├── owners.dat
    ├── animals.dat
    ├── visits.dat
    ├── services.dat
    ├── inventory.dat
    ├── hospitalization.dat
    └── ids.dat
```

---

## Part 1 — ISerializable

**File:** `core/ISerializable.h`

Pure interface. Every entity in the system must inherit from this.

```
class ISerializable {
public:
    virtual void serialize(std::ostream& out) const = 0;
    virtual void deserialize(std::istream& in)      = 0;
    virtual int  getId() const                      = 0;
    virtual ~ISerializable() = default;
};
```

Share this header with all teammates immediately — they need it before they can write their classes.

---

## Part 2 — Exception Classes

**File:** `core/exceptions.h`

Single header, no .cpp needed.

### Hierarchy

```
std::exception
└── VPMSException
    ├── NotFoundException
    ├── DuplicateIdException
    ├── FileIOException
    ├── ValidationException
    └── InsufficientStockException
```

### VPMSException (base)
- Constructor: `VPMSException(std::string message)`
- Stores message in `std::string _message`
- Overrides `what()` returning `_message.c_str()`

### NotFoundException
- Constructor: `NotFoundException(int id)`
- Message: `"Record with ID " + std::to_string(id) + " not found."`

### DuplicateIdException
- Constructor: `DuplicateIdException(int id)`
- Message: `"Record with ID " + std::to_string(id) + " already exists."`

### FileIOException
- Constructor: `FileIOException(std::string filePath)`
- Message: `"File I/O error: " + filePath`

### ValidationException
- Constructor: `ValidationException(std::string reason)`
- Message: passes reason directly

### InsufficientStockException
- Constructor: `InsufficientStockException(std::string itemName, int available, int requested)`
- Message: `"Insufficient stock for " + itemName + ": available=" + available + ", requested=" + requested`

Share this header with all teammates — they need to catch these in their unit tests.

---

## Part 3 — IdGenerator

**Files:** `core/IdGenerator.h`, `core/IdGenerator.cpp`

### Purpose
Generates unique, auto-incrementing integer IDs per entity type. Persists counters to file so IDs survive restarts.

### Internal state
```
std::string _filePath;                    // "data/ids.dat"
std::map<std::string, int> _counters;     // e.g. {"owner":42, "animal":17}
```

### File format (ids.dat)
```
owner 42
animal 17
visit 88
service 5
inventory 12
hospitalization 3
```

### Methods

| Method | Parameters | Returns | Logic |
|---|---|---|---|
| `IdGenerator` | `std::string filePath` | — | Load file into `_counters`. If file missing, start all counters at 0. |
| `next` | `std::string entityName` | `int` | Increment `_counters[entityName]`, save file, return new value. |
| `reset` | `std::string entityName` | `void` | Set counter to 0, save. (For testing only.) |

### load() logic
1. Open file with `std::ifstream`.
2. Read pairs: `entityName` (string) then `value` (int).
3. Store in `_counters`.

### save() logic
1. Open file with `std::ofstream` (truncate).
2. Write each pair on its own line: `name + " " + value`.

---

## Part 4 — Repository<T>

**File:** `core/Repository.h` (full template — everything in the header)

### Template constraint
`T` must inherit `ISerializable`. Not enforceable at compile time without C++20 concepts, but document it clearly.

### Internal state
```
std::vector<T>  _items;
std::string     _filePath;
```

### Constructor
- Takes `std::string filePath`
- Calls `load()`

### Public Methods

**add(const T& item) → void**
1. If `exists(item.getId())` → throw `DuplicateIdException(item.getId())`
2. `_items.push_back(item)`
3. `save()`

**getById(int id) → T&**
1. Loop `_items`, find where `item.getId() == id`
2. If not found → throw `NotFoundException(id)`
3. Return reference to found item

**getAll() → const std::vector<T>&**
- Return `_items` directly

**update(const T& item) → void**
1. Loop `_items`, find index where `getId() == item.getId()`
2. If not found → throw `NotFoundException(item.getId())`
3. Replace: `_items[index] = item`
4. `save()`

**remove(int id) → void**
1. Find iterator where `getId() == id`
2. If not found → throw `NotFoundException(id)`
3. `_items.erase(iterator)`
4. `save()`

**exists(int id) → bool**
- Return true if any item in `_items` has that ID. No exception.

**count() → int**
- Return `(int)_items.size()`

### Private Methods

**load() → void**
1. Open `_filePath` with `std::ifstream`
2. If file doesn't exist → return silently (first run)
3. If open fails for other reason → throw `FileIOException(_filePath)`
4. Loop:
   - Construct default `T` object
   - Call `item.deserialize(in)`
   - If stream still good → `_items.push_back(item)`
5. Continue until `in.eof()`

**save() → void**
1. Open `_filePath` with `std::ofstream` (truncate)
2. If open fails → throw `FileIOException(_filePath)`
3. For each item → call `item.serialize(out)`

### Serialization format convention
Tell all teammates to follow this record format:

```
<field1>\n
<field2>\n
...
---\n       ← sentinel line marking end of one record
```

`deserialize` reads fields line by line and stops when it reads `---`.
`serialize` writes fields then writes `---\n` at the end.

---

## Part 5 — ClinicService

**Files:** `core/ClinicService.h`, `core/ClinicService.cpp`

Single instance, created in `main.cpp`, passed to `MainWindow`.

### Internal state

```cpp
Repository<Owner>                 _owners       {"data/owners.dat"};
Repository<Visit>                 _visits       {"data/visits.dat"};
Repository<InventoryItem>         _inventory    {"data/inventory.dat"};
Repository<HospitalizationRecord> _hospital     {"data/hospitalization.dat"};
AnimalRepository                  _animals      {"data/animals.dat"};   // polymorphic
ServiceRepository                 _services     {"data/services.dat"};  // polymorphic
IdGenerator                       _idGen        {"data/ids.dat"};
```

> `AnimalRepository` and `ServiceRepository` are thin subclasses of Repository that override `load()` to use the respective factories. Student 2 and 3 must deliver these alongside their factory classes.

### Owner Methods

**addOwner(std::string name, std::string contact) → Owner**
1. Validate name not empty → throw `ValidationException` if empty
2. `int id = _idGen.next("owner")`
3. Construct `Owner(id, name, contact)`
4. `_owners.add(owner)`
5. Return owner

**getOwner(int ownerId) → Owner&**
- Return `_owners.getById(ownerId)` — propagates `NotFoundException`

**getAllOwners() → const std::vector<Owner>&**
- Return `_owners.getAll()`

**updateOwner(const Owner& owner) → void**
1. Validate name not empty
2. `_owners.update(owner)`

**removeOwner(int ownerId) → void**
1. Get all animals, check if any has `ownerId` — throw `ValidationException("Owner has registered animals")` if true
2. `_owners.remove(ownerId)`

### Animal Methods

**addAnimal(std::string type, std::string name, int age, int ownerId) → Animal***
1. If not `_owners.exists(ownerId)` → throw `NotFoundException(ownerId)`
2. Validate age > 0
3. `int id = _idGen.next("animal")`
4. `Animal* a = AnimalFactory::create(type, id, name, age, ownerId)`
5. `_animals.add(a)`
6. Return a

**getAnimal(int animalId) → Animal***
- Return `_animals.getById(animalId)`

**getAnimalsByOwner(int ownerId) → std::vector<Animal*>**
- Filter `_animals.getAll()` where `a->getOwnerId() == ownerId`

**updateAnimal(Animal* animal) → void**
- `_animals.update(animal)`

**removeAnimal(int animalId) → void**
1. Check no active hospitalization for animalId → throw `ValidationException("Animal is currently hospitalized")` if found
2. `_animals.remove(animalId)`

### Visit Methods

**createVisit(int animalId, int ownerId, std::string date, std::vector<int> serviceIds) → Visit**
1. If not `_animals.exists(animalId)` → throw `NotFoundException(animalId)`
2. If not `_owners.exists(ownerId)` → throw `NotFoundException(ownerId)`
3. For each serviceId → if not `_services.exists(id)` → throw `NotFoundException(id)`
4. Validate date not empty
5. `int id = _idGen.next("visit")`
6. Construct `Visit(id, animalId, ownerId, date, serviceIds)`
7. `_visits.add(visit)`
8. Return visit

**getVisit(int visitId) → Visit&**
- `_visits.getById(visitId)`

**getVisitsByAnimal(int animalId) → std::vector<Visit>**
- Filter by animalId, sort by date using `std::sort` with `operator<`

**getVisitsByOwner(int ownerId) → std::vector<Visit>**
- Filter by ownerId

**cancelVisit(int visitId) → void**
- `_visits.remove(visitId)`

### Service Catalog Methods

**addService(std::string type, std::string name, double price) → Service***
1. Validate price > 0
2. `int id = _idGen.next("service")`
3. `Service* s = ServiceFactory::create(type, id, name, price)`
4. `_services.add(s)`
5. Return s

**getAllServices() → const std::vector<Service*>&**
- `_services.getAll()`

**removeService(int serviceId) → void**
1. Scan `_visits.getAll()` — if any visit contains serviceId → throw `ValidationException("Service is used in existing visits")`
2. `_services.remove(serviceId)`

### Inventory Methods

**addInventoryItem(std::string name, int qty, double unitPrice, std::string category) → InventoryItem**
1. Validate qty >= 0, price > 0
2. `int id = _idGen.next("inventory")`
3. Construct and add

**stockIn(int itemId, int qty) → void**
1. `InventoryItem& item = _inventory.getById(itemId)`
2. `item += qty`
3. `_inventory.update(item)`

**stockOut(int itemId, int qty) → void**
1. `InventoryItem& item = _inventory.getById(itemId)`
2. If `item.getQuantity() < qty` → throw `InsufficientStockException(item.getName(), item.getQuantity(), qty)`
3. `item -= qty`
4. `_inventory.update(item)`

**getLowStockItems(int threshold = 10) → std::vector<InventoryItem>**
- Filter where `qty <= threshold`

**getAllInventory() → const std::vector<InventoryItem>&**
- `_inventory.getAll()`

### Hospitalization Methods

**admitAnimal(int animalId, std::string ward, std::string admitDate, double dailyRate) → HospitalizationRecord**
1. If not `_animals.exists(animalId)` → throw `NotFoundException(animalId)`
2. Check active records (dischargeDate empty) for animalId → throw `ValidationException("Animal already admitted")`
3. `int id = _idGen.next("hospitalization")`
4. Construct and add record

**dischargeAnimal(int recordId, std::string dischargeDate) → double**
1. `HospitalizationRecord& r = _hospital.getById(recordId)`
2. If `r.getDischargeDate()` not empty → throw `ValidationException("Already discharged")`
3. Set discharge date
4. Calculate days between admitDate and dischargeDate
5. `bill = days * dailyRate`
6. `_hospital.update(r)`
7. Return bill

**getActiveHospitalizations() → std::vector<HospitalizationRecord>**
- Filter where `dischargeDate` is empty

**addServiceToHospitalization(int recordId, int serviceId) → void**
1. Verify both exist
2. Append serviceId to record's list
3. `_hospital.update(record)`

---

## Part 6 — Qt GUI

Build GUI after Core is complete. Use `ClinicService` exclusively — no direct repository access in GUI code.

### MainWindow
- Central window with tab widget or sidebar navigation
- One tab per domain: Animals/Owners, Visits, Inventory, Hospitalization
- Owns single `ClinicService` instance, passes reference to child widgets

### Per-module widgets (one per domain)
Each widget follows the same pattern:
- Table view (QTableWidget) showing list of records
- Add / Edit / Delete buttons
- Dialog window for Add/Edit with input fields
- All operations call `ClinicService` methods
- Wrap every call in try/catch, show `QMessageBox` on `VPMSException`

### Error handling in GUI
```cpp
try {
    clinicService.addAnimal(...);
} catch (const ValidationException& e) {
    QMessageBox::warning(this, "Validation Error", e.what());
} catch (const VPMSException& e) {
    QMessageBox::critical(this, "Error", e.what());
}
```

---

## Delivery Order

1. `ISerializable.h` + `exceptions.h` → share immediately with all teammates
2. `IdGenerator` → needed before ClinicService
3. `Repository<T>` → needed before ClinicService
4. `ClinicService` stubs (headers only) → teammates need method signatures to compile against
5. `ClinicService` full implementation
6. Qt GUI
