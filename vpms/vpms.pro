QT += core gui widgets
CONFIG += c++17
TARGET = vpms
TEMPLATE = app

SOURCES += \
    main.cpp \
    core/IdGenerator.cpp \
    core/ClinicService.cpp \
    animals/AnimalRepository.cpp \
    animals/AnimalFactory.cpp \
    visits/ServiceRepository.cpp \
    visits/ServiceFactory.cpp \
    gui/MainWindow.cpp \
    gui/OwnersTab.cpp \
    gui/AnimalsTab.cpp \
    gui/VisitsTab.cpp \
    gui/ServicesTab.cpp \
    gui/InventoryTab.cpp \
    gui/HospitalizationTab.cpp

HEADERS += \
    core/ISerializable.h \
    core/exceptions.h \
    core/IdGenerator.h \
    core/Repository.h \
    core/ClinicService.h \
    animals/Owner.h \
    animals/Animal.h \
    animals/Dog.h \
    animals/Cat.h \
    animals/Bird.h \
    animals/Reptile.h \
    animals/AnimalRepository.h \
    animals/AnimalFactory.h \
    visits/Service.h \
    visits/Consultation.h \
    visits/Vaccination.h \
    visits/Surgery.h \
    visits/Grooming.h \
    visits/ServiceRepository.h \
    visits/ServiceFactory.h \
    visits/Visit.h \
    inventory/InventoryItem.h \
    inventory/HospitalizationRecord.h \
    gui/MainWindow.h \
    gui/OwnersTab.h \
    gui/AnimalsTab.h \
    gui/VisitsTab.h \
    gui/ServicesTab.h \
    gui/InventoryTab.h \
    gui/HospitalizationTab.h
