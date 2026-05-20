QT += core gui widgets
CONFIG += c++17
TARGET = vpms
TEMPLATE = app

SOURCES += \
    main.cpp \
    core/IdGenerator.cpp \
    core/ClinicService.cpp \
    animals/Animal.cpp \
    animals/Owner.cpp \
    animals/Dog.cpp \
    animals/Cat.cpp \
    animals/Bird.cpp \
    animals/Reptile.cpp \
    animals/AnimalRepository.cpp \
    animals/AnimalFactory.cpp \
    visits/Service.cpp \
    visits/Consultation.cpp \
    visits/Vaccination.cpp \
    visits/Surgery.cpp \
    visits/Grooming.cpp \
    visits/Visit.cpp \
    visits/ServiceRepository.cpp \
    visits/ServiceFactory.cpp \
    inventory/InventoryItem.cpp \
    inventory/HospitalizationRecord.cpp \
    gui/MainWindow.cpp \
    gui/ClientsTab.cpp \
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
    animals/Animal.h \
    animals/Owner.h \
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
    visits/Visit.h \
    visits/ServiceRepository.h \
    visits/ServiceFactory.h \
    inventory/InventoryItem.h \
    inventory/HospitalizationRecord.h \
    gui/MainWindow.h \
    gui/ClientsTab.h \
    gui/VisitsTab.h \
    gui/ServicesTab.h \
    gui/InventoryTab.h \
    gui/HospitalizationTab.h
