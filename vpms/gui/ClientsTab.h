#pragma once
#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "../core/ClinicService.h"

class ClientsTab : public QWidget {
    Q_OBJECT
public:
    explicit ClientsTab(ClinicService& svc, QWidget* parent = nullptr);

private slots:
    void onOwnerSelected();
    void onAnimalSelected();
    void onOwnerAdd();
    void onOwnerEdit();
    void onOwnerDelete();
    void onAnimalAdd();
    void onAnimalEdit();
    void onAnimalDelete();
    void onVisitNew();
    void onVisitCancel();
    void onOwnerSearch(const QString& text);

private:
    ClinicService& _svc;

    QLineEdit*    _ownerSearch;
    QListWidget*  _ownerList;
    QPushButton*  _ownerAddBtn;
    QPushButton*  _ownerEditBtn;
    QPushButton*  _ownerDeleteBtn;

    QLabel*       _animalsHeader;
    QTableWidget* _animalTable;
    QPushButton*  _animalAddBtn;
    QPushButton*  _animalEditBtn;
    QPushButton*  _animalDeleteBtn;

    QLabel*       _visitsHeader;
    QTableWidget* _visitTable;
    QPushButton*  _visitNewBtn;
    QPushButton*  _visitCancelBtn;

    void setupUI();
    void loadOwners(const QString& filter = "");
    void loadAnimals(int ownerId);
    void loadVisits(int animalId);
    int  selectedOwnerId()  const;
    int  selectedAnimalId() const;
    int  selectedVisitId()  const;
    void clearAnimals();
    void clearVisits();
};
