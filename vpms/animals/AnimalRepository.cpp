#include "AnimalRepository.h"
#include "AnimalFactory.h"
#include <algorithm>
using namespace std;

AnimalRepository::AnimalRepository(const string& filePath) : _filePath(filePath) { load(); }

AnimalRepository::~AnimalRepository() {
    for (Animal* a : _items) delete a;
}

void AnimalRepository::add(Animal* item) {
    if (exists(item->getId())) throw DuplicateIdException(item->getId());
    _items.push_back(item);
    save();
}

Animal* AnimalRepository::getById(int id) {
    for (Animal* a : _items)
        if (a->getId() == id) return a;
    throw NotFoundException(id);
}

const vector<Animal*>& AnimalRepository::getAll() const { return _items; }

void AnimalRepository::update(Animal* item) {
    for (int i = 0; i < (int)_items.size(); i++) {
        if (_items[i]->getId() == item->getId()) {
            if (_items[i] != item) {
                delete _items[i];
                _items[i] = item;
            }
            save();
            return;
        }
    }
    throw NotFoundException(item->getId());
}

void AnimalRepository::remove(int id) {
    auto it = find_if(_items.begin(), _items.end(), [id](Animal* a){ return a->getId() == id; });
    if (it == _items.end()) throw NotFoundException(id);
    delete *it;
    _items.erase(it);
    save();
}

bool AnimalRepository::exists(int id) const {
    for (Animal* a : _items)
        if (a->getId() == id) return true;
    return false;
}

void AnimalRepository::load() {
    ifstream file(_filePath);
    if (!file.is_open()) return;
    while (file.peek() != EOF) {
        string typeTag;
        if (!getline(file, typeTag) || typeTag.empty()) break;
        Animal* a = AnimalFactory::createFromStream(typeTag, file);
        if (a) _items.push_back(a);
    }
}

void AnimalRepository::save() {
    ofstream file(_filePath);
    if (!file.is_open()) throw FileIOException(_filePath);
    for (Animal* a : _items) a->serialize(file);
}
