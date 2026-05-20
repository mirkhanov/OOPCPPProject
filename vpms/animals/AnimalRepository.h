#pragma once
#include "Animal.h"
#include "../core/exceptions.h"
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class AnimalRepository {
public:
    explicit AnimalRepository(const string& filePath);
    ~AnimalRepository();

    void                    add(Animal* item);
    Animal*                 getById(int id);
    const vector<Animal*>&  getAll() const;
    void                    update(Animal* item);
    void                    remove(int id);
    bool                    exists(int id) const;

private:
    vector<Animal*> _items;
    string          _filePath;

    void load();
    void save();
};
