#include <QTest>
#include <QObject>
#include <QCoreApplication>
#include <sstream>
#include <cstdio>

#include "animals/Animal.h"
#include "animals/Dog.h"
#include "animals/Cat.h"
#include "animals/Bird.h"
#include "animals/Reptile.h"
#include "animals/Owner.h"
#include "animals/AnimalFactory.h"
#include "animals/AnimalRepository.h"
#include "visits/Service.h"
#include "visits/Consultation.h"
#include "visits/Vaccination.h"
#include "visits/Surgery.h"
#include "visits/Grooming.h"
#include "visits/Visit.h"
#include "visits/ServiceFactory.h"
#include "inventory/InventoryItem.h"
#include "inventory/HospitalizationRecord.h"
#include "core/Repository.h"
#include "core/exceptions.h"
#include "core/IdGenerator.h"

using namespace std;

// helper: check that expr throws T
#define THROWS(T, expr) \
    do { bool _caught = false; \
         try { expr; } catch (const T&) { _caught = true; } \
         QVERIFY(_caught); } while(0)

// ════════════════════════════════════════════════════════
//  TestAnimals
// ════════════════════════════════════════════════════════
class TestAnimals : public QObject {
    Q_OBJECT
private slots:
    void dogGetters();
    void dogSpeciesInfo();
    void dogTreatmentCost();
    void catTreatmentCost();
    void birdTreatmentCost();
    void reptileTreatmentCost();
    void polymorphicDispatch();
    void dogSerializeDeserialize();
    void catSerializeDeserialize();
    void ownerGettersAndSetters();
    void ownerEquality();
    void animalFactoryType();
    void animalFactoryUnknownThrows();
};

void TestAnimals::dogGetters() {
    Dog d(1, "Rex", 5, 10, "German Shepherd");
    QCOMPARE(d.getId(), 1);
    QCOMPARE(d.getName(), string("Rex"));
    QCOMPARE(d.getAge(), 5);
    QCOMPARE(d.getOwnerId(), 10);
    QCOMPARE(d.getTypeTag(), string("DOG"));
}

void TestAnimals::dogSpeciesInfo() {
    Dog d(1, "Rex", 3, 1, "Labrador");
    QVERIFY(d.getSpeciesInfo().find("Labrador") != string::npos);
}

void TestAnimals::dogTreatmentCost() {
    Dog d(1, "Rex", 3, 1, "Labrador");
    QCOMPARE(d.calculateTreatmentCost(), 80.0);
}

void TestAnimals::catTreatmentCost() {
    Cat c(2, "Whiskers", 4, 1, "Persian");
    QCOMPARE(c.calculateTreatmentCost(), 60.0);
}

void TestAnimals::birdTreatmentCost() {
    Bird b(3, "Tweety", 2, 1, "Canary");
    QCOMPARE(b.calculateTreatmentCost(), 40.0);
}

void TestAnimals::reptileTreatmentCost() {
    Reptile r(4, "Spike", 3, 1, "Iguana");
    QCOMPARE(r.calculateTreatmentCost(), 50.0);
}

void TestAnimals::polymorphicDispatch() {
    Animal* zoo[] = {
        new Dog(1,     "D", 1, 1, "Breed"),
        new Cat(2,     "C", 2, 1, "Fur"),
        new Bird(3,    "B", 1, 1, "Parrot"),
        new Reptile(4, "R", 2, 1, "Iguana")
    };
    double expected[] = { 80.0, 60.0, 40.0, 50.0 };
    for (int i = 0; i < 4; i++) {
        QCOMPARE(zoo[i]->calculateTreatmentCost(), expected[i]);
        delete zoo[i];
    }
}

void TestAnimals::dogSerializeDeserialize() {
    Dog original(7, "Buddy", 4, 2, "Golden Retriever");
    stringstream ss;
    original.serialize(ss);

    string typeTag; getline(ss, typeTag); // consumed by AnimalRepository in production
    QCOMPARE(typeTag, string("DOG"));

    Dog loaded;
    loaded.deserialize(ss);
    QCOMPARE(loaded.getId(), 7);
    QCOMPARE(loaded.getName(), string("Buddy"));
    QCOMPARE(loaded.getAge(), 4);
    QCOMPARE(loaded.getOwnerId(), 2);
    QVERIFY(loaded.getSpeciesInfo().find("Golden Retriever") != string::npos);
}

