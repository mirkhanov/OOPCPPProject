#pragma once
#include "Animal.h"
using namespace std;

class Reptile : public Animal {
public:
    Reptile() : Animal() { _typeTag = "REPTILE"; }
    Reptile(int id, const string& name, int age, int ownerId, const string& reptileType)
        : Animal(id, name, age, ownerId, "REPTILE"), _reptileType(reptileType) {}

    string getSpeciesInfo()         const override { return "Reptile - Type: " + _reptileType; }
    double calculateTreatmentCost() const override { return 50.0; }

    void serialize(ostream& out) const override {
        Animal::serialize(out);
        out << _reptileType << "\n---\n";
    }

    void deserialize(istream& in) override {
        Animal::deserialize(in);
        in.ignore();
        getline(in, _reptileType);
        string sentinel; getline(in, sentinel);
    }

private:
    string _reptileType;
};
