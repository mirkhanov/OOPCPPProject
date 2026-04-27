#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include "../core/ClinicService.h"

class ClientsTab : public QWidget {
    Q_OBJECT

public:
    explicit ClientsTab(ClinicService& service, QWidget* parent = nullptr);

private slots:
    void onOwnerSelected();
    void onOwnerAdd();
    void onOwnerEdit();
    void onOwnerDelete();
    void onAnimalAdd();
    void onAnimalEdit();
    void onAnimalDelete();
    void onVisitAdd();
    void onVisitCancel();

private:
    ClinicService& _service;

    // Left: owners
    QTableWidget* _ownerTable;
    QPushButton*  _ownerAddBtn;
    QPushButton*  _ownerEditBtn;
    QPushButton*  _ownerDeleteBtn;

    // Right: detail tabs
    QTabWidget*   _detailTabs;
    QLabel*       _detailHeader;

    // Animals tab
    QTableWidget* _animalTable;
    QPushButton*  _animalAddBtn;
    QPushButton*  _animalEditBtn;
    QPushButton*  _animalDeleteBtn;

    // Visits tab
    QTableWidget* _visitTable;
    QPushButton*  _visitAddBtn;
    QPushButton*  _visitCancelBtn;

    void setupUI();
    void loadOwners();
    void refreshDetail(int ownerId);
    void loadAnimals(int ownerId);
    void loadVisits(int ownerId);
    int  selectedOwnerId() const;
    int  selectedAnimalId() const;
    int  selectedVisitId() const;
};
