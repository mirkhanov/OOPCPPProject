#include "ClinicService.h"
#include "exceptions.h"
#include <algorithm>
using namespace std;

ClinicService::ClinicService()
    : _owners  ("data/owners.dat"),
      _visits  ("data/visits.dat"),
      _inventory("data/inventory.dat"),
      _hospital ("data/hospitalization.dat"),
      _animals  (new AnimalRepository("data/animals.dat")),
      _services (new ServiceRepository("data/services.dat")),
      _idGen    ("data/ids.dat")
{}

// ──────────────────────────────────────────────
// Owner
// ──────────────────────────────────────────────

Owner ClinicService::addOwner(const string& name, const string& contact) {
    if (name.empty()) throw ValidationException("Owner name cannot be empty.");
    int id = _idGen.next("owner");
    Owner owner(id, name, contact);
    _owners.add(owner);
    return owner;
}

Owner& ClinicService::getOwner(int ownerId) {
    return _owners.getById(ownerId);
}

const vector<Owner>& ClinicService::getAllOwners() {
    return _owners.getAll();
}

void ClinicService::updateOwner(const Owner& owner) {
    if (owner.getName().empty()) throw ValidationException("Owner name cannot be empty.");
    _owners.update(owner);
}

void ClinicService::removeOwner(int ownerId) {
    for (Animal* a : _animals->getAll()) {
        if (a->getOwnerId() == ownerId)
            throw ValidationException("Cannot delete owner: they have registered animals.");
    }
    _owners.remove(ownerId);
}

// ──────────────────────────────────────────────
// Animal
// ──────────────────────────────────────────────

Animal* ClinicService::addAnimal(const string& type, const string& name,
                                  int age, int ownerId) {
    if (!_owners.exists(ownerId)) throw NotFoundException(ownerId);
    if (age <= 0) throw ValidationException("Animal age must be positive.");
    int id = _idGen.next("animal");
    Animal* a = AnimalFactory::create(type, id, name, age, ownerId);
    _animals->add(a);
    return a;
}

Animal* ClinicService::getAnimal(int animalId) {
    return _animals->getById(animalId);
}

vector<Animal*> ClinicService::getAnimalsByOwner(int ownerId) {
    vector<Animal*> result;
    for (Animal* a : _animals->getAll()) {
        if (a->getOwnerId() == ownerId) result.push_back(a);
    }
    return result;
}

void ClinicService::updateAnimal(Animal* animal) {
    _animals->update(animal);
}

void ClinicService::removeAnimal(int animalId) {
    for (const HospitalizationRecord& r : _hospital.getAll()) {
        if (r.getAnimalId() == animalId && r.isActive())
            throw ValidationException("Cannot delete animal: currently hospitalized.");
    }
    _animals->remove(animalId);
}

// ──────────────────────────────────────────────
// Visit
// ──────────────────────────────────────────────

Visit ClinicService::createVisit(int animalId, int ownerId,
                                  const string& date,
                                  const vector<int>& serviceIds) {
    if (!_animals->exists(animalId)) throw NotFoundException(animalId);
    if (!_owners.exists(ownerId))    throw NotFoundException(ownerId);
    if (date.empty()) throw ValidationException("Visit date cannot be empty.");

    double total = 0.0;
    for (int sid : serviceIds) {
        Service* s = _services->getById(sid);
        total += s->getFinalPrice();
    }

    int id = _idGen.next("visit");
    Visit visit(id, animalId, ownerId, date, serviceIds, total);
    _visits.add(visit);
    return visit;
}

Visit& ClinicService::getVisit(int visitId) {
    return _visits.getById(visitId);
}

vector<Visit> ClinicService::getVisitsByAnimal(int animalId) {
    vector<Visit> result;
    for (const Visit& v : _visits.getAll()) {
        if (v.getAnimalId() == animalId) result.push_back(v);
    }
    sort(result.begin(), result.end());
    return result;
}

vector<Visit> ClinicService::getVisitsByOwner(int ownerId) {
    vector<Visit> result;
    for (const Visit& v : _visits.getAll()) {
        if (v.getOwnerId() == ownerId) result.push_back(v);
    }
    return result;
}

void ClinicService::cancelVisit(int visitId) {
    _visits.remove(visitId);
}

// ──────────────────────────────────────────────
// Service Catalog
// ──────────────────────────────────────────────

