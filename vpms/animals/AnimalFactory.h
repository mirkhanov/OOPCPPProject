#pragma once
#include "Animal.h"
#include <string>
using namespace std;

class AnimalFactory {
public:
    static Animal* create(const string& typeTag, int id,
                          const string& name, int age, int ownerId);
    static Animal* createFromStream(const string& typeTag, istream& in);
};
