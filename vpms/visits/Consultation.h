#pragma once
#include "Service.h"

class Consultation : public Service {
public:
    Consultation();
    Consultation(int id, const string& name, double price, int durationMinutes);

    double getFinalPrice()  const override;
    string getDescription() const override;

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    int _durationMinutes;
};
