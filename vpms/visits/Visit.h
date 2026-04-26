#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Visit : public ISerializable {
public:
    Visit() : _id(0), _animalId(0), _ownerId(0), _totalCost(0.0) {}
    Visit(int id, int animalId, int ownerId, const string& date,
          const vector<int>& serviceIds, double totalCost)
        : _id(id), _animalId(animalId), _ownerId(ownerId),
          _date(date), _serviceIds(serviceIds), _totalCost(totalCost) {}

    int                getId()         const override { return _id; }
    int                getAnimalId()   const { return _animalId; }
    int                getOwnerId()    const { return _ownerId; }
    string             getDate()       const { return _date; }
    double             getTotalCost()  const { return _totalCost; }
    string             getNotes()      const { return _notes; }
    const vector<int>& getServiceIds() const { return _serviceIds; }

    void setNotes(const string& notes) { _notes = notes; }

    bool operator<(const Visit& other)  const { return _date < other._date; }
    bool operator==(const Visit& other) const { return _id == other._id; }

    friend ostream& operator<<(ostream& out, const Visit& v) {
        out << "[" << v._id << "] Animal:" << v._animalId << " Date:" << v._date << " Cost:" << v._totalCost;
        return out;
    }

    void serialize(ostream& out) const override {
        out << _id << "\n" << _animalId << "\n" << _ownerId << "\n"
            << _date << "\n" << _totalCost << "\n" << _notes << "\n"
            << _serviceIds.size() << "\n";
        for (int sid : _serviceIds) out << sid << "\n";
        out << "---\n";
    }

    void deserialize(istream& in) override {
        in >> _id >> _animalId >> _ownerId;
        in.ignore();
        getline(in, _date);
        in >> _totalCost;
        in.ignore();
        getline(in, _notes);
        int count; in >> count;
        _serviceIds.clear();
        for (int i = 0; i < count; i++) { int sid; in >> sid; _serviceIds.push_back(sid); }
        string sentinel; in >> sentinel;
    }

private:
    int         _id;
    int         _animalId;
    int         _ownerId;
    string      _date;
    vector<int> _serviceIds;
    double      _totalCost;
    string      _notes;
};
