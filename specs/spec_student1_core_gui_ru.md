# Студент 1 — Ядро системы (Core) + GUI

## Твоя роль
Ты — ��ундамент всего проекта. Все остальные студенты зависят от твоего кода.
Сначала напиши Core, сразу поделись заголовочными файлами с командой, затем строй GUI пока другие заканчивают свои модули.

---

## Структура папок

```
vpms/
├── vpms.pro
├── main.cpp
├── core/
│   ├── ISerializable.h
│   ├── Repository.h          (шаблон — полная реализация в .h)
│   ├── IdGenerator.h / .cpp
│   ├── ClinicService.h / .cpp
│   └── exceptions.h
├── animals/                  (заполняет Студент 2)
├── visits/                   (заполняет Студент 3)
├── inventory/                (заполняет Студент 4)
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

## Часть 1 — ISerializable

**Файл:** `core/ISerializable.h`

Чистый интерфейс. Каждый класс-сущность в системе обязан наследоваться от него.

```cpp
class ISerializable {
public:
    virtual void serialize(ostream& out) const = 0;
    virtual void deserialize(istream& in)      = 0;
    virtual int  getId() const                 = 0;
    virtual ~ISerializable() = default;
};
```

Поделись этим заголовком со всей командой сразу — он нужен им до того, как они начнут писать свои классы.

---

## Часть 2 — Классы исключений

**Файл:** `core/exceptions.h`

Один заголовочный файл, .cpp не нужен.

### Иерархия

```
std::exception
└── VPMSException
    ├── NotFoundException
    ├── DuplicateIdException
    ├── FileIOException
    ├── ValidationException
    └── InsufficientStockException
