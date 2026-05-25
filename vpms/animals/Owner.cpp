#include "Owner.h"

Owner::Owner() {
    _id;
    _name;
    _contactInfo;
}

Owner::Owner(int id, const string& name, const string& contactInfo) {
    _id = id;
    _name = name;
    _contactInfo = contactInfo;
}

void Owner::setName(const string& name) {
    _name = name;
}

void Owner::setContactInfo(const string& contactInfo) {
    _contactInfo = contactInfo;
}

int Owner::getId() const {
    return _id;
}

string Owner::getName() const {
    return _name;
}

string Owner::getContactInfo() const {
    return _contactInfo;
}

void Owner::serialize(ostream& out) const {
    out << _id << endl;
    out << _name << endl;
    out << _contactInfo << endl;
    out << "---" << endl;
}

void Owner::deserialize(istream& in) {
    string line;

    getline(in, line);

    _id = stoi(line);

    getline(in, _name);

    getline(in, _contactInfo);

    getline(in, line);
}

bool Owner::operator==(const Owner& other) const {
    return _id == other._id;
}

ostream& operator<<(ostream& out, const Owner& owner) {
    out << "Owner: " << owner._name
        << " | Contact: " << owner._contactInfo;

    return out;
}
