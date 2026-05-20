#include "InventoryItem.h"

InventoryItem::InventoryItem() : _id(0), _quantity(0), _unitPrice(0.0) {}

InventoryItem::InventoryItem(int id, const string& name, int quantity,
                             double unitPrice, const string& category)
    : _id(id), _name(name), _quantity(quantity), _unitPrice(unitPrice), _category(category) {}

int    InventoryItem::getId()        const { return _id; }
string InventoryItem::getName()      const { return _name; }
int    InventoryItem::getQuantity()  const { return _quantity; }
double InventoryItem::getUnitPrice() const { return _unitPrice; }
string InventoryItem::getCategory()  const { return _category; }

void InventoryItem::setName(const string& name)       { _name = name; }
void InventoryItem::setUnitPrice(double price)         { _unitPrice = price; }
void InventoryItem::setCategory(const string& cat)     { _category = cat; }

InventoryItem& InventoryItem::operator+=(int qty) { _quantity += qty; return *this; }
InventoryItem& InventoryItem::operator-=(int qty) { _quantity -= qty; return *this; }

bool InventoryItem::operator<(const InventoryItem& other)  const { return _name < other._name; }
bool InventoryItem::operator==(const InventoryItem& other) const { return _id == other._id; }

ostream& operator<<(ostream& out, const InventoryItem& item) {
    out << "[" << item._id << "] " << item._name
        << " | qty:" << item._quantity
        << " | price:" << item._unitPrice
        << " | " << item._category;
    return out;
}

void InventoryItem::serialize(ostream& out) const {
    out << _id << "\n" << _name << "\n" << _quantity << "\n"
        << _unitPrice << "\n" << _category << "\n---\n";
}

void InventoryItem::deserialize(istream& in) {
    in >> _id;
    in.ignore();
    getline(in, _name);
    in >> _quantity >> _unitPrice;
    in.ignore();
    getline(in, _category);
    string sentinel; getline(in, sentinel);
}
