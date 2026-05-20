#pragma once
#include "../core/ISerializable.h"
#include <string>
using namespace std;

class Animal : public ISerializable {
public:
    Animal();
    Animal(int id, const string& name, int age, int ownerId, const string& typeTag);
    virtual ~Animal() = default;

    int    getId()      const override;
    string getName()    const;
    int    getAge()     const;
    int    getOwnerId() const;
    string getTypeTag() const;

    void setName(const string& name);
    void setAge(int age);

    virtual string getSpeciesInfo()         const = 0;
    virtual double calculateTreatmentCost() const = 0;

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

protected:
    int    _id;
    string _name;
    int    _age;
    int    _ownerId;
    string _typeTag;
};
