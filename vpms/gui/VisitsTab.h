#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class VisitsTab : public QWidget {
    Q_OBJECT

public:
    explicit VisitsTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onAdd();
    void onCancel();

private:
    ClinicService& _service;
    QTableWidget*  _table;
    QPushButton*   _addBtn;
    QPushButton*   _cancelBtn;

    void setupUI();
    void loadData();
};
