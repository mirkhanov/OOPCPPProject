#include "MainWindow.h"

MainWindow::MainWindow(ClinicService& service, QWidget* parent)
    : QMainWindow(parent), _service(service)
{
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("Veterinary Clinic Management System");
    setMinimumSize(900, 600);

    _tabs = new QTabWidget(this);

    _ownersTab          = new OwnersTab(_service, this);
    _animalsTab         = new AnimalsTab(_service, this);
    _visitsTab          = new VisitsTab(_service, this);
    _servicesTab        = new ServicesTab(_service, this);
    _inventoryTab       = new InventoryTab(_service, this);
    _hospitalizationTab = new HospitalizationTab(_service, this);

    _tabs->addTab(_ownersTab,          "Owners");
    _tabs->addTab(_animalsTab,         "Animals");
    _tabs->addTab(_visitsTab,          "Visits");
    _tabs->addTab(_servicesTab,        "Services");
    _tabs->addTab(_inventoryTab,       "Inventory");
    _tabs->addTab(_hospitalizationTab, "Hospitalization");

    setCentralWidget(_tabs);
}