void TestAnimals::catSerializeDeserialize() {
    Cat original(3, "Luna", 2, 1, "Siamese");
    stringstream ss;
    original.serialize(ss);
    string typeTag; getline(ss, typeTag);
    Cat loaded;
    loaded.deserialize(ss);
    QCOMPARE(loaded.getId(), 3);
    QCOMPARE(loaded.getName(), string("Luna"));
    QVERIFY(loaded.getSpeciesInfo().find("Siamese") != string::npos);
}

void TestAnimals::ownerGettersAndSetters() {
    Owner o(1, "John Doe", "+1 555-0100");
    QCOMPARE(o.getId(), 1);
    QCOMPARE(o.getName(), string("John Doe"));
    QCOMPARE(o.getContactInfo(), string("+1 555-0100"));
    o.setName("Jane Doe");
    QCOMPARE(o.getName(), string("Jane Doe"));
}

void TestAnimals::ownerEquality() {
    Owner a(1, "Alice", "alice@mail.com");
    Owner b(1, "Bob",   "bob@mail.com"); // same id
    Owner c(2, "Alice", "alice@mail.com"); // different id
    QVERIFY(a == b);
    QVERIFY(!(a == c));
}

void TestAnimals::animalFactoryType() {
    Animal* dog  = AnimalFactory::create("DOG",     1, "Rex",  3, 1, "Lab");
    Animal* cat  = AnimalFactory::create("CAT",     2, "Luna", 2, 1, "Tabby");
    Animal* bird = AnimalFactory::create("BIRD",    3, "Kiwi", 1, 1, "Parrot");
    Animal* rep  = AnimalFactory::create("REPTILE", 4, "Gex",  2, 1, "Gecko");
    QCOMPARE(dog->getTypeTag(),  string("DOG"));
    QCOMPARE(cat->getTypeTag(),  string("CAT"));
    QCOMPARE(bird->getTypeTag(), string("BIRD"));
    QCOMPARE(rep->getTypeTag(),  string("REPTILE"));
    delete dog; delete cat; delete bird; delete rep;
}

void TestAnimals::animalFactoryUnknownThrows() {
    THROWS(VPMSException,
        AnimalFactory::create("FISH", 1, "Nemo", 1, 1, ""));
}

// ════════════════════════════════════════════════════════
//  TestVisits
// ════════════════════════════════════════════════════════
class TestVisits : public QObject {
    Q_OBJECT
private slots:
    void consultationPrice();
    void consultationDescription();
    void vaccinationPrice();
    void surgeryWithAnesthesia();
    void surgeryWithoutAnesthesia();
    void surgeryDiscountCap();
    void groomingPremium();
    void groomingBasic();
    void groomingStandard();
    void visitGetters();
    void visitOrderByDate();
    void visitEquality();
    void consultationSerializeDeserialize();
    void serviceFactoryTypes();
    void serviceFactoryUnknownThrows();
};

void TestVisits::consultationPrice() {
    Consultation c(1, "Checkup", 50.0, 30);
    QCOMPARE(c.getFinalPrice(), 50.0);
}

void TestVisits::consultationDescription() {
    Consultation c(1, "Checkup", 50.0, 45);
    QVERIFY(c.getDescription().find("45") != string::npos);
}

void TestVisits::vaccinationPrice() {
    Vaccination v(2, "Rabies", 35.0, "Rabies");
    QCOMPARE(v.getFinalPrice(), 35.0);
}

void TestVisits::surgeryWithAnesthesia() {
    Surgery s(3, "Op", 200.0, "Spay", true);
    QCOMPARE(s.getFinalPrice(), 240.0);
}

void TestVisits::surgeryWithoutAnesthesia() {
    Surgery s(4, "Op", 200.0, "Incision", false);
    QCOMPARE(s.getFinalPrice(), 200.0);
}

