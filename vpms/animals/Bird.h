#pragma once
#include "Animal.h"
using namespace std;

class Bird : public Animal {
public:
    Bird() : Animal() { _typeTag = "BIRD"; }
    Bird(int id, const string& name, int age, int ownerId, const string& species)
        : Animal(id, name, age, ownerId, "BIRD"), _species(species) {}

    string getSpeciesInfo()         const override { return "Bird - Species: " + _species; }
    double calculateTreatmentCost() const override { return 40.0; }

    void serialize(ostream& out) const override {
        Animal::serialize(out);
        out << _species << "\n---\n";
    }

    void deserialize(istream& in) override {
        Animal::deserialize(in);
        in.ignore();
        getline(in, _species);
        string sentinel; getline(in, sentinel);
    }

private:
    string _species;
};
