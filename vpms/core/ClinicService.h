#pragma once
#include "Repository.h"
#include "IdGenerator.h"
#include "../animals/Owner.h"
#include "../animals/Animal.h"
#include "../animals/AnimalRepository.h"
#include "../animals/AnimalFactory.h"
#include "../visits/Visit.h"
#include "../visits/Service.h"
#include "../visits/ServiceRepository.h"
#include "../visits/ServiceFactory.h"
#include "../inventory/InventoryItem.h"
#include "../inventory/HospitalizationRecord.h"
using namespace std;

#include <string>
#include <vector>

class ClinicService {
public:
    ClinicService();
    ~ClinicService() = default;

    // --- Owner ---
    Owner       addOwner(const string& name, const string& contact);
    Owner&      getOwner(int ownerId);
    const vector<Owner>& getAllOwners();
    void        updateOwner(const Owner& owner);
    void        removeOwner(int ownerId);

    // --- Animal ---
    Animal*     addAnimal(const string& type, const string& name,
                          int age, int ownerId, const string& extra = "");
    Animal*     getAnimal(int animalId);
    const vector<Animal*>& getAllAnimals();
    vector<Animal*> getAnimalsByOwner(int ownerId);
    void        updateAnimal(Animal* animal);
    void        removeAnimal(int animalId);

    // --- Visit ---
    Visit       createVisit(int animalId, int ownerId,
                            const string& date,
                            const vector<int>& serviceIds);
    Visit&      getVisit(int visitId);
    const vector<Visit>& getAllVisits();
    vector<Visit> getVisitsByAnimal(int animalId);
    vector<Visit> getVisitsByOwner(int ownerId);
    void        cancelVisit(int visitId);

    // --- Service Catalog ---
    Service*    addService(const string& type, const string& name,
                           double price, const string& extra = "");
    const vector<Service*>& getAllServices();
    void        removeService(int serviceId);

    // --- Inventory ---
    InventoryItem addInventoryItem(const string& name, int qty,
                                   double unitPrice, const string& category);
    void        stockIn(int itemId, int qty);
    void        stockOut(int itemId, int qty);
    void        removeInventoryItem(int itemId);
    vector<InventoryItem> getLowStockItems(int threshold = 10);
    const vector<InventoryItem>& getAllInventory();

    // --- Hospitalization ---
    HospitalizationRecord admitAnimal(int animalId, const string& ward,
                                      const string& admitDate, double dailyRate);
    double      dischargeAnimal(int recordId, const string& dischargeDate);
    const vector<HospitalizationRecord>& getAllHospitalizations();
    vector<HospitalizationRecord> getActiveHospitalizations();
    void        addServiceToHospitalization(int recordId, int serviceId);

private:
    Repository<Owner>                 _owners;
    Repository<Visit>                 _visits;
    Repository<InventoryItem>         _inventory;
    Repository<HospitalizationRecord> _hospital;
    AnimalRepository*                 _animals;
    ServiceRepository*                _services;
    IdGenerator                       _idGen;
};
