# Студент 3 — Модуль Визиты и Услуги

## КРИТИЧНО — Точные сигнатуры которые ты обязан соблюдать

Скопировано напрямую из `ClinicService.cpp`. Если твои имена или сигнатуры отличаются хоть немного — проект не скомпилируется.

### ServiceRepository — должен иметь ровно эти методы:
```cpp
void                    add(Service* item);
Service*                getById(int id);
const vector<Service*>& getAll() const;
void                    update(Service* item);
void                    remove(int id);
bool                    exists(int id) const;
```

### ServiceFactory — должен иметь ровно этот статический метод:
```cpp
static Service* create(const string& typeTag, int id,
                       const string& name, double price,
                       const string& extra = "");
```

### Service — должен иметь ровно эти методы:
```cpp
int    getId()         const;    // из ISerializable
double getFinalPrice() const;    // чисто виртуальный — реализовать в каждом подклассе
```

### Visit — должен иметь ровно эти методы:
```cpp
int                  getId()         const;   // из ISerializable
int                  getAnimalId()   const;
int                  getOwnerId()    const;
string               getDate()       const;
const vector<int>&   getServiceIds() const;
```

### Конструктор Visit должен совпадать:
```cpp
Visit(int id, int animalId, int ownerId,
      const string& date,
      const vector<int>& serviceIds,
      double totalCost);
```

---

## Твоя роль
Ты реализуешь все типы услуг и записи о визитах.
Твои классы хранятся через `Repository<T>` Студента 1 и вызываются через `ClinicService`.

---

## Зависимости (подожди пока Студент 1 поделится этими файлами)
- `core/ISerializable.h` — твои классы должны наследовать этот интерфейс
- `core/exceptions.h` — используй для тестов
- `core/Repository.h` — ты напишешь `ServiceRepository` для полиморфных услуг

Не пиши свой файловый ввод-вывод. Реализуй только `serialize` / `deserialize`.

---

## Структура папок

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

## Часть 1 — Service (абстрактный базовый класс)

**Файл:** `visits/Service.h`

### Наследует
`ISerializable`

### Поля
```
int    _id
string _name
double _basePrice
string _typeTag    // "CONSULTATION", "VACCINATION", "SURGERY", "GROOMING"
```

### Конструктор
`Service(int id, string name, double basePrice, string typeTag)`

Также нужен конструктор по умолчанию `Service()`.

### Методы

| Метод | Возвращает | Виртуальный? | Примечания |
|---|---|---|---|
| `getId()` | `int` | Нет | Требование ISerializable |
| `getName()` | `string` | Нет | |
| `getBasePrice()` | `double` | Нет | |
| `getTypeTag()` | `string` | Нет | Используется ServiceFactory |
| `getFinalPrice()` | `double` | **чисто виртуальный** | Цена после скидок/наценок |
| `getDescription()` | `string` | **чисто виртуальный** | Текстовое описание для чека |
| `applyDiscount(double pct)` | `void` | виртуальный | По умолчанию: уменьшить `_basePrice` на pct% |
| `serialize(ostream&)` | `void` | виртуальный | Базовый пишет свои поля — подклассы вызывают его сначала |
| `deserialize(istream&)` | `void` | виртуальный | Базовый читает свои поля — подклассы вызывают его сначала |

### Формат сериализации
Базовый serialize записывает:
```
_typeTag\n       ← ОБЯЗАТЕЛЬНО первым, чтобы ServiceFactory мог прочитать тип
_id\n
_name\n
_basePrice\n
```
Подкласс дописывает свои поля после, затем записывает `---\n`.

---

## Часть 2 — Конкретные подклассы Service

### Consultation (Консультация)

**Дополнительное поле:** `int _durationMinutes`

**Конструктор:** `Consultation(int id, string name, double price, int durationMinutes)`

**Реализует:**
- `getFinalPrice()` → вернуть `_basePrice` (без наценки)
- `getDescription()` → `"Consultation (" + to_string(_durationMinutes) + " min)"`
- `applyDiscount(double pct)` → `_basePrice *= (1.0 - pct / 100.0)`

### Vaccination (Вакцинация)

**Дополнительное поле:** `string _vaccineType` (например "Rabies", "Parvovirus")

**Конструктор:** `Vaccination(int id, string name, double price, string vaccineType)`

**Реализует:**
- `getFinalPrice()` → вернуть `_basePrice`
- `getDescription()` → `"Vaccination: " + _vaccineType`

### Surgery (Операция)

**Дополнительные поля:** `string _surgeryType`, `bool _requiresAnesthesia`

**Конструктор:** `Surgery(int id, string name, double price, string surgeryType, bool requiresAnesthesia)`

**Реализует:**
- `getFinalPrice()` → если `_requiresAnesthesia` добавить 20% к `_basePrice`, иначе вернуть как есть
- `getDescription()` → `"Surgery: " + _surgeryType + (_requiresAnesthesia ? " (with anesthesia)" : "")`
- Переопределить `applyDiscount()` → операции получают максимум 10% скидку (ограничить сверху)

### Grooming (Груминг)

**Дополнительное поле:** `string _groomingLevel` (например "Basic", "Full", "Premium")

**Конструктор:** `Grooming(int id, string name, double price, string groomingLevel)`

