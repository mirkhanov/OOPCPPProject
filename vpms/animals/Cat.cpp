#include "Cat.h"

Cat::Cat() : Animal() { _typeTag = "CAT"; }

Cat::Cat(int id, const string& name, int age, int ownerId, const string& furType)
    : Animal(id, name, age, ownerId, "CAT"), _furType(furType) {}

string Cat::getSpeciesInfo()         const { return "Cat - Fur: " + _furType; }
double Cat::calculateTreatmentCost() const { return 60.0; }

void Cat::serialize(ostream& out) const {
    Animal::serialize(out);
    out << _furType << "\n---\n";
}

void Cat::deserialize(istream& in) {
    Animal::deserialize(in);
    getline(in, _furType);
    string sentinel; getline(in, sentinel);
}
