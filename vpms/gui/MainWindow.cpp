#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(ClinicService& service, QWidget* parent)
    : QMainWindow(parent), _service(service) {
    setupUI();
}

void MainWindow::setupUI(){
    setWindowTitle("Veterinary Clinic Management System");
    setMinimumSize(900, 600);

    _tabs = new QTabWidget(this);
    setCentralWidget(_tabs);
}