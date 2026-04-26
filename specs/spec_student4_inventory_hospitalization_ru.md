# Студент 4 — Модуль Инвентарь и Госпитализация

## КРИТИЧНО — Точные сигнатуры которые ты обязан соблюдать

Скопировано напрямую из `ClinicService.cpp`. Если твои имена или сигнатуры отличаются хоть немного — проект не скомпилируется.

### InventoryItem — должен иметь ровно эти методы и операторы:
```cpp
int    getId()       const;   // из ISerializable
string getName()     const;
int    getQuantity() const;

InventoryItem& operator+=(int qty);
InventoryItem& operator-=(int qty);
```

### Конструктор InventoryItem должен совпадать:
```cpp
InventoryItem(int id, const string& name, int quantity,
              double unitPrice, const string& category);
```

### HospitalizationRecord — должен иметь ровно эти методы:
```cpp
int                getId()            const;   // из ISerializable
int                getAnimalId()      const;
string             getAdmitDate()     const;
string             getDischargeDate() const;
double             getDailyRate()     const;
const vector<int>& getServiceIds()    const;
bool               isActive()         const;   // возвращает dischargeDate.empty()

void setDischargeDate(const string& date);
void setTotalBill(double bill);
void addServiceId(int serviceId);
```

### Конструктор HospitalizationRecord должен совпадать:
```cpp
HospitalizationRecord(int id, int animalId, const string& ward,
                      const string& admitDate, double dailyRate);
```

---

## Твоя роль
Ты реализуешь управление запасами и записи о госпитализации животных.
Твои классы хранятся через `Repository<T>` Студента 1 и вызываются через `ClinicService`.

---

## Зависимости (подожди пока Студент 1 поделится этими файлами)
- `core/ISerializable.h` — твои классы должны наследовать этот интерфейс
- `core/exceptions.h` — особенно `InsufficientStockException`
- `core/Repository.h` — используй напрямую (полиморфизм здесь не нужен, фабрика не нужна)

Не пиши свой файловый ввод-вывод. Реализуй только `serialize` / `deserialize`.

---

## Структура папок

```
inventory/
├── InventoryItem.h / InventoryItem.cpp
├── HospitalizationRecord.h / HospitalizationRecord.cpp
```

Всё — никакой фабрики, никакого абстрактного базового класса, никаких подклассов.

---

## Часть 1 — InventoryItem

**Файлы:** `inventory/InventoryItem.h`, `inventory/InventoryItem.cpp`

### Наследует
`ISerializable`

### Поля
```
int    _id
string _name
int    _quantity
double _unitPrice
string _category    // например "Medicine", "Equipment", "Consumable"
```

### Конструкторы
`InventoryItem(int id, string name, int quantity, double unitPrice, string category)`

Конструктор по умолчанию `InventoryItem()` также обязателен (нужен для десериализации в Repository).

### Методы

| Метод | Возвращает | Примечания |
|---|---|---|
| `getId()` | `int` | Требование ISerializable |
| `getName()` | `string` | |
| `getQuantity()` | `int` | Используется в проверке stockOut |
| `getUnitPrice()` | `double` | |
| `getCategory()` | `string` | |
| `setName(string)` | `void` | |
| `setUnitPrice(double)` | `void` | |
| `setCategory(string)` | `void` | |
| `serialize(ostream&)` | `void` | |
| `deserialize(istream&)` | `void` | |

### Перегрузка операторов (обязательно для оценки по ООП)

**operator+=(int qty)**
- `_quantity += qty`
- Вернуть `InventoryItem&`
- Используется в `ClinicService::stockIn`

**operator-=(int qty)**
- `_quantity -= qty`
- Вернуть `InventoryItem&`
- Используется в `ClinicService::stockOut` (ClinicService проверяет перед вызовом, здесь проверка не нужна)

**operator<(const InventoryItem& other)**
- Сравнение по `_name` алфавитно
- Используется при сортировке списка инвентаря

**operator==(const InventoryItem& other)**
- Сравнение по `_id`

**operator<<(ostream&, const InventoryItem&)**
- Вывести: `[id] name | qty: X | price: Y | category: Z`

### Формат сериализации
```
serialize записывает:
  _id\n
  _name\n
  _quantity\n
  _unitPrice\n
  _category\n
  ---\n

deserialize читает в том же порядке, читает и выбрасывает "---"
```

---

## Часть 2 — HospitalizationRecord

**Файлы:** `inventory/HospitalizationRecord.h`, `inventory/HospitalizationRecord.cpp`

### Наследует
`ISerializable`

