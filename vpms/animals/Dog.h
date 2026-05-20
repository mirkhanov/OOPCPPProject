#pragma once
#include "Animal.h"

class Dog : public Animal {
public:
    Dog();
    Dog(int id, const string& name, int age, int ownerId, const string& breed);

    string getSpeciesInfo()         const override;
    double calculateTreatmentCost() const override;
    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _breed;
};
