#pragma once
#include "Service.h"
#include <string>
using namespace std;

class ServiceFactory {
public:
    static Service* create(const string& typeTag, int id,
                           const string& name, double price,
                           const string& extra = "");
    static Service* createFromStream(const string& typeTag, istream& in);
};