### Поля
```
int         _id
int         _animalId
string      _ward           // например "Ward A", "ICU"
string      _admitDate      // "YYYY-MM-DD"
string      _dischargeDate  // "" означает всё ещё активна
double      _dailyRate
vector<int> _serviceIds     // дополнительные услуги во время пребывания
double      _totalBill      // устанавливается при выписке, 0.0 пока активна
```

### Конструкторы
`HospitalizationRecord(int id, int animalId, string ward, string admitDate, double dailyRate)`

Конструктор по умолчанию `HospitalizationRecord()` также обязателен.

### Методы

| Метод | Возвращает | Примечания |
|---|---|---|
| `getId()` | `int` | Требование ISerializable |
| `getAnimalId()` | `int` | ClinicService использует для поиска |
| `getWard()` | `string` | |
| `getAdmitDate()` | `string` | |
| `getDischargeDate()` | `string` | Пустая строка = всё ещё активна |
| `getDailyRate()` | `double` | |
| `getServiceIds()` | `const vector<int>&` | |
| `getTotalBill()` | `double` | |
| `isActive()` | `bool` | Возвращает `_dischargeDate.empty()` |
| `setDischargeDate(string)` | `void` | Вызывается в ClinicService::dischargeAnimal |
| `setTotalBill(double)` | `void` | Вызывается в ClinicService::dischargeAnimal |
| `addServiceId(int)` | `void` | Добавить в `_serviceIds` |
| `serialize(ostream&)` | `void` | |
| `deserialize(istream&)` | `void` | |

### Перегрузка операторов (обязательно для оценки по ООП)

**operator<(const HospitalizationRecord& other)**
- Сравнение по `_admitDate` (лексикографически, работает для YYYY-MM-DD)
- Позволяет сортировать записи хронологически

**operator==(const HospitalizationRecord& other)**
- Сравнение по `_id`

**operator<<(ostream&, const HospitalizationRecord&)**
- Вывести: `[id] animalId=X | ward=Y | admitted=Z | discharged=W | bill=B`

### Формат сериализации
```
serialize записывает:
  _id\n
  _animalId\n
  _ward\n
  _admitDate\n
  _dischargeDate\n         ← пустая строка если не выписан
  _dailyRate\n
  _totalBill\n
  количество serviceIds\n
  serviceIds[0]\n
  serviceIds[1]\n
  ...
  ---\n
```

Для `_dischargeDate`: использовать `getline(in, _dischargeDate)` — пустая строка означает не выписан.

---

## Что ты демонстрируешь из ООП

| Требование | Где |
|---|---|
| Перегрузка операторов | `InventoryItem`: `+=`, `-=`, `<`, `==`, `<<` |
| Перегрузка операторов | `HospitalizationRecord`: `<`, `==`, `<<` |
| Работа с файлами | `serialize`/`deserialize` в обоих классах |
| Обработка исключений | `InsufficientStockException` бросается в `ClinicService::stockOut` — убедись что `getQuantity()` возвращает правильное значение |

Примечание: Наследование и полиморфизм демонстрируют другие модули. Твой вклад в ООП — это перегрузка операторов и работа с файлами.

---

## Что передать Студенту 1

Когда закончишь, скажи Студенту 1:
- `InventoryItem` и `HospitalizationRecord` готовы — `Repository<InventoryItem>` и `Repository<HospitalizationRecord>` работают из коробки (специальный репозиторий или фабрика не нужны)
- Пути включения: `#include "inventory/InventoryItem.h"`, `#include "inventory/HospitalizationRecord.h"`

---

## Компиляция и тестирование без GUI

```cpp
// Тест перегрузки операторов
InventoryItem item(1, "Амоксициллин", 50, 12.99, "Medicine");
item += 20;
assert(item.getQuantity() == 70);
item -= 10;
assert(item.getQuantity() == 60);
cout << item << endl;

// Тест сортировки
InventoryItem a(2, "Бинты", 100, 2.50, "Consumable");
InventoryItem b(3, "Амоксициллин", 60, 12.99, "Medicine");
cout << (b < a) << endl;   // true — "А" раньше "Б"

// Тест сериализации
stringstream ss;
item.serialize(ss);
InventoryItem item2;
item2.deserialize(ss);
assert(item2.getId() == item.getId());
assert(item2.getQuantity() == 60);

// Тест HospitalizationRecord
HospitalizationRecord rec(1, 5, "Ward A", "2024-03-01", 150.0);
assert(rec.isActive() == true);
rec.setDischargeDate("2024-03-05");
assert(rec.isActive() == false);

// Тест сериализации
stringstream ss2;
rec.addServiceId(3);
rec.addServiceId(7);
rec.serialize(ss2);
HospitalizationRecord rec2;
rec2.deserialize(ss2);
assert(rec2.getServiceIds().size() == 2);
```
