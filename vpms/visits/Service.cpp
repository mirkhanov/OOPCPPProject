#include "Service.h"

Service::Service() : _id(0), _basePrice(0.0) {}

Service::Service(int id, const string& name, double basePrice, const string& typeTag)
    : _id(id), _name(name), _basePrice(basePrice), _typeTag(typeTag) {}

int    Service::getId()        const { return _id; }
string Service::getName()      const { return _name; }
double Service::getBasePrice() const { return _basePrice; }
string Service::getTypeTag()   const { return _typeTag; }

void Service::applyDiscount(double pct) { _basePrice *= (1.0 - pct / 100.0); }

void Service::serialize(ostream& out) const {
    out << _typeTag << "\n" << _id << "\n" << _name << "\n" << _basePrice << "\n";
}

void Service::deserialize(istream& in) {
    // typeTag already consumed by ServiceRepository::load
    in >> _id;
    in.ignore();
    getline(in, _name);
    in >> _basePrice;
}
