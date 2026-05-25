#include "Service.h"

Service::Service() {}

Service::Service(int id, const string& name, double basePrice, const string& typeTag)
    : _id(id), _name(name), _basePrice(basePrice), _typeTag(typeTag) {}

int Service::getId() const {
    return _id;
}

string Service::getName() const {
    return _name;
}

double Service::getBasePrice() const {
    return _basePrice;
}

string Service::getTypeTag() const {
    return _typeTag;
}

void Service::applyDiscount(double pct) {
    _basePrice *= (1.0 - pct / 100.0);
}

void Service::serialize(ostream& out) const {
    out << _typeTag << "\n";
    out << _id << "\n";
    out << _name << "\n";
    out << _basePrice << "\n";
}

void Service::deserialize(istream& in) {
    in >> _id;
    in.ignore();
    getline(in, _name);
    in >> _basePrice;
    in.ignore();
}
