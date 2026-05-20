#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
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
    ClinicService& _svc;
    QTableWidget*  _table;
    QLabel*        _totalLabel;
    QPushButton*   _addBtn;
    QPushButton*   _stockInBtn;
    QPushButton*   _stockOutBtn;
    QPushButton*   _deleteBtn;

    void setupUI();
    void loadItems();
};
