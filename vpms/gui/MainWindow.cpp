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
        QTableWidget { alternate-background-color: #f8f8f8; gridline-color: #e0e0e0; }
        QHeaderView::section { background-color: #f0f0f0; padding: 5px; border: none; border-bottom: 1px solid #ccc; font-weight: bold; }
        QPushButton { padding: 4px 14px; min-width: 70px; }
        QListWidget::item:selected { background: #0078d4; color: white; }
        QTabBar::tab { padding: 7px 18px; }
        QTabBar::tab:selected { font-weight: bold; }
    )");
}
