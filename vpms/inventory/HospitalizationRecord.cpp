#include "HospitalizationRecord.h"
// Default constructor
HospitalizationRecord::HospitalizationRecord()
    : _id(0), _animalId(0), _ward(""), _admitDate(""),
      _dischargeDate(""), _dailyRate(0.0), _totalBill(0.0) {}
// Parameterized constructor
HospitalizationRecord::HospitalizationRecord(int id, int animalId,
                                             const string& ward,
                                             const string& admitDate,
                                             double dailyRate)
    : _id(id), _animalId(animalId), _ward(ward),
      _admitDate(admitDate), _dischargeDate(""),
      _dailyRate(dailyRate), _totalBill(0.0) {}
// Getters
int    HospitalizationRecord::getId()            const { return _id; }
int    HospitalizationRecord::getAnimalId()      const { return _animalId; }
string HospitalizationRecord::getWard()          const { return _ward; }
string HospitalizationRecord::getAdmitDate()     const { return _admitDate; }
string HospitalizationRecord::getDischargeDate() const { return _dischargeDate; }
double HospitalizationRecord::getDailyRate()     const { return _dailyRate; }
double HospitalizationRecord::getTotalBill()     const { return _totalBill; }
bool   HospitalizationRecord::isActive()         const { return _dischargeDate.empty(); } // Active if no discharge date
// Return a const reference to the vector of service IDs to avoid unnecessary copying
const vector<int>& HospitalizationRecord::getServiceIds() const {
    return _serviceIds;
}
// Setters
void HospitalizationRecord::setDischargeDate(const string& date) {
    _dischargeDate = date;
}

void HospitalizationRecord::setTotalBill(double bill) {
    _totalBill = bill;
}

void HospitalizationRecord::addServiceId(int serviceId) {
	_serviceIds.push_back(serviceId); // Add a service ID to the list of services used during hospitalization
}
// Operators
bool HospitalizationRecord::operator<(const HospitalizationRecord& other) const {
    return _admitDate < other._admitDate; // Sort by admit date
}

bool HospitalizationRecord::operator==(const HospitalizationRecord& other) const {
    return _id == other._id;
}
// Output operator for easy display
ostream& operator<<(ostream& out, const HospitalizationRecord& r) {
    out << "[" << r._id << "] "
        << "animalId="  << r._animalId
        << " | ward="   << r._ward
        << " | admitted=" << r._admitDate
        << " | discharged=" << (r._dischargeDate.empty() ? "active" : r._dischargeDate)
        << " | bill="   << r._totalBill;
    return out;
}
// Serialization format:
void HospitalizationRecord::serialize(ostream& out) const {
    out << _id            << "\n"
        << _animalId      << "\n"
        << _ward          << "\n"
        << _admitDate     << "\n"
        << _dischargeDate << "\n"
        << _dailyRate     << "\n"
        << _totalBill     << "\n"
        << _serviceIds.size() << "\n";
    for (int sid : _serviceIds) {
        out << sid << "\n";
    }
    out << "---" << "\n";
}
// Deserialization format:
void HospitalizationRecord::deserialize(istream& in) {
    string sep;
    in >> _id >> _animalId >> ws;
    getline(in, _ward);
    getline(in, _admitDate);
	getline(in, _dischargeDate);  // Read discharge date (can be empty if active)
    in >> _dailyRate >> _totalBill;

    int count = 0;
    in >> count >> ws;
    _serviceIds.clear();
    for (int i = 0; i < count; i++) {
        int sid;
        in >> sid >> ws;
        _serviceIds.push_back(sid);
    }
    getline(in, sep); // "---"
}
