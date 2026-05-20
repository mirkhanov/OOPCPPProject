#include "AnimalFactory.h"
#include "Dog.h"
#include "Cat.h"
#include "Bird.h"
#include "Reptile.h"
#include "../core/exceptions.h"
using namespace std;

Animal* AnimalFactory::create(const string& typeTag, int id,
                               const string& name, int age, int ownerId) {
    if (typeTag == "DOG")     return new Dog(id, name, age, ownerId, "Unknown");
    if (typeTag == "CAT")     return new Cat(id, name, age, ownerId, "Unknown");
    if (typeTag == "BIRD")    return new Bird(id, name, age, ownerId, "Unknown");
    if (typeTag == "REPTILE") return new Reptile(id, name, age, ownerId, "Unknown");
    throw ValidationException("Unknown animal type: " + typeTag);
}

Animal* AnimalFactory::createFromStream(const string& typeTag, istream& in) {
    Animal* a = nullptr;
    if      (typeTag == "DOG")     a = new Dog();
    else if (typeTag == "CAT")     a = new Cat();
    else if (typeTag == "BIRD")    a = new Bird();
    else if (typeTag == "REPTILE") a = new Reptile();
    else throw ValidationException("Unknown animal type: " + typeTag);
    a->deserialize(in);
    return a;
}
