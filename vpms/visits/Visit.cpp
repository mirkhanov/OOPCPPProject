#include "Visit.h"

Visit::Visit() : _id(0), _animalId(0), _ownerId(0), _totalCost(0.0) {}

Visit::Visit(int id, int animalId, int ownerId, const string& date,
             const vector<int>& serviceIds, double totalCost)
    : _id(id), _animalId(animalId), _ownerId(ownerId),
      _date(date), _serviceIds(serviceIds), _totalCost(totalCost) {}

int                Visit::getId()         const { return _id; }
int                Visit::getAnimalId()   const { return _animalId; }
int                Visit::getOwnerId()    const { return _ownerId; }
string             Visit::getDate()       const { return _date; }
double             Visit::getTotalCost()  const { return _totalCost; }
string             Visit::getNotes()      const { return _notes; }
const vector<int>& Visit::getServiceIds() const { return _serviceIds; }

void Visit::setNotes(const string& notes) { _notes = notes; }

bool Visit::operator<(const Visit& other)  const { return _date < other._date; }
bool Visit::operator==(const Visit& other) const { return _id == other._id; }

ostream& operator<<(ostream& out, const Visit& v) {
    out << "[" << v._id << "] Animal:" << v._animalId << " Date:" << v._date << " Cost:" << v._totalCost;
    return out;
}

void Visit::serialize(ostream& out) const {
    out << _id << "\n" << _animalId << "\n" << _ownerId << "\n"
        << _date << "\n" << _totalCost << "\n" << _notes << "\n"
        << _serviceIds.size() << "\n";
    for (int sid : _serviceIds) out << sid << "\n";
    out << "---\n";
}

void Visit::deserialize(istream& in) {
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
