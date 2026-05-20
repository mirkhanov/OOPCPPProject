#pragma once
#include "../core/ISerializable.h"
#include <string>
#include <iostream>
using namespace std;

class InventoryItem : public ISerializable {
public:
    InventoryItem();
    InventoryItem(int id, const string& name, int quantity,
                  double unitPrice, const string& category);

    int    getId()        const override;
    string getName()      const;
    int    getQuantity()  const;
    double getUnitPrice() const;
    string getCategory()  const;

    void setName(const string& name);
    void setUnitPrice(double price);
    void setCategory(const string& category);

    InventoryItem& operator+=(int qty);
    InventoryItem& operator-=(int qty);
    bool operator<(const InventoryItem& other)  const;
    bool operator==(const InventoryItem& other) const;
    friend ostream& operator<<(ostream& out, const InventoryItem& item);

    void serialize(ostream& out) const override;
    void deserialize(istream& in) override;

private:
    int    _id;
    string _name;
    int    _quantity;
    double _unitPrice;
    string _category;
};