void TestVisits::surgeryDiscountCap() {
    Surgery s(5, "Op", 100.0, "Type", false);
    s.applyDiscount(50.0); // capped at 10%
    QCOMPARE(s.getFinalPrice(), 90.0);
}

void TestVisits::groomingPremium() {
    Grooming g(6, "Full Groom", 40.0, "Premium");
    QCOMPARE(g.getFinalPrice(), 40.0 * 1.15);
}

void TestVisits::groomingBasic() {
    Grooming g(7, "Bath", 20.0, "Basic");
    QCOMPARE(g.getFinalPrice(), 20.0 * 0.90);
}

void TestVisits::groomingStandard() {
    Grooming g(8, "Trim", 30.0, "Standard");
    QCOMPARE(g.getFinalPrice(), 30.0);
}

void TestVisits::visitGetters() {
    Visit v(1, 5, 2, "2026-04-01", {3, 4}, 150.0);
    QCOMPARE(v.getId(), 1);
    QCOMPARE(v.getAnimalId(), 5);
    QCOMPARE(v.getOwnerId(), 2);
    QCOMPARE(v.getDate(), string("2026-04-01"));
    QCOMPARE(v.getTotalCost(), 150.0);
    QCOMPARE((int)v.getServiceIds().size(), 2);
}

void TestVisits::visitOrderByDate() {
    Visit a(1, 1, 1, "2026-03-01", {}, 0.0);
    Visit b(2, 1, 1, "2026-04-01", {}, 0.0);
    QVERIFY(a < b);
    QVERIFY(!(b < a));
}

void TestVisits::visitEquality() {
    Visit a(1, 1, 1, "2026-03-01", {}, 0.0);
    Visit b(1, 9, 9, "2026-12-31", {5}, 999.0); // same id only
    Visit c(2, 1, 1, "2026-03-01", {}, 0.0);
    QVERIFY(a == b);
    QVERIFY(!(a == c));
}

void TestVisits::consultationSerializeDeserialize() {
    Consultation original(1, "General Checkup", 50.0, 30);
    stringstream ss;
    original.serialize(ss);
    string typeTag; getline(ss, typeTag);
    QCOMPARE(typeTag, string("CONSULTATION"));
    Consultation loaded;
    loaded.deserialize(ss);
    QCOMPARE(loaded.getId(), 1);
    QCOMPARE(loaded.getName(), string("General Checkup"));
    QCOMPARE(loaded.getFinalPrice(), 50.0);
}

void TestVisits::serviceFactoryTypes() {
    Service* c = ServiceFactory::create("CONSULTATION", 1, "Check", 50.0, "30");
    Service* v = ServiceFactory::create("VACCINATION",  2, "Rabies", 35.0, "Rabies");
    Service* s = ServiceFactory::create("SURGERY",      3, "Op",    200.0, "Type|0");
    Service* g = ServiceFactory::create("GROOMING",     4, "Bath",   25.0, "Basic");
    QCOMPARE(c->getTypeTag(), string("CONSULTATION"));
    QCOMPARE(v->getTypeTag(), string("VACCINATION"));
    QCOMPARE(s->getTypeTag(), string("SURGERY"));
    QCOMPARE(g->getTypeTag(), string("GROOMING"));
    delete c; delete v; delete s; delete g;
}

void TestVisits::serviceFactoryUnknownThrows() {
    THROWS(VPMSException,
        ServiceFactory::create("XRAY", 1, "X-Ray", 100.0, ""));
}

// ════════════════════════════════════════════════════════
//  TestInventory
// ════════════════════════════════════════════════════════
class TestInventory : public QObject {
    Q_OBJECT
private slots:
    void inventoryGetters();
    void stockIn();
    void stockOut();
    void inventoryEquality();
    void inventoryOrder();
    void inventorySerializeDeserialize();
    void hospitalizationGetters();
    void hospitalizationIsActive();
    void hospitalizationDischarge();
    void hospitalizationSerializeDeserialize();
};

