#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../core/ClinicService.h"

class VisitsTab : public QWidget {
    Q_OBJECT

public:
    explicit VisitsTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onFilterChanged();
    void onNewVisit();
    void onCancelVisit();

private:
    ClinicService& _svc;
    QTableWidget*  _visitTable;
    QComboBox*     _ownerFilter;
    QPushButton*   _newVisitBtn;
    QPushButton*   _cancelBtn;

    void setupUI();
    void loadVisits();
    int  selectedVisitId();
};
