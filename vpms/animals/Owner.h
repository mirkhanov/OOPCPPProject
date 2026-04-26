#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <iostream>
using namespace std;

// ЗАГЛУШКА — Студент 2 заменит этот файл своей реализацией

class Owner : public ISerializable {
public:
    Owner() : _id(0) {}
    Owner(int id, const string& name, const string& contactInfo)
        : _id(id), _name(name), _contactInfo(contactInfo) {}

    int    getId()          const override { return _id; }
    string getName()        const { return _name; }
    string getContactInfo() const { return _contactInfo; }

    void setName(const string& name)               { _name = name; }
    void setContactInfo(const string& contactInfo) { _contactInfo = contactInfo; }

    bool operator==(const Owner& other) const { return _id == other._id; }

    friend ostream& operator<<(ostream& out, const Owner& o) {
        out << "[" << o._id << "] " << o._name << " | " << o._contactInfo;
        return out;
    }

    void serialize(ostream& out) const override {
        out << _id << "\n" << _name << "\n" << _contactInfo << "\n---\n";
    }

    void deserialize(istream& in) override {
        string sentinel;
        in >> _id;
        in.ignore();
        getline(in, _name);
        getline(in, _contactInfo);
        getline(in, sentinel);
    }

private:
    int    _id;
    string _name;
    string _contactInfo;
};