void TestInventory::inventoryGetters() {
    InventoryItem item(1, "Bandages", 100, 2.5, "Supplies");
    QCOMPARE(item.getId(), 1);
    QCOMPARE(item.getName(), string("Bandages"));
    QCOMPARE(item.getQuantity(), 100);
    QCOMPARE(item.getUnitPrice(), 2.5);
    QCOMPARE(item.getCategory(), string("Supplies"));
}

void TestInventory::stockIn() {
    InventoryItem item(1, "Syringes", 50, 1.0, "Supplies");
    item += 25;
    QCOMPARE(item.getQuantity(), 75);
}

void TestInventory::stockOut() {
    InventoryItem item(1, "Syringes", 50, 1.0, "Supplies");
    item -= 20;
    QCOMPARE(item.getQuantity(), 30);
}

void TestInventory::inventoryEquality() {
    InventoryItem a(1, "A", 10, 1.0, "Cat1");
    InventoryItem b(1, "B", 20, 2.0, "Cat2"); // same id
    InventoryItem c(2, "A", 10, 1.0, "Cat1");
    QVERIFY(a == b);
    QVERIFY(!(a == c));
}

void TestInventory::inventoryOrder() {
    InventoryItem a(1, "Bandages", 10, 1.0, "Supplies");
    InventoryItem b(2, "Vaccine",  10, 1.0, "Vaccines");
    QVERIFY(a < b); // "Bandages" < "Vaccine"
    QVERIFY(!(b < a));
}

void TestInventory::inventorySerializeDeserialize() {
    InventoryItem original(3, "Antiseptic", 200, 4.5, "Supplies");
    stringstream ss;
    original.serialize(ss);
    InventoryItem loaded;
    loaded.deserialize(ss);
    QCOMPARE(loaded.getId(), 3);
    QCOMPARE(loaded.getName(), string("Antiseptic"));
    QCOMPARE(loaded.getQuantity(), 200);
    QCOMPARE(loaded.getUnitPrice(), 4.5);
    QCOMPARE(loaded.getCategory(), string("Supplies"));
}

void TestInventory::hospitalizationGetters() {
    HospitalizationRecord r(1, 5, "Ward-A", "2026-04-10", 80.0);
    QCOMPARE(r.getId(), 1);
    QCOMPARE(r.getAnimalId(), 5);
    QCOMPARE(r.getWard(), string("Ward-A"));
    QCOMPARE(r.getAdmitDate(), string("2026-04-10"));
    QCOMPARE(r.getDailyRate(), 80.0);
    QCOMPARE(r.getTotalBill(), 0.0);
}

void TestInventory::hospitalizationIsActive() {
    HospitalizationRecord r(1, 5, "Ward-A", "2026-04-10", 80.0);
    QVERIFY(r.isActive());
}

void TestInventory::hospitalizationDischarge() {
    HospitalizationRecord r(1, 5, "Ward-A", "2026-04-10", 80.0);
    r.setDischargeDate("2026-04-15");
    r.setTotalBill(400.0);
    QVERIFY(!r.isActive());
    QCOMPARE(r.getDischargeDate(), string("2026-04-15"));
    QCOMPARE(r.getTotalBill(), 400.0);
}

void TestInventory::hospitalizationSerializeDeserialize() {
    HospitalizationRecord original(2, 3, "ICU", "2026-05-01", 120.0);
    original.setDischargeDate("2026-05-05");
    original.setTotalBill(480.0);
    stringstream ss;
    original.serialize(ss);
    HospitalizationRecord loaded;
    loaded.deserialize(ss);
    QCOMPARE(loaded.getId(), 2);
    QCOMPARE(loaded.getWard(), string("ICU"));
    QCOMPARE(loaded.getAdmitDate(), string("2026-05-01"));
    QCOMPARE(loaded.getDischargeDate(), string("2026-05-05"));
    QCOMPARE(loaded.getTotalBill(), 480.0);
    QVERIFY(!loaded.isActive());
}

