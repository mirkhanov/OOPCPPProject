#include "Animal.h"

Animal::Animal() : _id(0), _age(0), _ownerId(0) {}

Animal::Animal(int id, const string& name, int age, int ownerId, const string& typeTag)
    : _id(id), _name(name), _age(age), _ownerId(ownerId), _typeTag(typeTag) {}

int    Animal::getId()      const { return _id; }
string Animal::getName()    const { return _name; }
int    Animal::getAge()     const { return _age; }
int    Animal::getOwnerId() const { return _ownerId; }
string Animal::getTypeTag() const { return _typeTag; }

void Animal::setName(const string& name) { _name = name; }
void Animal::setAge(int age)             { _age = age; }

void Animal::serialize(ostream& out) const {
    out << _typeTag << "\n" << _id << "\n" << _name << "\n" << _age << "\n" << _ownerId << "\n";
}

void Animal::deserialize(istream& in) {
    // typeTag already consumed by AnimalRepository::load
    in >> _id;
    in.ignore();
    getline(in, _name);
    in >> _age >> _ownerId;
    in.ignore(); // consume newline so subclass can use getline for its extra field
}
