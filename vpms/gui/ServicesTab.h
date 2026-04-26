#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class ServicesTab : public QWidget {
    Q_OBJECT

public:
    explicit ServicesTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onAdd();
    void onDelete();

private:
    ClinicService& _service;
    QTableWidget*  _table;
    QPushButton*   _addBtn;
    QPushButton*   _deleteBtn;

    void setupUI();
    void loadData();
};
