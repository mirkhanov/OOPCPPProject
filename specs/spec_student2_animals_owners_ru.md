# Студент 2 — Модуль Животные и Владельцы

## КРИТИЧНО — Точные сигнатуры которые ты обязан соблюдать

Скопировано напрямую из `ClinicService.cpp`. Если твои имена или сигнатуры отличаются хоть немного — проект не скомпилируется.

### AnimalRepository — должен иметь ровно эти методы:
```cpp
void                        add(Animal* item);
Animal*                     getById(int id);
const vector<Animal*>&      getAll() const;
void                        update(Animal* item);
void                        remove(int id);
bool                        exists(int id) const;
```

### AnimalFactory — должен иметь ровно этот статический метод:
```cpp
static Animal* create(const string& typeTag, int id,
                      const string& name, int age, int ownerId);
```

### Animal — должен иметь ровно эти методы:
```cpp
int    getId()      const;   // из ISerializable
int    getOwnerId() const;
string getTypeTag() const;
```

### Owner — должен иметь ровно эти методы:
```cpp
int    getId()   const;      // из ISerializable
string getName() const;
```

### Конструктор Owner должен совпадать:
```cpp
Owner(int id, const string& name, const string& contactInfo);
```

---

## Твоя роль
Ты реализуешь все классы животных и владельцев.
Твои классы хранятся и извлекаются через `Repository<T>` Студента 1 и вызываются через `ClinicService`.

---

## Зависимости (подожди пока Студент 1 поделится этими файлами)
- `core/ISerializable.h` — твои классы должны наследовать этот интерфейс
- `core/exceptions.h` — используй эти типы исключений в своих тестах
- `core/Repository.h` — ты напишешь `AnimalRepository` на его основе

Не пиши свой файловый ввод-вывод. Реализуй только `serialize` / `deserialize`.

---

## Структура папок

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

## Часть 1 — Owner

**Файлы:** `animals/Owner.h`, `animals/Owner.cpp`

### Наследует
`ISerializable`

### Поля
```
int    _id
string _name
string _contactInfo
```

### Конструктор
`Owner(int id, string name, string contactInfo)`

Также нужен конструктор по умолчанию `Owner()` (нужен шаблону Repository для десериализации).

### Методы

| Метод | Возвращает | Примечания |
|---|---|---|
| `getId()` | `int` | Требование ISerializable |
| `getName()` | `string` | |
| `getContactInfo()` | `string` | |
| `setName(string)` | `void` | Используется в updateOwner |
| `setContactInfo(string)` | `void` | |
| `serialize(ostream&)` | `void` | Записать поля в поток |
| `deserialize(istream&)` | `void` | Читать поля из потока |

### Перегрузка операторов
- `operator==(const Owner& other)` — сравнение по `_id`
- `operator<<(ostream&, const Owner&)` — вывести имя и контакт

### Формат сериализации
```
serialize записывает:
  _id\n
  _name\n
  _contactInfo\n
  ---\n

deserialize читает в том же порядке, останавливается на "---"
```

---

## Часть 2 — Animal (абстрактный базовый класс)

**Файл:** `animals/Animal.h`

### Наследует
`ISerializable`

### Поля
```
int    _id
string _name
int    _age
int    _ownerId
string _typeTag    // "DOG", "CAT", "BIRD", "REPTILE"
```

### Конструктор
`Animal(int id, string name, int age, int ownerId, string typeTag)`

Также нужен конструктор по умолчанию `Animal()`.

### Методы

| Метод | Возвращает | Виртуальный? | Примечания |
|---|---|---|---|
| `getId()` | `int` | Нет | Требование ISerializable |
| `getName()` | `string` | Нет | |
| `getAge()` | `int` | Нет | |
| `getOwnerId()` | `int` | Нет | ClinicService использует это |
| `getTypeTag()` | `string` | Нет | Используется AnimalFactory |
| `setName(string)` | `void` | Нет | |
| `setAge(int)` | `void` | Нет | |
| `getSpeciesInfo()` | `string` | **чисто виртуальный** | Описание породы/вида |
| `calculateTreatmentCost()` | `double` | **чисто виртуальный** | Базовая стоимость лечения |
| `serialize(ostream&)` | `void` | виртуальный | Базовый класс пишет свои поля — подклассы сначала вызывают его, потом дописывают своё |
| `deserialize(istream&)` | `void` | виртуальный | Базовый класс читает свои поля — подклассы сначала вызывают его, потом читают своё |

### Формат сериализации
Базовый serialize записывает:
```
_typeTag\n       ← ОБЯЗАТЕЛЬНО первым, чтобы AnimalFactory мог прочитать тип
_id\n
_name\n
_age\n
_ownerId\n
```
Подкласс дописывает свои поля после, затем записывает `---\n`.
Базовый deserialize читает typeTag, id, name, age, ownerId.
Подкласс вызывает базовый, потом читает свои поля, потом читает и игнорирует `---`.

---

## Часть 3 — Конкретные подклассы Animal

Каждый подкласс наследует `Animal` и добавляет одно или два поля.

### Dog

**Дополнительное поле:** `string _breed`

