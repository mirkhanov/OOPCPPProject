#pragma once
#include "Service.h"
using namespace std;

class Surgery : public Service {
public:
    Surgery() : Service(), _requiresAnesthesia(false) { _typeTag = "SURGERY"; }
    Surgery(int id, const string& name, double price, const string& surgeryType, bool anesthesia)
        : Service(id, name, price, "SURGERY"), _surgeryType(surgeryType), _requiresAnesthesia(anesthesia) {}

    double getFinalPrice()  const override { return _requiresAnesthesia ? _basePrice * 1.2 : _basePrice; }
    string getDescription() const override { return "Surgery: " + _surgeryType + (_requiresAnesthesia ? " (with anesthesia)" : ""); }
    void   applyDiscount(double pct) override { if (pct > 10.0) pct = 10.0; _basePrice *= (1.0 - pct / 100.0); }

    void serialize(ostream& out) const override {
        Service::serialize(out);
        out << _surgeryType << "\n" << _requiresAnesthesia << "\n---\n";
    }
    void deserialize(istream& in) override {
        Service::deserialize(in);
        in.ignore();
        getline(in, _surgeryType);
        in >> _requiresAnesthesia;
        string s; in >> s;
    }

private:
    string _surgeryType;
    bool   _requiresAnesthesia;
};
