#pragma once
#include "../core/ISerializable.h"
#include <string>
using namespace std;

class Service : public ISerializable {
public:
    Service();
    Service(int id, const string& name, double basePrice, const string& typeTag);
    virtual ~Service() = default;

    int    getId()        const override;
    string getName()      const;
    double getBasePrice() const;
    string getTypeTag()   const;

    virtual double getFinalPrice()  const = 0;
    virtual string getDescription() const = 0;
    virtual void   applyDiscount(double pct);

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

protected:
    int    _id;
    string _name;
    double _basePrice;
    string _typeTag;
};
