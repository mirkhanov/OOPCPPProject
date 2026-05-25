#include "Grooming.h"

Grooming::Grooming() : Service() {}

Grooming::Grooming(int id, const string& name, double price, const string& groomingLevel)
    : Service(id, name, price, "GROOMING"), _groomingLevel(groomingLevel) {}

double Grooming::getFinalPrice() const {
    if (_groomingLevel == "Premium") return _basePrice * 1.15;
    if (_groomingLevel == "Basic") return _basePrice * 0.9;
    return _basePrice;
}

string Grooming::getDescription() const {
    return "Grooming: " + _groomingLevel;
}

void Grooming::serialize(ostream& out) const {
    Service::serialize(out);
    out << _groomingLevel << "\n---\n";
}

void Grooming::deserialize(istream& in) {
    Service::deserialize(in);
    getline(in, _groomingLevel);
    string line;
    getline(in, line);
}