Service* ClinicService::addService(const string& type, const string& name,
                                    double price, const string& extra) {
    if (price <= 0) throw ValidationException("Service price must be positive.");
    int id = _idGen.next("service");
    Service* s = ServiceFactory::create(type, id, name, price, extra);
    _services->add(s);
    return s;
}

const vector<Service*>& ClinicService::getAllServices() {
    return _services->getAll();
}

void ClinicService::removeService(int serviceId) {
    for (const Visit& v : _visits.getAll()) {
        for (int sid : v.getServiceIds()) {
            if (sid == serviceId)
                throw ValidationException("Cannot delete service: used in existing visits.");
        }
    }
    _services->remove(serviceId);
}

// ──────────────────────────────────────────────
// Inventory
// ──────────────────────────────────────────────

InventoryItem ClinicService::addInventoryItem(const string& name, int qty,
                                               double unitPrice,
                                               const string& category) {
    if (qty < 0)       throw ValidationException("Quantity cannot be negative.");
    if (unitPrice <= 0) throw ValidationException("Unit price must be positive.");
    int id = _idGen.next("inventory");
    InventoryItem item(id, name, qty, unitPrice, category);
    _inventory.add(item);
    return item;
}

void ClinicService::stockIn(int itemId, int qty) {
    InventoryItem item = _inventory.getById(itemId);
    item += qty;
    _inventory.update(item);
}

void ClinicService::stockOut(int itemId, int qty) {
    InventoryItem item = _inventory.getById(itemId);
    if (item.getQuantity() < qty)
        throw InsufficientStockException(item.getName(), item.getQuantity(), qty);
    item -= qty;
    _inventory.update(item);
}

vector<InventoryItem> ClinicService::getLowStockItems(int threshold) {
    vector<InventoryItem> result;
    for (const InventoryItem& item : _inventory.getAll()) {
        if (item.getQuantity() <= threshold) result.push_back(item);
    }
    return result;
}

const vector<InventoryItem>& ClinicService::getAllInventory() {
    return _inventory.getAll();
}

// ──────────────────────────────────────────────
// Hospitalization
// ──────────────────────────────────────────────

HospitalizationRecord ClinicService::admitAnimal(int animalId,
                                                  const string& ward,
                                                  const string& admitDate,
                                                  double dailyRate) {
    if (!_animals->exists(animalId)) throw NotFoundException(animalId);

    for (const HospitalizationRecord& r : _hospital.getAll()) {
        if (r.getAnimalId() == animalId && r.isActive())
            throw ValidationException("Animal is already admitted.");
    }

    int id = _idGen.next("hospitalization");
    HospitalizationRecord record(id, animalId, ward, admitDate, dailyRate);
    _hospital.add(record);
    return record;
}

double ClinicService::dischargeAnimal(int recordId, const string& dischargeDate) {
    HospitalizationRecord record = _hospital.getById(recordId);

    if (!record.isActive())
        throw ValidationException("Animal has already been discharged.");

    record.setDischargeDate(dischargeDate);

    // Calculate days between dates (simple string-based day count)
    // Dates are "YYYY-MM-DD" — convert to day count via tm struct
    int days = 1;
    struct tm admit = {}, discharge = {};
    strptime(record.getAdmitDate().c_str(),    "%Y-%m-%d", &admit);
    strptime(dischargeDate.c_str(), "%Y-%m-%d", &discharge);
    time_t t1 = mktime(&admit);
    time_t t2 = mktime(&discharge);
    if (t1 != -1 && t2 != -1 && t2 > t1) {
        days = (int)((t2 - t1) / 86400);
    }

    double servicesCost = 0.0;
    for (int sid : record.getServiceIds()) {
        servicesCost += _services->getById(sid)->getFinalPrice();
    }

    double total = days * record.getDailyRate() + servicesCost;
    record.setTotalBill(total);
    _hospital.update(record);
    return total;
}

vector<HospitalizationRecord> ClinicService::getActiveHospitalizations() {
    vector<HospitalizationRecord> result;
    for (const HospitalizationRecord& r : _hospital.getAll()) {
        if (r.isActive()) result.push_back(r);
    }
    return result;
}

void ClinicService::addServiceToHospitalization(int recordId, int serviceId) {
    if (!_services->exists(serviceId)) throw NotFoundException(serviceId);
    HospitalizationRecord record = _hospital.getById(recordId);
    record.addServiceId(serviceId);
    _hospital.update(record);
}
