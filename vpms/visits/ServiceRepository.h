#pragma once
#include "Service.h"
#include "../core/exceptions.h"
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class ServiceRepository {
public:
    explicit ServiceRepository(const string& filePath);
    ~ServiceRepository();

    void                     add(Service* item);
    Service*                 getById(int id);
    const vector<Service*>&  getAll() const;
    void                     update(Service* item);
    void                     remove(int id);
    bool                     exists(int id) const;

private:
    vector<Service*> _items;
    string           _filePath;

    void load();
    void save();
};
