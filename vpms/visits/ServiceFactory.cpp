#include "ServiceFactory.h"
#include "Consultation.h"
#include "Vaccination.h"
#include "Surgery.h"
#include "Grooming.h"
#include "../core/exceptions.h"
using namespace std;

Service* ServiceFactory::create(const string& typeTag, int id,
                                 const string& name, double price,
                                 const string& extra) {
    if (typeTag == "CONSULTATION") {
        int dur = extra.empty() ? 30 : stoi(extra);
        return new Consultation(id, name, price, dur);
    }
    if (typeTag == "VACCINATION")
        return new Vaccination(id, name, price, extra.empty() ? "General" : extra);
    if (typeTag == "SURGERY") {
        size_t pos = extra.find('|');
        string type = (pos != string::npos) ? extra.substr(0, pos) : extra;
        bool anesthesia = (pos != string::npos) && (extra.substr(pos + 1) == "1");
        return new Surgery(id, name, price, type.empty() ? "General" : type, anesthesia);
    }
    if (typeTag == "GROOMING")
        return new Grooming(id, name, price, extra.empty() ? "Standard" : extra);
    throw ValidationException("Unknown service type: " + typeTag);
}

Service* ServiceFactory::createFromStream(const string& typeTag, istream& in) {
    Service* s = nullptr;
    if      (typeTag == "CONSULTATION") s = new Consultation();
    else if (typeTag == "VACCINATION")  s = new Vaccination();
    else if (typeTag == "SURGERY")      s = new Surgery();
    else if (typeTag == "GROOMING")     s = new Grooming();
    else throw ValidationException("Unknown service type: " + typeTag);
    s->deserialize(in);
    return s;
}
