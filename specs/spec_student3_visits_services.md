# Student 3 — Visits & Services Module

## CRITICAL — Exact Signatures You Must Match

These are copied directly from `ClinicService.cpp`. If your names or signatures differ even slightly, the project will not compile.

### ServiceRepository — must have exactly these methods:
```cpp
void                         add(Service* item);
Service*                     getById(int id);
const std::vector<Service*>& getAll() const;
void                         update(Service* item);
void                         remove(int id);
bool                         exists(int id) const;
```

### ServiceFactory — must have exactly this static method:
```cpp
static Service* create(const std::string& typeTag, int id,
                       const std::string& name, double price,
                       const std::string& extra = "");
```

### Service — must have exactly these methods:
```cpp
int    getId()         const;    // from ISerializable
double getFinalPrice() const;    // pure virtual — implement in each subclass
```

### Visit — must have exactly these methods:
```cpp
int                     getId()        const;   // from ISerializable
int                     getAnimalId()  const;
int                     getOwnerId()   const;
std::string             getDate()      const;
const std::vector<int>& getServiceIds() const;
```

### Visit constructor must match:
```cpp
Visit(int id, int animalId, int ownerId,
      const std::string& date,
      const std::vector<int>& serviceIds,
      double totalCost);
```

## Your Role
You implement all service types and visit records.
Your classes are stored by Student 1's `Repository<T>` and called through `ClinicService`.

---

## Dependencies (wait for Student 1 to share these first)
- `core/ISerializable.h` — your classes must inherit this
- `core/exceptions.h` — use for unit testing
- `core/Repository.h` — you will write `ServiceRepository` for polymorphic services

Do not write your own file I/O. Only implement `serialize` / `deserialize`.

---

## Folder Structure

```
visits/
├── Service.h
├── Consultation.h / Consultation.cpp
├── Vaccination.h / Vaccination.cpp
├── Surgery.h / Surgery.cpp
├── Grooming.h / Grooming.cpp
├── ServiceFactory.h / ServiceFactory.cpp
├── ServiceRepository.h / ServiceRepository.cpp
├── Visit.h / Visit.cpp
```

---

## Part 1 — Service (abstract base)

**File:** `visits/Service.h`

### Inherits
`ISerializable`

### Fields
```
int         _id
std::string _name
double      _basePrice
std::string _typeTag    // "CONSULTATION", "VACCINATION", "SURGERY", "GROOMING"
```

### Constructor
`Service(int id, std::string name, double basePrice, std::string typeTag)`

Also provide default constructor `Service()`.

### Methods

| Method | Returns | Virtual? | Notes |
|---|---|---|---|
| `getId()` | `int` | No | ISerializable requirement |
| `getName()` | `std::string` | No | |
| `getBasePrice()` | `double` | No | |
| `getTypeTag()` | `std::string` | No | Used by ServiceFactory |
| `getFinalPrice()` | `double` | **pure virtual** | Price after any discounts/markups |
| `getDescription()` | `std::string` | **pure virtual** | Human-readable description for receipts |
| `applyDiscount(double pct)` | `void` | virtual | Default: reduce `_basePrice` by pct. Subclasses may override. |
| `serialize(std::ostream&)` | `void` | virtual | Write base fields — subclasses call this then add their own |
| `deserialize(std::istream&)` | `void` | virtual | Read base fields — subclasses call this then read their own |

### Serialization Format
Base class serialize writes:
```
_typeTag\n       ← MUST be first so ServiceFactory can read it
_id\n
_name\n
_basePrice\n
```
Subclass serialize adds its own fields after, then writes `---\n`.
Base class deserialize reads typeTag, id, name, basePrice.
Subclass deserialize calls base then reads its own fields, then reads and discards `---`.

---

## Part 2 — Concrete Service Subclasses

### Consultation

**Extra fields:** `int _durationMinutes`

**Constructor:** `Consultation(int id, std::string name, double price, int durationMinutes)`

**Implements:**
- `getFinalPrice()` → return `_basePrice` (no markup)
- `getDescription()` → `"Consultation (" + std::to_string(_durationMinutes) + " min)"`
- `applyDiscount(double pct)` → reduce `_basePrice *= (1.0 - pct / 100.0)`

### Vaccination

**Extra fields:** `std::string _vaccineType` (e.g. "Rabies", "Parvovirus")

**Constructor:** `Vaccination(int id, std::string name, double price, std::string vaccineType)`

**Implements:**
- `getFinalPrice()` → return `_basePrice`
- `getDescription()` → `"Vaccination: " + _vaccineType`

### Surgery

**Extra fields:** `std::string _surgeryType`, `bool _requiresAnesthesia`

**Constructor:** `Surgery(int id, std::string name, double price, std::string surgeryType, bool requiresAnesthesia)`

**Implements:**
- `getFinalPrice()` → if `_requiresAnesthesia` add 20% to `_basePrice`, else return as-is
- `getDescription()` → `"Surgery: " + _surgeryType + (_requiresAnesthesia ? " (with anesthesia)" : "")`
- Override `applyDiscount()` → surgeries get maximum 10% discount (enforce this cap)

### Grooming

**Extra fields:** `std::string _groomingLevel` (e.g. "Basic", "Full", "Premium")

**Constructor:** `Grooming(int id, std::string name, double price, std::string groomingLevel)`

**Implements:**
- `getFinalPrice()` → Premium adds 15%, Basic subtracts 10%, Full is base
- `getDescription()` → `"Grooming: " + _groomingLevel`