**Конструктор:** `Dog(int id, string name, int age, int ownerId, string breed)`

**Реализует:**
- `getSpeciesInfo()` → `"Dog - Breed: " + _breed`
- `calculateTreatmentCost()` → `80.0`
- `serialize()` → вызвать `Animal::serialize()`, записать `_breed\n`, записать `---\n`
- `deserialize()` → вызвать `Animal::deserialize()`, прочитать `_breed`, прочитать и выбросить `---`

### Cat

**Дополнительное поле:** `string _furType` (например "Short", "Long")

**Конструктор:** `Cat(int id, string name, int age, int ownerId, string furType)`

**Реализует:**
- `getSpeciesInfo()` → `"Cat - Fur: " + _furType`
- `calculateTreatmentCost()` → `60.0`

### Bird

**Дополнительное поле:** `string _species` (например "Parrot", "Canary")

**Конструктор:** `Bird(int id, string name, int age, int ownerId, string species)`

**Реализует:**
- `getSpeciesInfo()` → `"Bird - Species: " + _species`
- `calculateTreatmentCost()` → `40.0`

### Reptile

**Дополнительное поле:** `string _reptileType` (например "Turtle", "Lizard")

**Конструктор:** `Reptile(int id, string name, int age, int ownerId, string reptileType)`

**Реализует:**
- `getSpeciesInfo()` → `"Reptile - Type: " + _reptileType`
- `calculateTreatmentCost()` → `50.0`

---

## Часть 4 — AnimalFactory

**Файлы:** `animals/AnimalFactory.h`, `animals/AnimalFactory.cpp`

### Назначение
Создаёт правильный подкласс `Animal` по строке-тегу типа.
Нужен потому что `Repository` не может полиморфно десериализовать без знания типа.

### Статический метод (для загрузки из файла)

**createFromStream(string typeTag, istream& in) → Animal***

1. По typeTag создать нужный подкласс: `"DOG"` → `new Dog()` и т.д.
2. Вызвать `animal->deserialize(in)`
3. Вернуть указатель

Если typeTag неизвестен → бросить `ValidationException("Unknown animal type: " + typeTag)`

### Статический метод (для новых животных)

**create(string typeTag, int id, string name, int age, int ownerId) → Animal***

Используется в `ClinicService::addAnimal`. Принимает полный набор полей, создаёт без десериализации.

---

## Часть 5 — AnimalRepository

**Файлы:** `animals/AnimalRepository.h`, `animals/AnimalRepository.cpp`

### Назначение
`Repository<Animal>` не работает для полиморфных типов хранящихся по значению.
`AnimalRepository` управляет указателями `Animal*` с собственной загрузкой/сохранением.

### Наследование
Не наследовать Repository — управлять хранилищем напрямую.

### Внутреннее состояние
```
vector<Animal*> _items;
string          _filePath;
```

### Методы — тот же интерфейс что у Repository<T>

| Метод | Параметры | Возвращает |
|---|---|---|
| `add` | `Animal* item` | `void` |
| `getById` | `int id` | `Animal*` |
| `getAll` | — | `const vector<Animal*>&` |
| `update` | `Animal* item` | `void` |
| `remove` | `int id` | `void` |
| `exists` | `int id` | `bool` |

### Логика load()
1. Открыть файл через `ifstream`
2. Цикл: читать строку typeTag → `AnimalFactory::createFromStream(typeTag, in)` → добавить в `_items`
3. До EOF

### Логика save()
1. Открыть файл через `ofstream` (с очисткой)
2. Для каждого `Animal*` → вызвать `a->serialize(out)`

### Управление памятью
- `AnimalRepository` владеет всеми указателями `Animal*`
- Деструктор обязан вызвать `delete` для всех элементов в `_items`
- `remove()` обязан вызвать `delete` перед удалением из вектора

---

## Что ты демонстрируешь из ООП

| Требование | Где |
|---|---|
| Наследование | `Dog`, `Cat`, `Bird`, `Reptile` наследуют `Animal` |
| Полиморфизм | `getSpeciesInfo()` и `calculateTreatmentCost()` вызываются через `Animal*` |
| Перегрузка операторов | `Owner::operator==`, `Owner::operator<<` |
| Работа с файлами | `serialize`/`deserialize` во всех классах |

---

## Что передать Студенту 1

Когда закончишь, скажи Студенту 1:
- `AnimalRepository` готов — он подключает его в `ClinicService`
- `AnimalFactory::create(typeTag, id, name, age, ownerId)` готов для `ClinicService::addAnimal`
- Пути включения: `#include "animals/Animal.h"`, `#include "animals/Owner.h"` и т.д.

---

## Компиляция и тестирование без GUI

```cpp
Owner o(1, "Алиса", "+7-999-000-0000");
Dog d(1, "Рекс", 3, 1, "Labrador");
cout << o << endl;
cout << d.getSpeciesInfo() << endl;
cout << d.calculateTreatmentCost() << endl;

// Тест сериализации/десериализации
stringstream ss;
d.serialize(ss);
Dog d2;
d2.deserialize(ss);
assert(d2.getId() == d.getId());
```
