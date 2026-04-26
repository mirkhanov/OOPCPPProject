#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <iostream>
using namespace std;

class InventoryItem : public ISerializable {
public:
    InventoryItem() : _id(0), _quantity(0), _unitPrice(0.0) {}
    InventoryItem(int id, const string& name, int quantity,
                  double unitPrice, const string& category)
        : _id(id), _name(name), _quantity(quantity),
          _unitPrice(unitPrice), _category(category) {}

    int    getId()        const override { return _id; }
    string getName()      const { return _name; }
    int    getQuantity()  const { return _quantity; }
    double getUnitPrice() const { return _unitPrice; }
    string getCategory()  const { return _category; }

    void setName(const string& name)         { _name = name; }
    void setUnitPrice(double price)          { _unitPrice = price; }
    void setCategory(const string& category) { _category = category; }

    InventoryItem& operator+=(int qty) { _quantity += qty; return *this; }
    InventoryItem& operator-=(int qty) { _quantity -= qty; return *this; }
    bool operator<(const InventoryItem& other)  const { return _name < other._name; }
    bool operator==(const InventoryItem& other) const { return _id == other._id; }

    friend ostream& operator<<(ostream& out, const InventoryItem& item) {
        out << "[" << item._id << "] " << item._name
            << " | qty:" << item._quantity
            << " | price:" << item._unitPrice
            << " | " << item._category;
        return out;
    }

    void serialize(ostream& out) const override {
        out << _id << "\n" << _name << "\n" << _quantity << "\n"
            << _unitPrice << "\n" << _category << "\n---\n";
    }

    void deserialize(istream& in) override {
        in >> _id;
        in.ignore();
        getline(in, _name);
        in >> _quantity >> _unitPrice;
        in.ignore();
        getline(in, _category);
        string sentinel; getline(in, sentinel);
    }

private:
    int    _id;
    string _name;
    int    _quantity;
    double _unitPrice;
    string _category;
};
