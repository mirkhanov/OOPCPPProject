#include "Owner.h"

Owner::Owner() : _id(0) {}

Owner::Owner(int id, const string& name, const string& contactInfo)
    : _id(id), _name(name), _contactInfo(contactInfo) {}

int    Owner::getId()          const { return _id; }
string Owner::getName()        const { return _name; }
string Owner::getContactInfo() const { return _contactInfo; }

void Owner::setName(const string& name)               { _name = name; }
void Owner::setContactInfo(const string& contactInfo) { _contactInfo = contactInfo; }

bool Owner::operator==(const Owner& other) const { return _id == other._id; }

ostream& operator<<(ostream& out, const Owner& o) {
    out << "[" << o._id << "] " << o._name << " | " << o._contactInfo;
    return out;
}

void Owner::serialize(ostream& out) const {
    out << _id << "\n" << _name << "\n" << _contactInfo << "\n---\n";
}

void Owner::deserialize(istream& in) {
    in >> _id;
    in.ignore();
    getline(in, _name);
    getline(in, _contactInfo);
    string sentinel; getline(in, sentinel);
}
