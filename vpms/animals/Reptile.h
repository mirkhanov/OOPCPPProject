#pragma once
#include "Animal.h"

class Reptile : public Animal {
public:
    Reptile();
    Reptile(int id, const string& name, int age, int ownerId, const string& reptileType);

    string getSpeciesInfo()         const override;
    double calculateTreatmentCost() const override;
    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _reptileType;
};
