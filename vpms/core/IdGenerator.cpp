#include "IdGenerator.h"
#include "exceptions.h"
#include <fstream>
using namespace std;

IdGenerator::IdGenerator(const string& filePath) : _filePath(filePath) {
    load();
}

int IdGenerator::next(const string& entityName) {
    _counters[entityName]++;
    save();
    return _counters[entityName];
}

void IdGenerator::reset(const string& entityName) {
    _counters[entityName] = 0;
    save();
}

void IdGenerator::load() {
    ifstream file(_filePath);
    if (!file.is_open()) return;

    string name;
    int value;
    while (file >> name >> value) {
        _counters[name] = value;
    }
}

void IdGenerator::save() {
    ofstream file(_filePath);
    if (!file.is_open()) throw FileIOException(_filePath);

    for (const auto& pair : _counters) {
        file << pair.first << " " << pair.second << "\n";
    }
}
