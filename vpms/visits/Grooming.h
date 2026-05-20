#pragma once
#include "Service.h"

class Grooming : public Service {
public:
    Grooming();
    Grooming(int id, const string& name, double price, const string& groomingLevel);

    double getFinalPrice()  const override;
    string getDescription() const override;

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _groomingLevel;
};
