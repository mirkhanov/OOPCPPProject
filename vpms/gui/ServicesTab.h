#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class ServicesTab : public QWidget {
    Q_OBJECT

public:
    explicit ServicesTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onFilterChanged();
    void onAdd();
    void onDelete();

private:
    ClinicService& _svc;
    QTableWidget*  _table;
    QComboBox*     _typeFilter;
    QPushButton*   _addBtn;
    QPushButton*   _deleteBtn;

    void setupUI();
    void loadData();
};
