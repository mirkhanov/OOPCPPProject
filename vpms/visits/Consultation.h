#pragma once
#include "Service.h"
using namespace std;

class Consultation : public Service {
public:
    Consultation() : Service(), _durationMinutes(30) { _typeTag = "CONSULTATION"; }
    Consultation(int id, const string& name, double price, int duration)
        : Service(id, name, price, "CONSULTATION"), _durationMinutes(duration) {}

    double getFinalPrice()  const override { return _basePrice; }
    string getDescription() const override { return "Consultation (" + to_string(_durationMinutes) + " min)"; }

    void serialize(ostream& out) const override {
        Service::serialize(out);
        out << _durationMinutes << "\n---\n";
    }
    void deserialize(istream& in) override {
        Service::deserialize(in);
        in >> _durationMinutes;
        string s; in.ignore(); getline(in, s);
    }

private:
    int _durationMinutes;
};
