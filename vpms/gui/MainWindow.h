#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include "../core/ClinicService.h"
#include "ClientsTab.h"
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
    ClientsTab*         _clientsTab;
    VisitsTab*          _visitsTab;
    ServicesTab*        _servicesTab;
    InventoryTab*       _inventoryTab;
    HospitalizationTab* _hospitalizationTab;

    void setupUI();
};
