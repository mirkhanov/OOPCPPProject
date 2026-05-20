#pragma once
#include "Service.h"

class Vaccination : public Service {
public:
    Vaccination();
    Vaccination(int id, const string& name, double price, const string& vaccineType);

    double getFinalPrice()  const override;
    string getDescription() const override;

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _vaccineType;
};
