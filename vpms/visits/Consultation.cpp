#include "Consultation.h"

Consultation::Consultation() : Service(), _durationMinutes(30) { _typeTag = "CONSULTATION"; }

Consultation::Consultation(int id, const string& name, double price, int durationMinutes)
    : Service(id, name, price, "CONSULTATION"), _durationMinutes(durationMinutes) {}

double Consultation::getFinalPrice()  const { return _basePrice; }
string Consultation::getDescription() const { return "Consultation (" + to_string(_durationMinutes) + " min)"; }

void Consultation::serialize(ostream& out) const {
    Service::serialize(out);
    out << _durationMinutes << "\n---\n";
}

void Consultation::deserialize(istream& in) {
    Service::deserialize(in);
    in >> _durationMinutes;
    string s; in.ignore(); getline(in, s);
}
