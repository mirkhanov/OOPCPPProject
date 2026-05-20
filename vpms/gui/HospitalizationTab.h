#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "../core/ClinicService.h"

class HospitalizationTab : public QWidget {
    Q_OBJECT

public:
    explicit HospitalizationTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onAdmit();
    void onDischarge();
    void onActiveSelectionChanged();

private:
    ClinicService& _svc;
    QTableWidget*  _activeTable;
    QTableWidget*  _dischargedTable;
    QPushButton*   _admitBtn;
    QPushButton*   _dischargeBtn;

    void setupUI();
    void loadTables();
};
