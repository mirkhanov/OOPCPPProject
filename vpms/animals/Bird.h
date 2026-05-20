#pragma once
#include "Animal.h"

class Bird : public Animal {
public:
    Bird();
    Bird(int id, const string& name, int age, int ownerId, const string& species);

    string getSpeciesInfo()         const override;
    double calculateTreatmentCost() const override;
    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _species;
};
