#include "Bird.h"

Bird::Bird() : Animal() {
    _species = "";
    _typeTag = "BIRD";
}

Bird::Bird(int id, const string& name, int age, int ownerId, const string& species) : Animal(id, name, age, ownerId, "BIRD") {
    _species = species;
}

string Bird::getSpeciesInfo() const {
    return "Bird - Species: " + _species;
}

double Bird::calculateTreatmentCost() const {
    return 40.0;
}

void Bird::serialize(ostream& out) const { Animal::serialize(out);

    out << _species << endl;
    out << "---" << endl;
}

void Bird::deserialize(istream& in) { Animal::deserialize(in);

    string line;
    getline(in, _species);
    getline(in, line);
}
