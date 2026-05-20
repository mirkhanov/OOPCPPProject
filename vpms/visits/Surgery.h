#pragma once
#include "Service.h"

class Surgery : public Service {
public:
    Surgery();
    Surgery(int id, const string& name, double price, const string& surgeryType, bool requiresAnesthesia);

    double getFinalPrice()  const override;
    string getDescription() const override;
    void   applyDiscount(double pct) override;

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    string _surgeryType;
    bool   _requiresAnesthesia;
};
