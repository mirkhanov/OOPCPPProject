#include "Reptile.h"

Reptile::Reptile() : Animal() { _typeTag = "REPTILE"; }

Reptile::Reptile(int id, const string& name, int age, int ownerId, const string& reptileType)
    : Animal(id, name, age, ownerId, "REPTILE"), _reptileType(reptileType) {}

string Reptile::getSpeciesInfo()         const { return "Reptile - Type: " + _reptileType; }
double Reptile::calculateTreatmentCost() const { return 50.0; }

void Reptile::serialize(ostream& out) const {
    Animal::serialize(out);
    out << _reptileType << "\n---\n";
}

void Reptile::deserialize(istream& in) {
    Animal::deserialize(in);
    getline(in, _reptileType);
    string sentinel; getline(in, sentinel);
}
