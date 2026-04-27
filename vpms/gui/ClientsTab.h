#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
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

    // Left panel
    QTableWidget* _ownerTable;
    QPushButton*  _ownerAddBtn;
    QPushButton*  _ownerEditBtn;
    QPushButton*  _ownerDeleteBtn;

    // Right: unified detail
    QLabel*       _nameLabel;
    QLabel*       _contactLabel;

    QTableWidget* _animalTable;
    QPushButton*  _animalAddBtn;
    QPushButton*  _animalEditBtn;
    QPushButton*  _animalDeleteBtn;

    QTableWidget* _visitTable;
    QPushButton*  _visitAddBtn;
    QPushButton*  _visitCancelBtn;

    QWidget*      _detailWidget;  // shown/hidden based on selection

    void setupUI();
    void loadOwners();
    void refreshDetail(int ownerId);
    int  selectedOwnerId() const;
    int  selectedAnimalId() const;
    int  selectedVisitId() const;
};
