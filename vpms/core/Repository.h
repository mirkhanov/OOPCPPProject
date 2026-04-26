#pragma once
#include "ISerializable.h"
#include "exceptions.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

template <typename T>
class Repository {
public:
    explicit Repository(const string& filePath) : _filePath(filePath) {
        load();
    }

    void add(const T& item) {
        if (exists(item.getId()))
            throw DuplicateIdException(item.getId());
        _items.push_back(item);
        save();
    }

    T& getById(int id) {
        for (T& item : _items) {
            if (item.getId() == id) return item;
        }
        throw NotFoundException(id);
    }

    const vector<T>& getAll() const {
        return _items;
    }

    void update(const T& item) {
        for (int i = 0; i < (int)_items.size(); i++) {
            if (_items[i].getId() == item.getId()) {
                _items[i] = item;
                save();
                return;
            }
        }
        throw NotFoundException(item.getId());
    }

    void remove(int id) {
        auto it = find_if(_items.begin(), _items.end(),
            [id](const T& item) { return item.getId() == id; });

        if (it == _items.end()) throw NotFoundException(id);
        _items.erase(it);
        save();
    }

    bool exists(int id) const {
        for (const T& item : _items) {
            if (item.getId() == id) return true;
        }
        return false;
    }

    int count() const {
        return (int)_items.size();
    }

    void clear() {
        _items.clear();
        save();
    }

protected:
    vector<T> _items;
    string    _filePath;

    virtual void load() {
        ifstream file(_filePath);
        if (!file.is_open()) return;

        while (file.peek() != EOF) {
            T item;
            item.deserialize(file);
            if (file.good() || file.eof()) {
                _items.push_back(item);
            }
        }
    }

    virtual void save() {
        ofstream file(_filePath);
        if (!file.is_open()) throw FileIOException(_filePath);

        for (const T& item : _items) {
            item.serialize(file);
        }
    }
};
