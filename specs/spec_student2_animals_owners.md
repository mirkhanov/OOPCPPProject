# Student 2 — Animals & Owners Module

## CRITICAL — Exact Signatures You Must Match

These are copied directly from `ClinicService.cpp`. If your names or signatures differ even slightly, the project will not compile.

### AnimalRepository — must have exactly these methods:
```cpp
void                        add(Animal* item);
Animal*                     getById(int id);
const std::vector<Animal*>& getAll() const;
void                        update(Animal* item);
void                        remove(int id);
bool                        exists(int id) const;
```

### AnimalFactory — must have exactly this static method:
```cpp
static Animal* create(const std::string& typeTag, int id,
                      const std::string& name, int age, int ownerId);
```

### Animal — must have exactly these methods:
```cpp
int         getId()      const;   // from ISerializable
int         getOwnerId() const;
std::string getTypeTag() const;
```

### Owner — must have exactly these methods:
```cpp
int         getId()   const;      // from ISerializable
std::string getName() const;
```

### Owner constructor must match:
```cpp
Owner(int id, const std::string& name, const std::string& contactInfo);
```

## Your Role
You implement all animal and owner domain classes.
Your classes are stored and retrieved by Student 1's `Repository<T>` and called through `ClinicService`.

---

## Dependencies (wait for Student 1 to share these first)
- `core/ISerializable.h` — your classes must inherit this
- `core/exceptions.h` — use these exception types in your unit tests
- `core/Repository.h` — you will write `AnimalRepository` that extends it

Do not write your own file I/O. Only implement `serialize` / `deserialize`.

---

## Folder Structure

```
animals/
├── Animal.h
├── Dog.h / Dog.cpp
├── Cat.h / Cat.cpp
├── Bird.h / Bird.cpp
├── Reptile.h / Reptile.cpp
├── AnimalFactory.h / AnimalFactory.cpp
├── AnimalRepository.h / AnimalRepository.cpp
├── Owner.h / Owner.cpp
```

---

## Part 1 — Owner

**Files:** `animals/Owner.h`, `animals/Owner.cpp`

### Inherits
`ISerializable`

### Fields
```
int         _id
std::string _name
std::string _contactInfo
```

### Constructor
`Owner(int id, std::string name, std::string contactInfo)`

Also provide a default constructor `Owner()` (needed by Repository template for default construction during deserialization).

### Methods

| Method | Returns | Notes |
|---|---|---|
| `getId()` | `int` | Required by ISerializable |
| `getName()` | `std::string` | |
| `getContactInfo()` | `std::string` | |
| `setName(std::string)` | `void` | Used by updateOwner |
| `setContactInfo(std::string)` | `void` | |
| `serialize(std::ostream&)` | `void` | Write fields to stream |
| `deserialize(std::istream&)` | `void` | Read fields from stream |

### Operator Overloads
- `operator==(const Owner& other)` — compare by `_id`
- `operator<<(std::ostream&, const Owner&)` — print name and contact (for debug/display)

### Serialization Format
```
serialize writes:
  _id\n
  _name\n
  _contactInfo\n
  ---\n

deserialize reads in the same order, stops at "---"
```

---

## Part 2 — Animal (abstract base)

**File:** `animals/Animal.h`

### Inherits
`ISerializable`

### Fields
```
int         _id
std::string _name
int         _age
int         _ownerId
std::string _typeTag    // "DOG", "CAT", "BIRD", "REPTILE"
```

### Constructor
`Animal(int id, std::string name, int age, int ownerId, std::string typeTag)`

Also provide default constructor `Animal()`.

### Methods

| Method | Returns | Virtual? | Notes |
|---|---|---|---|
| `getId()` | `int` | No | ISerializable requirement |
| `getName()` | `std::string` | No | |
| `getAge()` | `int` | No | |
| `getOwnerId()` | `int` | No | ClinicService uses this |
| `getTypeTag()` | `std::string` | No | Used by AnimalFactory during deserialization |
| `setName(std::string)` | `void` | No | |
| `setAge(int)` | `void` | No | |
| `getSpeciesInfo()` | `std::string` | **pure virtual** | Returns breed, species description etc. |
| `calculateTreatmentCost()` | `double` | **pure virtual** | Base cost for treating this animal |
| `serialize(std::ostream&)` | `void` | virtual | Write base fields — subclasses call this then add their own |
| `deserialize(std::istream&)` | `void` | virtual | Read base fields — subclasses call this then read their own |

### Serialization Format
Base class serialize writes:
```
_typeTag\n       ← MUST be first so AnimalFactory can read it
_id\n
_name\n
_age\n
_ownerId\n
```
Subclass serialize adds its own fields after, then writes `---\n`.

Base class deserialize reads typeTag, id, name, age, ownerId.
Subclass deserialize calls base then reads its own fields, then reads and discards `---`.

---

## Part 3 — Concrete Animal Subclasses

Each subclass inherits `Animal` and adds one or two species-specific fields.

### Dog

**Extra fields:** `std::string _breed`

**Constructor:** `Dog(int id, std::string name, int age, int ownerId, std::string breed)`

