#pragma once
#include "Service.h"
using namespace std;

class Grooming : public Service {
public:
    Grooming() : Service() { _typeTag = "GROOMING"; }
    Grooming(int id, const string& name, double price, const string& level)
        : Service(id, name, price, "GROOMING"), _groomingLevel(level) {}

    double getFinalPrice() const override {
        if (_groomingLevel == "Premium") return _basePrice * 1.15;
        if (_groomingLevel == "Basic")   return _basePrice * 0.90;
        return _basePrice;
    }
    string getDescription() const override { return "Grooming: " + _groomingLevel; }

    void serialize(ostream& out) const override {
        Service::serialize(out);
        out << _groomingLevel << "\n---\n";
    }
    void deserialize(istream& in) override {
        Service::deserialize(in);
        in.ignore();
        getline(in, _groomingLevel);
        string s; getline(in, s);
    }

private:
    string _groomingLevel;
};
