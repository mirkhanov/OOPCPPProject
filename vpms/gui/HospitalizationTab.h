#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class HospitalizationTab : public QWidget {
    Q_OBJECT

public:
    explicit HospitalizationTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onAdmit();
    void onDischarge();
    void onAddService();

private:
    ClinicService& _service;
    QTableWidget*  _table;
    QPushButton*   _admitBtn;
    QPushButton*   _dischargeBtn;
    QPushButton*   _addServiceBtn;

    void setupUI();
    void loadData();
};
