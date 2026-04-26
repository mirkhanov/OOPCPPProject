#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include "../core/ClinicService.h"
#include "OwnersTab.h"
#include "AnimalsTab.h"
#include "VisitsTab.h"
#include "ServicesTab.h"
#include "InventoryTab.h"
#include "HospitalizationTab.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(ClinicService& service, QWidget* parent = nullptr);

private:
    ClinicService&      _service;
    QTabWidget*         _tabs;
    OwnersTab*          _ownersTab;
    AnimalsTab*         _animalsTab;
    VisitsTab*          _visitsTab;
    ServicesTab*        _servicesTab;
    InventoryTab*       _inventoryTab;
    HospitalizationTab* _hospitalizationTab;

    void setupUI();
};
