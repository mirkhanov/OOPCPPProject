#include "Visit.h"

Visit::Visit() {}

Visit::Visit(int id, int animalId, int ownerId,
             const string& date,
             const vector<int>& serviceIds,
             double totalCost)
    : _id(id), _animalId(animalId), _ownerId(ownerId),
      _date(date), _serviceIds(serviceIds), _totalCost(totalCost) {}

int Visit::getId() const {
    return _id;
}

int Visit::getAnimalId() const {
    return _animalId;
}

int Visit::getOwnerId() const {
    return _ownerId;
}

string Visit::getDate() const {
    return _date;
}

const vector<int>& Visit::getServiceIds() const {
    return _serviceIds;
}

double Visit::getTotalCost() const {
    return _totalCost;
}

string Visit::getNotes() const {
    return _notes;
}

void Visit::setNotes(const string& notes) {
    _notes = notes;
}

void Visit::serialize(ostream& out) const {
    out << _id << "\n";
    out << _animalId << "\n";
    out << _ownerId << "\n";
    out << _date << "\n";
    out << _totalCost << "\n";
    out << _notes << "\n";
    out << _serviceIds.size() << "\n";
    for (int id : _serviceIds) out << id << "\n";
    out << "---\n";
}

void Visit::deserialize(istream& in) {
    int size;
    in >> _id >> _animalId >> _ownerId;
    in.ignore();
    getline(in, _date);
    in >> _totalCost;
    in.ignore();
    getline(in, _notes);
    in >> size;

    _serviceIds.clear();
    for (int i = 0; i < size; i++) {
        int id;
        in >> id;
        _serviceIds.push_back(id);
    }
    in.ignore();

    string line;
    getline(in, line);
}

bool Visit::operator<(const Visit& other) const {
    return _date < other._date;
}

bool Visit::operator==(const Visit& other) const {
    return _id == other._id;
}

ostream& operator<<(ostream& out, const Visit& v) {
    out << "Visit #" << v._id << " " << v._date << " " << v._totalCost;
    return out;
}
