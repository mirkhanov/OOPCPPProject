#pragma once
#include <stdexcept>
#include <string>
using namespace std;

class VPMSException : public exception {
protected:
    string _message;
public:
    explicit VPMSException(const string& message) : _message(message) {}
    const char* what() const noexcept override { return _message.c_str(); }
};

class NotFoundException : public VPMSException {
public:
    explicit NotFoundException(int id)
        : VPMSException("Record with ID " + to_string(id) + " not found.") {}
};

class DuplicateIdException : public VPMSException {
public:
    explicit DuplicateIdException(int id)
        : VPMSException("Record with ID " + to_string(id) + " already exists.") {}
};

class FileIOException : public VPMSException {
public:
    explicit FileIOException(const string& filePath)
        : VPMSException("File I/O error: " + filePath) {}
};

class ValidationException : public VPMSException {
public:
    explicit ValidationException(const string& reason)
        : VPMSException("Validation error: " + reason) {}
};

class InsufficientStockException : public VPMSException {
public:
    InsufficientStockException(const string& itemName, int available, int requested)
        : VPMSException("Insufficient stock for '" + itemName +
                        "': available=" + to_string(available) +
                        ", requested=" + to_string(requested)) {}
};
