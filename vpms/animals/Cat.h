#pragma once
#include "Animal.h"
using namespace std;

class Cat : public Animal {
public:
    Cat() : Animal() { _typeTag = "CAT"; }
    Cat(int id, const string& name, int age, int ownerId, const string& furType)
        : Animal(id, name, age, ownerId, "CAT"), _furType(furType) {}

    string getSpeciesInfo()         const override { return "Cat - Fur: " + _furType; }
    double calculateTreatmentCost() const override { return 60.0; }

    void serialize(ostream& out) const override {
        Animal::serialize(out);
        out << _furType << "\n---\n";
    }

    void deserialize(istream& in) override {
        Animal::deserialize(in);
        in.ignore();
        getline(in, _furType);
        string sentinel; getline(in, sentinel);
    }

private:
    string _furType;
};
