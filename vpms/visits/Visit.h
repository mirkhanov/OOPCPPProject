#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Visit : public ISerializable {
public:
    Visit();
    Visit(int id, int animalId, int ownerId, const string& date,
          const vector<int>& serviceIds, double totalCost);

    int                getId()         const override;
    int                getAnimalId()   const;
    int                getOwnerId()    const;
    string             getDate()       const;
    double             getTotalCost()  const;
    string             getNotes()      const;
    const vector<int>& getServiceIds() const;

    void setNotes(const string& notes);

    bool operator<(const Visit& other)  const;
    bool operator==(const Visit& other) const;
    friend ostream& operator<<(ostream& out, const Visit& v);

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    int         _id;
    int         _animalId;
    int         _ownerId;
    string      _date;
    vector<int> _serviceIds;
    double      _totalCost;
    string      _notes;
};
