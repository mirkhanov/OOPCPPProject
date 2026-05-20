#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class HospitalizationRecord : public ISerializable {
public:
    HospitalizationRecord();
    HospitalizationRecord(int id, int animalId, const string& ward,
                          const string& admitDate, double dailyRate);

    int                getId()            const override;
    int                getAnimalId()      const;
    string             getWard()          const;
    string             getAdmitDate()     const;
    string             getDischargeDate() const;
    double             getDailyRate()     const;
    double             getTotalBill()     const;
    bool               isActive()         const;
    const vector<int>& getServiceIds()    const;

    void setDischargeDate(const string& date);
    void setTotalBill(double bill);
    void addServiceId(int serviceId);

    bool operator<(const HospitalizationRecord& other)  const;
    bool operator==(const HospitalizationRecord& other) const;
    friend ostream& operator<<(ostream& out, const HospitalizationRecord& r);

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

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