**Реализует:**
- `getFinalPrice()` → Premium добавляет 15%, Basic вычитает 10%, Full — базовая цена
- `getDescription()` → `"Grooming: " + _groomingLevel`

---

## Часть 3 — ServiceFactory

**Файлы:** `visits/ServiceFactory.h`, `visits/ServiceFactory.cpp`

### Статические методы

**createFromStream(string typeTag, istream& in) → Service***
Используется при загрузке из файла.
1. Создать нужный подкласс по typeTag
2. Вызвать `service->deserialize(in)`
3. Вернуть указатель

Если typeTag неизвестен → бросить `ValidationException("Unknown service type: " + typeTag)`

**create(string typeTag, int id, string name, double price, string extra) → Service***
Используется в `ClinicService::addService`.
- `"CONSULTATION"` → создать Consultation
- `"VACCINATION"` → extra это vaccineType
- `"SURGERY"` → extra это surgeryType
- `"GROOMING"` → extra это groomingLevel

---

## Часть 4 — ServiceRepository

**Файлы:** `visits/ServiceRepository.h`, `visits/ServiceRepository.cpp`

Тот же шаблон что `AnimalRepository` у Студента 2 — управляет указателями `Service*`.

### Внутреннее состояние
```
vector<Service*> _items;
string           _filePath;
```

### Методы — тот же интерфейс что у Repository<T>

| Метод | Параметры | Возвращает |
|---|---|---|
| `add` | `Service* item` | `void` |
| `getById` | `int id` | `Service*` |
| `getAll` | — | `const vector<Service*>&` |
| `update` | `Service* item` | `void` |
| `remove` | `int id` | `void` |
| `exists` | `int id` | `bool` |

### Управление памятью
- Деструктор удаляет все указатели
- `remove()` вызывает delete перед удалением из вектора

---

## Часть 5 — Visit

**Файлы:** `visits/Visit.h`, `visits/Visit.cpp`

### Наследует
`ISerializable`

### Поля
```
int          _id
int          _animalId
int          _ownerId
string       _date         // формат: "YYYY-MM-DD"
vector<int>  _serviceIds   // хранить ID, не указатели
double       _totalCost    // вычисляется и хранится
string       _notes        // заметки ветеринара
```

### Конструктор
`Visit(int id, int animalId, int ownerId, string date, vector<int> serviceIds, double totalCost)`

Конструктор по умолчанию `Visit()` также обязателен.

### Методы

| Метод | Возвращает | Примечания |
|---|---|---|
| `getId()` | `int` | Требование ISerializable |
| `getAnimalId()` | `int` | ClinicService использует это |
| `getOwnerId()` | `int` | ClinicService использует это |
| `getDate()` | `string` | |
| `getServiceIds()` | `const vector<int>&` | |
| `getTotalCost()` | `double` | |
| `getNotes()` | `string` | |
| `setNotes(string)` | `void` | |
| `serialize(ostream&)` | `void` | |
| `deserialize(istream&)` | `void` | |

### Перегрузка операторов
- `operator<(const Visit& other)` — сравнение по `_date` (лексикографически, работает для YYYY-MM-DD)
- `operator==(const Visit& other)` — сравнение по `_id`
- `operator<<(ostream&, const Visit&)` — вывести краткую информацию

### Формат сериализации
```
serialize записывает:
  _id\n
  _animalId\n
  _ownerId\n
  _date\n
  _totalCost\n
  _notes\n
  количество serviceIds\n
  serviceIds[0]\n
  serviceIds[1]\n
  ...
  ---\n
```

### Как устанавливается totalCost
`ClinicService::createVisit` разрешает ID услуг в указатели `Service*`, суммирует `getFinalPrice()` и передаёт итог в конструктор `Visit`. Сам `Visit` не вычисляет стоимость — только хранит заранее вычисленное значение.

---

## Что ты демонстрируешь из ООП

| Требование | Где |
|---|---|
| Наследование | `Consultation`, `Vaccination`, `Surgery`, `Grooming` наследуют `Service` |
| Полиморфизм | `getFinalPrice()`, `getDescription()`, `applyDiscount()` вызываются через `Service*` |
| Перегрузка операторов | `Visit::operator<`, `operator==`, `operator<<` |
| Работа с файлами | `serialize`/`deserialize` во всех классах |

---

## Что передать Студенту 1

Когда закончишь, скажи Студенту 1:
- `ServiceRepository` готов — он подключает его в `ClinicService`
- `ServiceFactory::create(typeTag, id, name, price, extra)` готов для `ClinicService::addService`
- Пути включения: `#include "visits/Visit.h"`, `#include "visits/Service.h"`

---

## Компиляция и тестирование без GUI

```cpp
// Тест полиморфизма услуг
Surgery s(1, "Нейтрализация", 200.0, "Neutering", true);
cout << s.getDescription() << endl;
cout << s.getFinalPrice() << endl;   // должно быть 240.0

// Тест operator< у Visit
Visit v1(1, 1, 1, "2024-03-01", {1,2}, 260.0);
Visit v2(2, 1, 1, "2024-01-15", {1}, 200.0);
cout << (v2 < v1) << endl;           // true — январь раньше марта

// Тест сериализации
stringstream ss;
v1.serialize(ss);
Visit v3;
v3.deserialize(ss);
assert(v3.getId() == v1.getId());
assert(v3.getServiceIds().size() == 2);
```
