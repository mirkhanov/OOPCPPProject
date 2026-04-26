#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class HospitalizationRecord : public ISerializable {
public:
    HospitalizationRecord() : _id(0), _animalId(0), _dailyRate(0.0), _totalBill(0.0) {}
    HospitalizationRecord(int id, int animalId, const string& ward,
                          const string& admitDate, double dailyRate)
        : _id(id), _animalId(animalId), _ward(ward),
          _admitDate(admitDate), _dailyRate(dailyRate), _totalBill(0.0) {}

    int                getId()            const override { return _id; }
    int                getAnimalId()      const { return _animalId; }
    string             getWard()          const { return _ward; }
    string             getAdmitDate()     const { return _admitDate; }
    string             getDischargeDate() const { return _dischargeDate; }
    double             getDailyRate()     const { return _dailyRate; }
    double             getTotalBill()     const { return _totalBill; }
    bool               isActive()         const { return _dischargeDate.empty(); }
    const vector<int>& getServiceIds()    const { return _serviceIds; }

    void setDischargeDate(const string& date) { _dischargeDate = date; }
    void setTotalBill(double bill)            { _totalBill = bill; }
    void addServiceId(int serviceId)          { _serviceIds.push_back(serviceId); }

    bool operator<(const HospitalizationRecord& other)  const { return _admitDate < other._admitDate; }
    bool operator==(const HospitalizationRecord& other) const { return _id == other._id; }

    friend ostream& operator<<(ostream& out, const HospitalizationRecord& r) {
        out << "[" << r._id << "] animal:" << r._animalId
            << " ward:" << r._ward
            << " admitted:" << r._admitDate
            << " discharged:" << (r._dischargeDate.empty() ? "active" : r._dischargeDate)
            << " bill:" << r._totalBill;
        return out;
    }

    void serialize(ostream& out) const override {
        out << _id << "\n" << _animalId << "\n" << _ward << "\n"
            << _admitDate << "\n" << _dischargeDate << "\n"
            << _dailyRate << "\n" << _totalBill << "\n"
            << _serviceIds.size() << "\n";
        for (int sid : _serviceIds) out << sid << "\n";
        out << "---\n";
    }

    void deserialize(istream& in) override {
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

private:
    int         _id;
    int         _animalId;
    string      _ward;
    string      _admitDate;
    string      _dischargeDate;
    double      _dailyRate;
    double      _totalBill;
    vector<int> _serviceIds;
};
