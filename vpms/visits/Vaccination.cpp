#include "Vaccination.h"

Vaccination::Vaccination() : Service() { _typeTag = "VACCINATION"; }

Vaccination::Vaccination(int id, const string& name, double price, const string& vaccineType)
    : Service(id, name, price, "VACCINATION"), _vaccineType(vaccineType) {}

double Vaccination::getFinalPrice()  const { return _basePrice; }
string Vaccination::getDescription() const { return "Vaccination: " + _vaccineType; }

void Vaccination::serialize(ostream& out) const {
    Service::serialize(out);
    out << _vaccineType << "\n---\n";
}

void Vaccination::deserialize(istream& in) {
    Service::deserialize(in);
    in.ignore();
    getline(in, _vaccineType);
    string s; getline(in, s);
}
