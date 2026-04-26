#pragma once
#include <iostream>
using namespace std;

class ISerializable {
public:
    virtual void serialize(ostream& out) const = 0;
    virtual void deserialize(istream& in)      = 0;
    virtual int  getId() const                      = 0;
    virtual ~ISerializable() = default;
};
