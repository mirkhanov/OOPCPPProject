#pragma once
#include "../core/ISerializable.h"
#include <string>
using namespace std;

// ЗАГЛУШКА — Студент 2 заменит этот файл своей реализацией

class Animal : public ISerializable {
public:
    Animal() : _id(0), _age(0), _ownerId(0) {}
    Animal(int id, const string& name, int age, int ownerId, const string& typeTag)
        : _id(id), _name(name), _age(age), _ownerId(ownerId), _typeTag(typeTag) {}
    virtual ~Animal() = default;

    int    getId()      const override { return _id; }
    string getName()    const { return _name; }
    int    getAge()     const { return _age; }
    int    getOwnerId() const { return _ownerId; }
    string getTypeTag() const { return _typeTag; }

    void setName(const string& name) { _name = name; }
    void setAge(int age)             { _age = age; }

    virtual string getSpeciesInfo()         const = 0;
    virtual double calculateTreatmentCost() const = 0;

    void serialize(ostream& out) const override {
        out << _typeTag << "\n" << _id << "\n" << _name << "\n" << _age << "\n" << _ownerId << "\n";
    }

    void deserialize(istream& in) override {
        in >> _typeTag >> _id;
        in.ignore();
        getline(in, _name);
        in >> _age >> _ownerId;
    }

protected:
    int    _id;
    string _name;
    int    _age;
    int    _ownerId;
    string _typeTag;
};
