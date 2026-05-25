#include "AnimalFactory.h"
#include "Dog.h"
#include "Cat.h"
#include "Bird.h"
#include "Reptile.h"
#include "../core/exceptions.h"

Animal* AnimalFactory::createFromStream(const string& typeTag, istream& in) {

    Animal* animal = nullptr;

    if (typeTag == "DOG") {
        animal = new Dog();}

    else if (typeTag == "CAT") {
        animal = new Cat();
    }

    else if (typeTag == "BIRD") {
        animal = new Bird();
    }
    else if (typeTag == "REPTILE") {
        animal = new Reptile();
    }

    else {
        throw ValidationException("Unknown animal type: " + typeTag);
    }

    animal->deserialize(in);

    return animal;
}

Animal* AnimalFactory::create(const string& typeTag, int id, const string& name, int age, int ownerId, const string& extra) {

    if (typeTag == "DOG") {
        return new Dog(id, name, age, ownerId, extra.empty() ? "Unknown Breed" : extra);
    }

    if (typeTag == "CAT") {
        return new Cat(id, name, age, ownerId, extra.empty() ? "Short" : extra);
    }

    if (typeTag == "BIRD") {
        return new Bird(id, name, age, ownerId, extra.empty() ? "Parrot" : extra);
    }

    if (typeTag == "REPTILE") {
        return new Reptile(id, name, age, ownerId, extra.empty() ? "Lizard" : extra);
    }

    throw ValidationException("Unknown animal type: " + typeTag);
}
