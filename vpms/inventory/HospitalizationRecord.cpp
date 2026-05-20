#include "HospitalizationRecord.h"

HospitalizationRecord::HospitalizationRecord()
    : _id(0), _animalId(0), _dailyRate(0.0), _totalBill(0.0) {}

HospitalizationRecord::HospitalizationRecord(int id, int animalId, const string& ward,
                                             const string& admitDate, double dailyRate)
    : _id(id), _animalId(animalId), _ward(ward),
      _admitDate(admitDate), _dailyRate(dailyRate), _totalBill(0.0) {}

int    HospitalizationRecord::getId()            const { return _id; }
int    HospitalizationRecord::getAnimalId()      const { return _animalId; }
string HospitalizationRecord::getWard()          const { return _ward; }
string HospitalizationRecord::getAdmitDate()     const { return _admitDate; }
string HospitalizationRecord::getDischargeDate() const { return _dischargeDate; }
double HospitalizationRecord::getDailyRate()     const { return _dailyRate; }
double HospitalizationRecord::getTotalBill()     const { return _totalBill; }
bool   HospitalizationRecord::isActive()         const { return _dischargeDate.empty(); }

const vector<int>& HospitalizationRecord::getServiceIds() const { return _serviceIds; }

void HospitalizationRecord::setDischargeDate(const string& date) { _dischargeDate = date; }
void HospitalizationRecord::setTotalBill(double bill)             { _totalBill = bill; }
void HospitalizationRecord::addServiceId(int serviceId)           { _serviceIds.push_back(serviceId); }

bool HospitalizationRecord::operator<(const HospitalizationRecord& other)  const { return _admitDate < other._admitDate; }
bool HospitalizationRecord::operator==(const HospitalizationRecord& other) const { return _id == other._id; }

ostream& operator<<(ostream& out, const HospitalizationRecord& r) {
    out << "[" << r._id << "] animal:" << r._animalId
        << " ward:" << r._ward
        << " admitted:" << r._admitDate
        << " discharged:" << (r._dischargeDate.empty() ? "active" : r._dischargeDate)
        << " bill:" << r._totalBill;
    return out;
}

void HospitalizationRecord::serialize(ostream& out) const {
    out << _id << "\n" << _animalId << "\n" << _ward << "\n"
        << _admitDate << "\n" << _dischargeDate << "\n"
        << _dailyRate << "\n" << _totalBill << "\n"
        << _serviceIds.size() << "\n";
    for (int sid : _serviceIds) out << sid << "\n";
    out << "---\n";
}

void HospitalizationRecord::deserialize(istream& in) {
    in >> _id >> _animalId;
    in.ignore();
    getline(in, _ward);
    getline(in, _admitDate);
    getline(in, _dischargeDate);
    in >> _dailyRate >> _totalBill;
    int count; in >> count;
    _serviceIds.clear();
    for (int i = 0; i < count; i++) { int sid; in >> sid; _serviceIds.push_back(sid); }
    string sentinel; in >> sentinel;
}