---

## Part 3 — ServiceFactory

**Files:** `visits/ServiceFactory.h`, `visits/ServiceFactory.cpp`

### Static Methods

**create(std::string typeTag, std::istream& in) → Service***
Used when loading from file.
1. Construct right subclass based on typeTag
2. Call `service->deserialize(in)`
3. Return pointer

If typeTag unknown → throw `ValidationException("Unknown service type: " + typeTag)`

**create(std::string typeTag, int id, std::string name, double price, ...) → Service***
Used by `ClinicService::addService`. Extra parameters depend on type:
- Consultation → `int durationMinutes`
- Vaccination → `std::string vaccineType`
- Surgery → `std::string surgeryType, bool requiresAnesthesia`
- Grooming → `std::string groomingLevel`

Since parameter sets differ, use a struct or pass extras as `std::string extras`:
- Recommended: overload `create` for each type, or use a parameter struct.

---

## Part 4 — ServiceRepository

**Files:** `visits/ServiceRepository.h`, `visits/ServiceRepository.cpp`

Same pattern as `AnimalRepository` from Student 2 — manages `Service*` pointers.

### Internal state
```
std::vector<Service*> _items;
std::string           _filePath;
```

### Methods — same interface as Repository<T>

| Method | Parameters | Returns |
|---|---|---|
| `add` | `Service* item` | `void` |
| `getById` | `int id` | `Service*` |
| `getAll` | — | `const std::vector<Service*>&` |
| `update` | `Service* item` | `void` |
| `remove` | `int id` | `void` |
| `exists` | `int id` | `bool` |

### load() logic
1. Open file
2. Loop: read typeTag line → `ServiceFactory::create(typeTag, in)` → push to `_items`

### save() logic
- For each `Service*` → call `s->serialize(out)`

### Memory management
- Destructor deletes all pointers
- `remove()` deletes before erasing

---

## Part 5 — Visit

**Files:** `visits/Visit.h`, `visits/Visit.cpp`

### Inherits
`ISerializable`

### Fields
```
int                  _id
int                  _animalId
int                  _ownerId
std::string          _date         // format: "YYYY-MM-DD"
std::vector<int>     _serviceIds   // store IDs, not pointers
double               _totalCost    // computed and stored
std::string          _notes        // optional vet notes
```

### Constructor
`Visit(int id, int animalId, int ownerId, std::string date, std::vector<int> serviceIds, double totalCost)`

Default constructor `Visit()` also required.

### Methods

| Method | Returns | Notes |
|---|---|---|
| `getId()` | `int` | ISerializable requirement |
| `getAnimalId()` | `int` | ClinicService uses this |
| `getOwnerId()` | `int` | ClinicService uses this |
| `getDate()` | `std::string` | |
| `getServiceIds()` | `const std::vector<int>&` | |
| `getTotalCost()` | `double` | |
| `getNotes()` | `std::string` | |
| `setNotes(std::string)` | `void` | |
| `serialize(std::ostream&)` | `void` | |
| `deserialize(std::istream&)` | `void` | |

### Operator Overloads
- `operator<(const Visit& other)` — compare by `_date` string (lexicographic works for YYYY-MM-DD)
- `operator==(const Visit& other)` — compare by `_id`
- `operator<<(std::ostream&, const Visit&)` — print summary line for display

### Serialization Format
```
serialize writes:
  _id\n
  _animalId\n
  _ownerId\n
  _date\n
  _totalCost\n
  _notes\n
  _serviceIds.size()\n
  _serviceIds[0]\n
  _serviceIds[1]\n
  ...
  ---\n

deserialize reads in same order.
```

### How totalCost is set
`ClinicService::createVisit` resolves service IDs to `Service*` objects, sums `getFinalPrice()`, and passes the total to the `Visit` constructor. `Visit` itself does not compute cost — it just stores the precomputed value.

---

## OOP Requirements You Demonstrate

| Requirement | Where |
|---|---|
| Inheritance | `Consultation`, `Vaccination`, `Surgery`, `Grooming` inherit `Service` |
| Polymorphism | `getFinalPrice()`, `getDescription()`, `applyDiscount()` called via `Service*` |
| Operator Overloading | `Visit::operator<`, `operator==`, `operator<<` |
| File Handling | `serialize`/`deserialize` on all classes |

---

## What to Hand Off to Student 1

Once done, tell Student 1:
- `ServiceRepository` is ready — plug into `ClinicService`
- `ServiceFactory::create(typeTag, id, name, price, ...)` is ready for `ClinicService::addService`
- Header paths: `#include "visits/Visit.h"`, `#include "visits/Service.h"`

---

## Compile & Test Without GUI

```cpp
// Test service polymorphism
Surgery s(1, "Neutering", 200.0, "Neutering", true);
std::cout << s.getDescription() << std::endl;
std::cout << s.getFinalPrice() << std::endl;   // should be 240.0

// Test visit operator<
Visit v1(1, 1, 1, "2024-03-01", {1,2}, 260.0);
Visit v2(2, 1, 1, "2024-01-15", {1}, 200.0);
std::cout << (v2 < v1) << std::endl;           // true — Jan before Mar

// Test serialize round-trip
std::stringstream ss;
v1.serialize(ss);
Visit v3;
v3.deserialize(ss);
assert(v3.getId() == v1.getId());
assert(v3.getServiceIds().size() == 2);
```
