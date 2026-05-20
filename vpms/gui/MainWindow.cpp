#include "MainWindow.h"

MainWindow::MainWindow(ClinicService& service, QWidget* parent)
    : QMainWindow(parent), _service(service)
{
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("Veterinary Clinic Management System");
    setMinimumSize(1000, 650);

    _tabs = new QTabWidget(this);

    _clientsTab         = new ClientsTab(_service, this);
    _visitsTab          = new VisitsTab(_service, this);
    _servicesTab        = new ServicesTab(_service, this);
    _inventoryTab       = new InventoryTab(_service, this);
    _hospitalizationTab = new HospitalizationTab(_service, this);

    _tabs->addTab(_clientsTab,         "Clients");
    _tabs->addTab(_visitsTab,          "All Visits");
    _tabs->addTab(_servicesTab,        "Services");
    _tabs->addTab(_inventoryTab,       "Inventory");
    _tabs->addTab(_hospitalizationTab, "Hospitalization");

    setCentralWidget(_tabs);

    setStyleSheet(R"(
        QPushButton { padding: 4px 14px; min-width: 70px; }
        QTabBar::tab { padding: 7px 18px; }
        QTabBar::tab:selected { font-weight: bold; }
        QHeaderView::section { padding: 5px; font-weight: bold; }
    )");
}
