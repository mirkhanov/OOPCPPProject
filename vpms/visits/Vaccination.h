#pragma once
#include "Service.h"
using namespace std;

class Vaccination : public Service {
public:
    Vaccination() : Service() { _typeTag = "VACCINATION"; }
    Vaccination(int id, const string& name, double price, const string& vaccineType)
        : Service(id, name, price, "VACCINATION"), _vaccineType(vaccineType) {}

    double getFinalPrice()  const override { return _basePrice; }
    string getDescription() const override { return "Vaccination: " + _vaccineType; }

    void serialize(ostream& out) const override {
        Service::serialize(out);
        out << _vaccineType << "\n---\n";
    }
    void deserialize(istream& in) override {
        Service::deserialize(in);
        in.ignore();
        getline(in, _vaccineType);
        string s; getline(in, s);
    }

private:
    string _vaccineType;
};
