#include "ServiceRepository.h"
#include "ServiceFactory.h"
#include <algorithm>
using namespace std;

ServiceRepository::ServiceRepository(const string& filePath) : _filePath(filePath) { load(); }

ServiceRepository::~ServiceRepository() {
    for (Service* s : _items) delete s;
}

void ServiceRepository::add(Service* item) {
    if (exists(item->getId())) throw DuplicateIdException(item->getId());
    _items.push_back(item);
    save();
}

Service* ServiceRepository::getById(int id) {
    for (Service* s : _items)
        if (s->getId() == id) return s;
    throw NotFoundException(id);
}

const vector<Service*>& ServiceRepository::getAll() const { return _items; }

void ServiceRepository::update(Service* item) {
    for (int i = 0; i < (int)_items.size(); i++) {
        if (_items[i]->getId() == item->getId()) {
            delete _items[i];
            _items[i] = item;
            save();
            return;
        }
    }
    throw NotFoundException(item->getId());
}

void ServiceRepository::remove(int id) {
    auto it = find_if(_items.begin(), _items.end(), [id](Service* s){ return s->getId() == id; });
    if (it == _items.end()) throw NotFoundException(id);
    delete *it;
    _items.erase(it);
    save();
}

bool ServiceRepository::exists(int id) const {
    for (Service* s : _items)
        if (s->getId() == id) return true;
    return false;
}

void ServiceRepository::load() {
    ifstream file(_filePath);
    if (!file.is_open()) return;
    while (file.peek() != EOF) {
        string typeTag;
        if (!getline(file, typeTag) || typeTag.empty()) break;
        Service* s = ServiceFactory::createFromStream(typeTag, file);
        if (s) _items.push_back(s);
    }
}

void ServiceRepository::save() {
    ofstream file(_filePath);
    if (!file.is_open()) throw FileIOException(_filePath);
    for (Service* s : _items) s->serialize(file);
}