// ════════════════════════════════════════════════════════
//  TestCore
// ════════════════════════════════════════════════════════
class TestCore : public QObject {
    Q_OBJECT
    static constexpr const char* TMP_OWNERS = "/tmp/vpms_test_owners.dat";
    static constexpr const char* TMP_IDS    = "/tmp/vpms_test_ids.dat";
private slots:
    void cleanup();
    void repositoryAdd();
    void repositoryDuplicate();
    void repositoryNotFound();
    void repositoryRemove();
    void repositoryExists();
    void repositoryCount();
    void repositoryPersistence();
    void exceptionMessages();
    void idGeneratorNext();
    void idGeneratorPersistence();
};

void TestCore::cleanup() {
    remove(TMP_OWNERS);
    remove(TMP_IDS);
}

void TestCore::repositoryAdd() {
    Repository<Owner> repo(TMP_OWNERS);
    Owner o(1, "Alice", "alice@mail.com");
    repo.add(o);
    QCOMPARE(repo.getById(1).getName(), string("Alice"));
}

void TestCore::repositoryDuplicate() {
    Repository<Owner> repo(TMP_OWNERS);
    repo.add(Owner(1, "Alice", "a@b.com"));
    THROWS(DuplicateIdException, repo.add(Owner(1, "Bob", "b@b.com")));
}

void TestCore::repositoryNotFound() {
    Repository<Owner> repo(TMP_OWNERS);
    THROWS(NotFoundException, repo.getById(999));
}

void TestCore::repositoryRemove() {
    Repository<Owner> repo(TMP_OWNERS);
    repo.add(Owner(1, "Alice", "a@b.com"));
    repo.remove(1);
    QVERIFY(!repo.exists(1));
}

void TestCore::repositoryExists() {
    Repository<Owner> repo(TMP_OWNERS);
    repo.add(Owner(1, "Alice", "a@b.com"));
    QVERIFY(repo.exists(1));
    QVERIFY(!repo.exists(2));
}

void TestCore::repositoryCount() {
    Repository<Owner> repo(TMP_OWNERS);
    QCOMPARE(repo.count(), 0);
    repo.add(Owner(1, "Alice", "a@b.com"));
    repo.add(Owner(2, "Bob",   "b@b.com"));
    QCOMPARE(repo.count(), 2);
}

void TestCore::repositoryPersistence() {
    {
        Repository<Owner> repo(TMP_OWNERS);
        repo.add(Owner(1, "Alice", "alice@mail.com"));
        repo.add(Owner(2, "Bob",   "bob@mail.com"));
    }
    Repository<Owner> repo2(TMP_OWNERS);
    QCOMPARE(repo2.count(), 2);
    QCOMPARE(repo2.getById(1).getName(), string("Alice"));
    QCOMPARE(repo2.getById(2).getName(), string("Bob"));
}

void TestCore::exceptionMessages() {
    NotFoundException nf(42);
    QVERIFY(string(nf.what()).find("42") != string::npos);

    DuplicateIdException dup(7);
    QVERIFY(string(dup.what()).find("7") != string::npos);

    ValidationException val("age must be positive");
    QVERIFY(string(val.what()).find("age") != string::npos);

    InsufficientStockException ise("Bandages", 3, 10);
    QVERIFY(string(ise.what()).find("Bandages") != string::npos);
}

void TestCore::idGeneratorNext() {
    IdGenerator gen(TMP_IDS);
    QCOMPARE(gen.next("owner"), 1);
    QCOMPARE(gen.next("owner"), 2);
    QCOMPARE(gen.next("owner"), 3);
    QCOMPARE(gen.next("animal"), 1);
}

void TestCore::idGeneratorPersistence() {
    {
        IdGenerator gen(TMP_IDS);
        gen.next("visit"); // 1
        gen.next("visit"); // 2
        gen.next("visit"); // 3
    }
    IdGenerator gen2(TMP_IDS);
    QCOMPARE(gen2.next("visit"), 4); // continues from 3
}

// ════════════════════════════════════════════════════════
//  main
// ════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    int status = 0;
    TestAnimals   ta; status |= QTest::qExec(&ta, argc, argv);
    TestVisits    tv; status |= QTest::qExec(&tv, argc, argv);
    TestInventory ti; status |= QTest::qExec(&ti, argc, argv);
    TestCore      tc; status |= QTest::qExec(&tc, argc, argv);
    return status;
}

#include "tests.moc"
