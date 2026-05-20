#pragma once
#include "Animal.h"

class Cat : public Animal {
public:
    Cat();
    Cat(int id, const string& name, int age, int ownerId, const string& furType);

    string getSpeciesInfo()         const override;
    double calculateTreatmentCost() const override;
    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _furType;
};
