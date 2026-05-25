#include "Animal.h"

Animal::Animal() {
    _id;
    _name;
    _age;
    _ownerId;
    _typeTag;
}

Animal::Animal(int id, const string& name, int age, int ownerId, const string& typeTag) {
    _id = id;
    _name = name;
    _age = age;
    _ownerId = ownerId;
    _typeTag = typeTag;
}

void Animal::setName(const string& name) {
    _name = name;
}

void Animal::setAge(int age) {
    _age = age;
}

int Animal::getId() const {
    return _id;
}

string Animal::getName() const {
    return _name;
}

int Animal::getAge() const {
    return _age;
}

int Animal::getOwnerId() const {
    return _ownerId;
}

string Animal::getTypeTag() const {
    return _typeTag;
}

void Animal::serialize(ostream& out) const {
    out << _typeTag << endl;
    out << _id << endl;
    out << _name << endl;
    out << _age << endl;
    out << _ownerId << endl;
}

void Animal::deserialize(istream& in) {
    string line;
    // typeTag is already consumed by AnimalRepository before calling deserialize

    getline(in, line);
    _id = stoi(line);

    getline(in, _name);

    getline(in, line);
    _age = stoi(line);

    getline(in, line);
    _ownerId = stoi(line);
}
