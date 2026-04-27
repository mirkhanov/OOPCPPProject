#pragma once
#include "../core/ISerializable.h"
#include <string>
using namespace std;

class Service : public ISerializable {
public:
    Service() : _id(0), _basePrice(0.0) {}
    Service(int id, const string& name, double basePrice, const string& typeTag)
        : _id(id), _name(name), _basePrice(basePrice), _typeTag(typeTag) {}
    virtual ~Service() = default;

    int    getId()        const override { return _id; }
    string getName()      const { return _name; }
    double getBasePrice() const { return _basePrice; }
    string getTypeTag()   const { return _typeTag; }

    virtual double getFinalPrice()  const = 0;
    virtual string getDescription() const = 0;
    virtual void   applyDiscount(double pct) { _basePrice *= (1.0 - pct / 100.0); }

    void serialize(ostream& out) const override {
        out << _typeTag << "\n" << _id << "\n" << _name << "\n" << _basePrice << "\n";
    }

    void deserialize(istream& in) override {
        // typeTag already consumed by ServiceRepository::load
        in >> _id;
        in.ignore();
        getline(in, _name);
        in >> _basePrice;
    }

protected:
    int    _id;
    string _name;
    double _basePrice;
    string _typeTag;
};
