#pragma once
#include "Animal.h"
using namespace std;

// ЗАГЛУШКА — Студент 2 заменит этот файл своей реализацией

class Dog : public Animal {
public:
    Dog() : Animal() { _typeTag = "DOG"; }
    Dog(int id, const string& name, int age, int ownerId, const string& breed)
        : Animal(id, name, age, ownerId, "DOG"), _breed(breed) {}

    string getSpeciesInfo()         const override { return "Dog - Breed: " + _breed; }
    double calculateTreatmentCost() const override { return 80.0; }

    void serialize(ostream& out) const override {
        Animal::serialize(out);
        out << _breed << "\n---\n";
    }

    void deserialize(istream& in) override {
        Animal::deserialize(in);
        in.ignore();
        getline(in, _breed);
        string sentinel; getline(in, sentinel);
    }

private:
    string _breed;
};
