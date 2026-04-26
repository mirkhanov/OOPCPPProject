#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class InventoryTab : public QWidget {
    Q_OBJECT

public:
    explicit InventoryTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onAdd();
    void onStockIn();
    void onStockOut();
    void onDelete();

private:
    ClinicService& _service;
    QTableWidget*  _table;
    QPushButton*   _addBtn;
    QPushButton*   _stockInBtn;
    QPushButton*   _stockOutBtn;
    QPushButton*   _deleteBtn;

    void setupUI();
    void loadData();
};
