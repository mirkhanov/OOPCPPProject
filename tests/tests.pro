QT += testlib core
QT -= gui
CONFIG += c++17 console
CONFIG -= app_bundle
TEMPLATE = app
TARGET = vpms_tests

INCLUDEPATH += ../vpms

SOURCES += \
    tests.cpp \
    ../vpms/animals/Animal.cpp \
    ../vpms/animals/Owner.cpp \
    ../vpms/animals/Dog.cpp \
    ../vpms/animals/Cat.cpp \
    ../vpms/animals/Bird.cpp \
    ../vpms/animals/Reptile.cpp \
    ../vpms/animals/AnimalRepository.cpp \
    ../vpms/animals/AnimalFactory.cpp \
    ../vpms/visits/Service.cpp \
    ../vpms/visits/Consultation.cpp \
    ../vpms/visits/Vaccination.cpp \
    ../vpms/visits/Surgery.cpp \
    ../vpms/visits/Grooming.cpp \
    ../vpms/visits/Visit.cpp \
    ../vpms/visits/ServiceRepository.cpp \
    ../vpms/visits/ServiceFactory.cpp \
    ../vpms/inventory/InventoryItem.cpp \
    ../vpms/inventory/HospitalizationRecord.cpp \
    ../vpms/core/IdGenerator.cpp

HEADERS += \
    ../vpms/core/ISerializable.h \
    ../vpms/core/exceptions.h \
    ../vpms/core/IdGenerator.h \
    ../vpms/core/Repository.h \
    ../vpms/animals/Animal.h \
    ../vpms/animals/Owner.h \
    ../vpms/animals/Dog.h \
    ../vpms/animals/Cat.h \
    ../vpms/animals/Bird.h \
    ../vpms/animals/Reptile.h \
    ../vpms/animals/AnimalRepository.h \
    ../vpms/animals/AnimalFactory.h \
    ../vpms/visits/Service.h \
    ../vpms/visits/Consultation.h \
    ../vpms/visits/Vaccination.h \
    ../vpms/visits/Surgery.h \
    ../vpms/visits/Grooming.h \
    ../vpms/visits/Visit.h \
    ../vpms/visits/ServiceRepository.h \
    ../vpms/visits/ServiceFactory.h \
    ../vpms/inventory/InventoryItem.h \
    ../vpms/inventory/HospitalizationRecord.h
