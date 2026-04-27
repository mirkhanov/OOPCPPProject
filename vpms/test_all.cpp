#include <iostream>
#include <cassert>
#include "core/ClinicService.h"
using namespace std;

int passed = 0, failed = 0;

#define TEST(name, ...) \
    try { \
        [&]() { __VA_ARGS__ }(); \
        cout << "[PASS] " << name << "\n"; passed++; \
    } catch (const exception& e) { \
        cout << "[FAIL] " << name << " — " << e.what() << "\n"; failed++; \
    }

#define TEST_THROWS(name, ...) \
    try { \
        [&]() { __VA_ARGS__ }(); \
        cout << "[FAIL] " << name << " — expected exception\n"; failed++; \
    } catch (const exception&) { \
        cout << "[PASS] " << name << "\n"; passed++; \
    }

int main() {
    system("rm -f data/owners.dat data/animals.dat data/visits.dat "
           "data/services.dat data/inventory.dat data/hospitalization.dat data/ids.dat");

    ClinicService svc;

    cout << "\n=== OWNERS ===\n";
    TEST("Add owner",
        Owner o = svc.addOwner("Alice", "555-1234");
        assert(o.getId() == 1);
        assert(o.getName() == "Alice");
    )
    TEST("Get owner",
        Owner& o = svc.getOwner(1);
        assert(o.getName() == "Alice");
    )
    TEST("Update owner",
        Owner o = svc.getOwner(1);
        o.setName("Alice Updated");
        svc.updateOwner(o);
        assert(svc.getOwner(1).getName() == "Alice Updated");
    )
    TEST("Add second owner",
        svc.addOwner("Bob", "555-9999");
    )
    TEST_THROWS("Empty owner name",
        svc.addOwner("", "555-0000");
    )
    TEST_THROWS("Get nonexistent owner",
        svc.getOwner(999);
    )

    cout << "\n=== ANIMALS ===\n";
    TEST("Add dog",
        Animal* a = svc.addAnimal("DOG", "Rex", 3, 1);
        assert(a != nullptr);
        assert(a->getName() == "Rex");
        assert(a->getTypeTag() == "DOG");
    )
    TEST("Add cat",
        Animal* a = svc.addAnimal("CAT", "Whiskers", 2, 1);
        assert(a != nullptr);
    )
    TEST("Add bird",
        Animal* a = svc.addAnimal("BIRD", "Tweety", 1, 2);
        assert(a != nullptr);
    )
    TEST("Add reptile",
        Animal* a = svc.addAnimal("REPTILE", "Sly", 5, 2);
        assert(a != nullptr);
    )
    TEST("Get all animals",
        assert(svc.getAllAnimals().size() == 4);
    )
    TEST("Get animals by owner",
        auto v = svc.getAnimalsByOwner(1);
        assert(v.size() == 2);
    )
    TEST("Update animal",
        Animal* a = svc.getAnimal(1);
        a->setAge(4);
        svc.updateAnimal(a);
        assert(svc.getAnimal(1)->getAge() == 4);
    )
    TEST_THROWS("Negative age",
        svc.addAnimal("DOG", "Bad", -1, 1);
    )
    TEST_THROWS("Nonexistent owner",
        svc.addAnimal("CAT", "Nobody", 2, 999);
    )
    TEST_THROWS("Unknown animal type",
        svc.addAnimal("FISH", "Nemo", 1, 1);
    )

    cout << "\n=== SERVICES ===\n";
    TEST("Add consultation",
        Service* s = svc.addService("CONSULTATION", "General Checkup", 50.0);
        assert(s != nullptr);
    )
    TEST("Add vaccination",
        Service* s = svc.addService("VACCINATION", "Rabies Vaccine", 30.0, "Rabies");
        assert(s != nullptr);
    )
    TEST("Add surgery",
        Service* s = svc.addService("SURGERY", "Spay/Neuter", 200.0, "Routine");
        assert(s != nullptr);
    )
    TEST("Add grooming",
        Service* s = svc.addService("GROOMING", "Full Groom", 40.0, "Full");
        assert(s != nullptr);
    )
    TEST("Get all services",
        assert(svc.getAllServices().size() == 4);
    )
    TEST_THROWS("Zero price service",
        svc.addService("CONSULTATION", "Free", 0.0);
    )

    cout << "\n=== VISITS ===\n";
    TEST("Create visit with services",
        Visit v = svc.createVisit(1, 1, "2026-04-27", {1, 2});
        assert(v.getAnimalId() == 1);
        assert(v.getTotalCost() > 0);
    )
    TEST("Create visit no services",
        Visit v = svc.createVisit(2, 1, "2026-04-28", {});
        assert(v.getId() == 2);
    )
    TEST("Get all visits",
        assert(svc.getAllVisits().size() == 2);
    )
    TEST("Get visits by animal",
        auto v = svc.getVisitsByAnimal(1);
        assert(v.size() == 1);
    )
    TEST_THROWS("Visit with empty date",
        svc.createVisit(1, 1, "", {});
    )
    TEST_THROWS("Visit with nonexistent animal",
        svc.createVisit(999, 1, "2026-04-27", {});
    )
    TEST("Cancel visit",
        svc.cancelVisit(2);
        assert(svc.getAllVisits().size() == 1);
    )

    cout << "\n=== INVENTORY ===\n";
    TEST("Add item",
        InventoryItem item = svc.addInventoryItem("Aspirin", 100, 0.50, "Medication");
        assert(item.getQuantity() == 100);
    )
    TEST("Add second item",
        svc.addInventoryItem("Syringe", 50, 1.20, "Equipment");
    )
    TEST("Stock in",
        svc.stockIn(1, 20);
        assert(svc.getAllInventory()[0].getQuantity() == 120);
    )
    TEST("Stock out",
        svc.stockOut(1, 30);
        assert(svc.getAllInventory()[0].getQuantity() == 90);
    )
    TEST("Low stock detection",
        svc.addInventoryItem("Bandage", 5, 0.10, "Supplies");
        auto low = svc.getLowStockItems(10);
        assert(!low.empty());
    )
    TEST_THROWS("Stock out more than available",
        svc.stockOut(1, 99999);
    )
    TEST("Remove inventory item",
        svc.removeInventoryItem(2);
        assert(svc.getAllInventory().size() == 2);
    )

    cout << "\n=== HOSPITALIZATION ===\n";
    TEST("Admit animal",
        HospitalizationRecord r = svc.admitAnimal(3, "Ward A", "2026-04-20", 100.0);
        assert(r.isActive());
    )
    TEST_THROWS("Admit same animal twice",
        svc.admitAnimal(3, "Ward B", "2026-04-21", 120.0);
    )
    TEST("Add service to hospitalization",
        svc.addServiceToHospitalization(1, 1);
    )
    TEST("Get active hospitalizations",
        auto active = svc.getActiveHospitalizations();
        assert(active.size() == 1);
    )
    TEST("Discharge animal",
        double bill = svc.dischargeAnimal(1, "2026-04-27");
        assert(bill > 0);
        cout << "     Bill: $" << bill << "\n";
    )
    TEST_THROWS("Discharge already discharged",
        svc.dischargeAnimal(1, "2026-04-28");
    )

    cout << "\n=== PERSISTENCE (reload from files) ===\n";
    TEST("Reload ClinicService from disk",
        ClinicService svc2;
        assert(svc2.getAllOwners().size() == 2);
        assert(svc2.getAllAnimals().size() == 4);
        assert(svc2.getAllVisits().size() == 1);
        assert(svc2.getAllServices().size() == 4);
        assert(svc2.getAllInventory().size() == 2);
        assert(svc2.getAllHospitalizations().size() == 1);
    )

    cout << "\n=== VALIDATION EDGE CASES ===\n";
    TEST_THROWS("Delete owner with animals",
        svc.removeOwner(1);
    )
    TEST_THROWS("Delete service used in visit",
        svc.removeService(1);
    )
    TEST("Admit animal 4 for delete test",
        svc.admitAnimal(4, "Ward C", "2026-04-25", 80.0);
    )
    TEST_THROWS("Delete hospitalized animal",
        svc.removeAnimal(4);
    )

    cout << "\n================================\n";
    cout << "PASSED: " << passed << " / " << (passed + failed) << "\n";
    if (failed > 0) cout << "FAILED: " << failed << "\n";
    cout << "================================\n";

    return failed > 0 ? 1 : 0;
}
