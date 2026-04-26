#pragma once
#include <map>
#include <string>
using namespace std;

class IdGenerator {
public:
    explicit IdGenerator(const string& filePath);
    int  next(const string& entityName);
    void reset(const string& entityName);

private:
    string              _filePath;
    map<string,int> _counters;

    void load();
    void save();
};