**Implements:**
- `getSpeciesInfo()` → returns `"Dog - Breed: " + _breed`
- `calculateTreatmentCost()` → returns `80.0` (base, can be adjusted)
- `serialize()` → call `Animal::serialize()`, then write `_breed\n`, then `---\n`
- `deserialize()` → call `Animal::deserialize()`, then read `_breed`, then read and discard `---`

### Cat

**Extra fields:** `std::string _furType` (e.g. "Short", "Long")

**Constructor:** `Cat(int id, std::string name, int age, int ownerId, std::string furType)`

**Implements:**
- `getSpeciesInfo()` → `"Cat - Fur: " + _furType`
- `calculateTreatmentCost()` → `60.0`

### Bird

**Extra fields:** `std::string _species` (e.g. "Parrot", "Canary")

**Constructor:** `Bird(int id, std::string name, int age, int ownerId, std::string species)`

**Implements:**
- `getSpeciesInfo()` → `"Bird - Species: " + _species`
- `calculateTreatmentCost()` → `40.0`

### Reptile

**Extra fields:** `std::string _reptileType` (e.g. "Turtle", "Lizard")

**Constructor:** `Reptile(int id, std::string name, int age, int ownerId, std::string reptileType)`

**Implements:**
- `getSpeciesInfo()` → `"Reptile - Type: " + _reptileType`
- `calculateTreatmentCost()` → `50.0`

---

## Part 4 — AnimalFactory

**Files:** `animals/AnimalFactory.h`, `animals/AnimalFactory.cpp`

### Purpose
Creates the correct `Animal` subclass from a type tag string.
Required because `Repository` can't polymorphically deserialize without knowing the type.

### Static Method

**create(std::string typeTag, std::istream& in) → Animal***

1. Read typeTag (already read by caller, passed in as parameter) — OR read it from stream directly
2. Construct the right subclass based on typeTag:
   - `"DOG"` → `new Dog()`
   - `"CAT"` → `new Cat()`
   - `"BIRD"` → `new Bird()`
   - `"REPTILE"` → `new Reptile()`
3. Call `animal->deserialize(in)` on the new object
4. Return pointer

If typeTag is unknown → throw `ValidationException("Unknown animal type: " + typeTag)`

### Alternative create (for new animals, not loading from file)

**create(std::string typeTag, int id, std::string name, int age, int ownerId, ...) → Animal***

Used by `ClinicService::addAnimal`. Takes full field list, constructs without deserializing.

---

## Part 5 — AnimalRepository

**Files:** `animals/AnimalRepository.h`, `animals/AnimalRepository.cpp`

### Purpose
`Repository<Animal>` won't work for polymorphic types stored by value.
`AnimalRepository` manages `Animal*` pointers with custom load/save.

### Inherits
Do not inherit Repository — manage storage directly.

### Internal state
```
std::vector<Animal*> _items;
std::string          _filePath;
```

### Methods — same interface as Repository<T>

| Method | Parameters | Returns |
|---|---|---|
| `add` | `Animal* item` | `void` |
| `getById` | `int id` | `Animal*` |
| `getAll` | — | `const std::vector<Animal*>&` |
| `update` | `Animal* item` | `void` |
| `remove` | `int id` | `void` |
| `exists` | `int id` | `bool` |

### load() logic
1. Open file with `std::ifstream`
2. Loop:
   - Read first line → this is the typeTag
   - `Animal* a = AnimalFactory::create(typeTag, in)`
   - Push to `_items`
3. Until EOF

### save() logic
1. Open file with `std::ofstream` (truncate)
2. For each `Animal*` → call `a->serialize(out)`

### Memory management
- `AnimalRepository` owns all `Animal*` pointers
- Destructor must `delete` all items in `_items`
- `remove()` must `delete` the pointer before erasing

---

## OOP Requirements You Demonstrate

| Requirement | Where |
|---|---|
| Inheritance | `Dog`, `Cat`, `Bird`, `Reptile` all inherit `Animal` |
| Polymorphism | `getSpeciesInfo()` and `calculateTreatmentCost()` called via `Animal*` |
| Operator Overloading | `Owner::operator==`, `Owner::operator<<` |
| File Handling | `serialize`/`deserialize` on all classes |

---

## What to Hand Off to Student 1

Once done, tell Student 1:
- `AnimalRepository` is ready — they plug it into `ClinicService` in place of `Repository<Animal>`
- `AnimalFactory::create(typeTag, id, name, age, ownerId, ...)` is ready for `ClinicService::addAnimal`
- Header include path: `#include "animals/Animal.h"`, `#include "animals/Owner.h"` etc.

---

## Compile & Test Without GUI

Write a simple `main()` in a test file:
```
Owner o(1, "Alice", "+1-555-0100");
Dog d(1, "Rex", 3, 1, "Labrador");
std::cout << o << std::endl;
std::cout << d.getSpeciesInfo() << std::endl;
std::cout << d.calculateTreatmentCost() << std::endl;

// Test serialize/deserialize round-trip
std::stringstream ss;
d.serialize(ss);
Dog d2;
d2.deserialize(ss);
assert(d2.getId() == d.getId());
```