```

### VPMSException (базовый)
- Конструктор: `VPMSException(string message)`
- Хранит сообщение в `string _message`
- Переопределяет `what()`, возвращая `_message.c_str()`

### NotFoundException
- Конструктор: `NotFoundException(int id)`
- Сообщение: `"Record with ID " + to_string(id) + " not found."`

### DuplicateIdException
- Конструктор: `DuplicateIdException(int id)`
- Сообщение: `"Record with ID " + to_string(id) + " already exists."`

### FileIOException
- Конструктор: `FileIOException(string filePath)`
- Сообщение: `"File I/O error: " + filePath`

### ValidationException
- Конструктор: `ValidationException(string reason)`
- Сообщение: передаёт reason напрямую

### InsufficientStockException
- Конструктор: `InsufficientStockException(string itemName, int available, int requested)`
- Сообщение: `"Insufficient stock for " + itemName + ": available=" + available + ", requested=" + requested`

Поделись этим заголовком со всей командой — он нужен им для тестов.

---

## Часть 3 — IdGenerator

**Файлы:** `core/IdGenerator.h`, `core/IdGenerator.cpp`

### Назначение
Генерирует уникальные автоинкрементные целочисленные ID для каждого типа сущности. Сохраняет счётчики в файл, чтобы ID не повторялись после перезапуска программы.

### Внутреннее состояние
```cpp
string          _filePath;     // "data/ids.dat"
map<string,int> _counters;     // например {"owner":42, "animal":17}
```

### Формат файла (ids.dat)
```
owner 42
animal 17
visit 88
service 5
inventory 12
hospitalization 3
```

### Методы

| Метод | Параметры | Возвращает | Логика |
|---|---|---|---|
| `IdGenerator` | `string filePath` | — | Загружает файл в `_counters`. Если файл не существует — все счётчики с 0. |
| `next` | `string entityName` | `int` | Увеличивает `_counters[entityName]`, сохраняет файл, возвращает новое значение. |
| `reset` | `string entityName` | `void` | Обнуляет счётчик, сохраняет. (Только для тестов.) |

### Логика load()
1. Открыть файл через `ifstream`.
2. Читать пары: `entityName` (строка) затем `value` (int).
3. Сохранить в `_counters`.

### Логика save()
1. Открыть файл через `ofstream` (с очисткой).
2. Записать каждую пару на отдельной строке: `name + " " + value`.

---

## Часть 4 — Repository<T>

**Файл:** `core/Repository.h` (шаблон — всё в заголовочном файле)

### Ограничение шаблона
`T` должен наследовать `ISerializable`.

### Внутреннее состояние
```cpp
vector<T> _items;
string    _filePath;
```

### Конструктор
- Принимает `string filePath`
- Вызывает `load()`

### Публичные методы

**add(const T& item) → void**
1. Если `exists(item.getId())` → бросить `DuplicateIdException(item.getId())`
2. `_items.push_back(item)`
3. `save()`

**getById(int id) → T&**
1. Перебрать `_items`, найти где `item.getId() == id`
2. Если не найдено → бросить `NotFoundException(id)`
3. Вернуть ссылку на найденный элемент

**getAll() → const vector<T>&**
- Вернуть `_items` напрямую

**update(const T& item) → void**
1. Найти индекс где `getId() == item.getId()`
2. Если не найдено → бросить `NotFoundException`
3. Заменить: `_items[index] = item`
4. `save()`

**remove(int id) → void**
1. Найти итератор где `getId() == id`
2. Если не найдено → бросить `NotFoundException(id)`
3. `_items.erase(iterator)`
4. `save()`

**exists(int id) → bool**
- Вернуть true если любой элемент в `_items` имеет этот ID. Без исключений.

**count() → int**
- Вернуть `(int)_items.size()`

### Приватные методы

**load() → void**
1. Открыть `_filePath` через `ifstream`
2. Если файл не существует → вернуться молча (первый запуск)
3. Если не открылся по другой причине → бросить `FileIOException`
4. Цикл: создать объект `T`, вызвать `item.deserialize(in)`, добавить в `_items`
5. До `in.eof()`

**save() → void**
1. Открыть `_filePath` через `ofstream` (с очисткой)
2. Если не открылся → бросить `FileIOException`
3. Для каждого элемента → вызвать `item.serialize(out)`

### Конвенция формата сериализации
Скажи всем участникам следовать этому формату:

```
<поле1>\n
<поле2>\n
...
---\n       ← сигнальная строка, конец одной записи
```

`deserialize` читает поля построчно и останавливается на `---`.
`serialize` записывает поля, затем записывает `---\n` в конце.

---

## Часть 5 — ClinicService

**Файлы:** `core/ClinicService.h`, `core/ClinicService.cpp`

Один экземпляр, создаётся в `main.cpp`, передаётся в `MainWindow`.

### Внутреннее состояние

```cpp
Repository<Owner>                 _owners    {"data/owners.dat"};
Repository<Visit>                 _visits    {"data/visits.dat"};
Repository<InventoryItem>         _inventory {"data/inventory.dat"};
Repository<HospitalizationRecord> _hospital  {"data/hospitalization.dat"};
AnimalRepository*                 _animals;   // полиморфный
ServiceRepository*                _services;  // полиморфный
IdGenerator                       _idGen     {"data/ids.dat"};
```

### Методы Owner

**addOwner(string name, string contact) → Owner**
1. Проверить что name не пустой → бросить `ValidationException` если пустой
2. `int id = _idGen.next("owner")`
3. Создать `Owner(id, name, contact)`
4. `_owners.add(owner)`
5. Вернуть owner

**getOwner(int ownerId) → Owner&**
- Вернуть `_owners.getById(ownerId)` — пробрасывает `NotFoundException`

**getAllOwners() → const vector<Owner>&**
- Вернуть `_owners.getAll()`

**updateOwner(const Owner& owner) → void**
1. Проверить что name не пустой
2. `_owners.update(owner)`

**removeOwner(int ownerId) → void**
1. Проверить что у владельца нет животных → бросить `ValidationException` если есть
2. `_owners.remove(ownerId)`

### Методы Animal

**addAnimal(string type, string name, int age, int ownerId) → Animal***
1. Если не `_owners.exists(ownerId)` → бросить `NotFoundException`
2. Проверить age > 0
3. `int id = _idGen.next("animal")`
4. `Animal* a = AnimalFactory::create(type, id, name, age, ownerId)`
5. `_animals->add(a)`
6. Вернуть a

**getAnimal(int animalId) → Animal***
- Вернуть `_animals->getById(animalId)`

**getAnimalsByOwner(int ownerId) → vector<Animal*>**
- Отфильтровать `_animals->getAll()` где `a->getOwnerId() == ownerId`

**updateAnimal(Animal* animal) → void**
- `_animals->update(animal)`

**removeAnimal(int animalId) → void**
1. Проверить нет ли активной госпитализации → бросить `ValidationException` если есть
2. `_animals->remove(animalId)`

### Методы Visit

**createVisit(int animalId, int ownerId, string date, vector<int> serviceIds) → Visit**
1. Проверить существование animal и owner
2. Проверить каждый serviceId
3. Проверить что date не пустой
4. Подсчитать total = сумма `getFinalPrice()` всех услуг
5. `int id = _idGen.next("visit")`
6. Создать `Visit(id, animalId, ownerId, date, serviceIds, total)`
7. `_visits.add(visit)`
8. Вернуть visit

**getVisitsByAnimal(int animalId) → vector<Visit>**
- Фильтр по animalId, сортировка по дате через `operator<`

**cancelVisit(int visitId) → void**
- `_visits.remove(visitId)`

### Методы Inventory

**stockIn(int itemId, int qty) → void**
1. Получить item через `getById`
2. `item += qty`
3. `_inventory.update(item)`

**stockOut(int itemId, int qty) → void**
1. Получить item через `getById`
2. Если `item.getQuantity() < qty` → бросить `InsufficientStockException`
3. `item -= qty`
4. `_inventory.update(item)`

### Методы Hospitalization

**admitAnimal(int animalId, string ward, string admitDate, double dailyRate) → HospitalizationRecord**
1. Проверить существование animal
2. Проверить нет ли активной записи для этого animal → бросить `ValidationException` если есть
3. Создать и добавить запись

**dischargeAnimal(int recordId, string dischargeDate) → double**
1. Получить запись
2. Проверить что она активна
3. Установить дату выписки
4. Подсчитать дни × дневная ставка + услуги
5. Сохранить, вернуть итоговую сумму

---

## Часть 6 — Qt GUI

Строй GUI после завершения Core. Используй только `ClinicService` — никакого прямого доступа к репозиториям из GUI.

### MainWindow
- Главное окно с вкладками (QTabWidget)
- По одной вкладке на каждый раздел: Животные/Владельцы, Визиты, Инвентарь, Госпитализация
- Владеет единственным экземпляром `ClinicService`

### Виджеты модулей
Каждый виджет следует одному шаблону:
- Таблица (QTableWidget) со списком записей
- Кнопки Добавить / Редактировать / Удалить
- Диалоговое окно для добавления/редактирования
- Все операции вызывают методы `ClinicService`
- Каждый вызов обёрнут в try/catch, при `VPMSException` показывать `QMessageBox`

### Обработка ошибок в GUI
```cpp
try {
    clinicService.addAnimal(...);
} catch (const ValidationException& e) {
    QMessageBox::warning(this, "Ошибка валидации", e.what());
} catch (const VPMSException& e) {
    QMessageBox::critical(this, "Ошибка", e.what());
}
```

---

## Порядок разработки

1. `ISerializable.h` + `exceptions.h` → сразу поделиться со всей командой
2. `IdGenerator` → нужен до ClinicService
3. `Repository<T>` → нужен до ClinicService
4. Заголовки `ClinicService` (только .h) → команде нужны сигнатуры методов
5. Полная реализация `ClinicService`
6. Qt GUI
