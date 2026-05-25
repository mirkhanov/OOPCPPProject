#include "InventoryItem.h"

InventoryItem::InventoryItem()
	: _id(0), _name(""), _quantity(0), _unitPrice(0.0), _category("") {
} // Default constructor

InventoryItem::InventoryItem(int id, const string& name, int quantity,
	double unitPrice, const string& category) // Parameterized constructor
    : _id(id), _name(name), _quantity(quantity),
      _unitPrice(unitPrice), _category(category) {}
// Getters
int    InventoryItem::getId()        const { return _id; }
string InventoryItem::getName()      const { return _name; }
int    InventoryItem::getQuantity()  const { return _quantity; }
double InventoryItem::getUnitPrice() const { return _unitPrice; }
string InventoryItem::getCategory()  const { return _category; }
// Setters
void InventoryItem::setName(const string& name)     { _name = name; }
void InventoryItem::setUnitPrice(double price)       { _unitPrice = price; }
void InventoryItem::setCategory(const string& cat)   { _category = cat; }
// Operators
InventoryItem& InventoryItem::operator+=(int qty) {
	_quantity += qty; // Adjust quantity item +=2
    return *this;
}

InventoryItem& InventoryItem::operator-=(int qty) {
	_quantity -= qty; // Adjust quantity item -=1
    return *this;
}

bool InventoryItem::operator<(const InventoryItem& other) const {
	return _name < other._name; // Sort by name
}

bool InventoryItem::operator==(const InventoryItem& other) const {
	return _id == other._id; // Compare by id (if have same id, consider same item)
}
// Output operator for easy display
ostream& operator<<(ostream& out, const InventoryItem& item) {
    out << "[" << item._id << "] " << item._name
        << " | qty: "      << item._quantity
        << " | price: "    << item._unitPrice
        << " | category: " << item._category;
    return out;
}
// Serialization format:
void InventoryItem::serialize(ostream& out) const {
    out << _id        << "\n"
        << _name      << "\n"
        << _quantity  << "\n"
        << _unitPrice << "\n"
        << _category  << "\n"
        << "---"      << "\n";
}
// Deserialization format:
void InventoryItem::deserialize(istream& in) {
    string sep;
	in >> _id >> ws; // Read id and consume any whitespace
	getline(in, _name); // Read name (until newline)
	in >> _quantity >> _unitPrice >> ws; // Read quantity and unit price, then consume any whitespace
    getline(in, _category);
    getline(in, sep); // "---"
}
