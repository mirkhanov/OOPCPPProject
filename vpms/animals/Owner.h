#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <iostream>
using namespace std;

class Owner : public ISerializable {
public:
    Owner();
    Owner(int id, const string& name, const string& contactInfo);

    int    getId()          const override;
    string getName()        const;
    string getContactInfo() const;

    void setName(const string& name);
    void setContactInfo(const string& contactInfo);

    bool operator==(const Owner& other) const;
    friend ostream& operator<<(ostream& out, const Owner& o);

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    int    _id;
    string _name;
    string _contactInfo;
};
