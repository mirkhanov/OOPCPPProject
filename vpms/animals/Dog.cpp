#include "Dog.h"

Dog::Dog() : Animal() {
    _breed = "";
    _typeTag = "DOG";
}

Dog::Dog(int id, const string& name, int age, int ownerId, const string& breed) : Animal(id, name, age, ownerId, "DOG") {
    _breed = breed;
}

string Dog::getSpeciesInfo() const {
    return "Dog - Breed: " + _breed;
}

double Dog::calculateTreatmentCost() const {
    return 80.0;
}

void Dog::serialize(ostream& out) const { Animal::serialize(out);
    out << _breed << endl;
    out << "---" << endl;
}

void Dog::deserialize(istream& in) { Animal::deserialize(in);
    string line;
    getline(in, _breed);
    getline(in, line);
}
