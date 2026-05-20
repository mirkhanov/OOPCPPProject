#include "Surgery.h"

Surgery::Surgery() : Service(), _requiresAnesthesia(false) { _typeTag = "SURGERY"; }

Surgery::Surgery(int id, const string& name, double price, const string& surgeryType, bool requiresAnesthesia)
    : Service(id, name, price, "SURGERY"), _surgeryType(surgeryType), _requiresAnesthesia(requiresAnesthesia) {}

double Surgery::getFinalPrice()  const { return _requiresAnesthesia ? _basePrice * 1.2 : _basePrice; }
string Surgery::getDescription() const { return "Surgery: " + _surgeryType + (_requiresAnesthesia ? " (with anesthesia)" : ""); }

void Surgery::applyDiscount(double pct) {
    if (pct > 10.0) pct = 10.0;
    _basePrice *= (1.0 - pct / 100.0);
}

void Surgery::serialize(ostream& out) const {
    Service::serialize(out);
    out << _surgeryType << "\n" << _requiresAnesthesia << "\n---\n";
}

void Surgery::deserialize(istream& in) {
    Service::deserialize(in);
    in.ignore();
    getline(in, _surgeryType);
    in >> _requiresAnesthesia;
    string s; in.ignore(); getline(in, s);
}
