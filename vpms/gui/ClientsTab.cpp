#include "ClientsTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QListWidget>
#include <QDateEdit>
#include <QDate>
#include <QFrame>

ClientsTab::ClientsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadOwners();
}

void ClientsTab::setupUI() {
    // ── Left: Owner list ────────────────────────────
    _ownerTable = new QTableWidget(this);
    _ownerTable->setColumnCount(2);
    _ownerTable->setHorizontalHeaderLabels({"Name", "Contact"});
    _ownerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ownerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ownerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _ownerTable->horizontalHeader()->setStretchLastSection(true);
    _ownerTable->verticalHeader()->setVisible(false);

    _ownerAddBtn    = new QPushButton("+ Add",   this);
    _ownerEditBtn   = new QPushButton("Edit",    this);
    _ownerDeleteBtn = new QPushButton("Delete",  this);

    QHBoxLayout* ownerBtns = new QHBoxLayout;
    ownerBtns->addWidget(_ownerAddBtn);
    ownerBtns->addWidget(_ownerEditBtn);
    ownerBtns->addWidget(_ownerDeleteBtn);

    QGroupBox* ownerPanel = new QGroupBox("Clients", this);
    QVBoxLayout* ownerL = new QVBoxLayout(ownerPanel);
    ownerL->addWidget(_ownerTable);
    ownerL->addLayout(ownerBtns);

    // ── Right: Detail ───────────────────────────────
    _detailHeader = new QLabel("← Select a client", this);
    _detailHeader->setStyleSheet("font-size: 14px; font-weight: bold; padding: 4px;");

    // Animals sub-tab
    _animalTable = new QTableWidget(this);
    _animalTable->setColumnCount(4);
    _animalTable->setHorizontalHeaderLabels({"Type", "Name", "Age", "Species Info"});
    _animalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _animalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _animalTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _animalTable->horizontalHeader()->setStretchLastSection(true);
    _animalTable->verticalHeader()->setVisible(false);

    _animalAddBtn    = new QPushButton("+ Add Animal", this);
    _animalEditBtn   = new QPushButton("Edit",         this);
    _animalDeleteBtn = new QPushButton("Delete",       this);

    QHBoxLayout* animalBtns = new QHBoxLayout;
    animalBtns->addWidget(_animalAddBtn);
    animalBtns->addWidget(_animalEditBtn);
    animalBtns->addWidget(_animalDeleteBtn);
    animalBtns->addStretch();

    QWidget* animalWidget = new QWidget(this);
    QVBoxLayout* animalL = new QVBoxLayout(animalWidget);
    animalL->setContentsMargins(0, 0, 0, 0);
    animalL->addWidget(_animalTable);
    animalL->addLayout(animalBtns);

    // Visits sub-tab
    _visitTable = new QTableWidget(this);
    _visitTable->setColumnCount(4);
    _visitTable->setHorizontalHeaderLabels({"Animal", "Date", "Services", "Total ($)"});
    _visitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _visitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _visitTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _visitTable->horizontalHeader()->setStretchLastSection(true);
    _visitTable->verticalHeader()->setVisible(false);

    _visitAddBtn    = new QPushButton("+ New Visit",    this);
    _visitCancelBtn = new QPushButton("Cancel Visit",   this);

    QHBoxLayout* visitBtns = new QHBoxLayout;
    visitBtns->addWidget(_visitAddBtn);
    visitBtns->addWidget(_visitCancelBtn);
    visitBtns->addStretch();

    QWidget* visitWidget = new QWidget(this);
    QVBoxLayout* visitL = new QVBoxLayout(visitWidget);
    visitL->setContentsMargins(0, 0, 0, 0);
    visitL->addWidget(_visitTable);
    visitL->addLayout(visitBtns);

    // Assemble tabs
    _detailTabs = new QTabWidget(this);
    _detailTabs->addTab(animalWidget, "Animals");
    _detailTabs->addTab(visitWidget,  "Visit History");

    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightL = new QVBoxLayout(rightPanel);
    rightL->addWidget(_detailHeader);
    rightL->addWidget(_detailTabs);

    // ── Splitter ─────────────────────────────────────
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(ownerPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setSizes({280, 560});

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addWidget(splitter);

    // ── Connections ──────────────────────────────────
    connect(_ownerTable, &QTableWidget::itemSelectionChanged, this, &ClientsTab::onOwnerSelected);
    connect(_ownerAddBtn,    &QPushButton::clicked, this, &ClientsTab::onOwnerAdd);
    connect(_ownerEditBtn,   &QPushButton::clicked, this, &ClientsTab::onOwnerEdit);
    connect(_ownerDeleteBtn, &QPushButton::clicked, this, &ClientsTab::onOwnerDelete);
    connect(_animalAddBtn,   &QPushButton::clicked, this, &ClientsTab::onAnimalAdd);
    connect(_animalEditBtn,  &QPushButton::clicked, this, &ClientsTab::onAnimalEdit);
    connect(_animalDeleteBtn,&QPushButton::clicked, this, &ClientsTab::onAnimalDelete);
    connect(_visitAddBtn,    &QPushButton::clicked, this, &ClientsTab::onVisitAdd);
    connect(_visitCancelBtn, &QPushButton::clicked, this, &ClientsTab::onVisitCancel);
}

// ── Data loading ─────────────────────────────────────

void ClientsTab::loadOwners() {
    const auto& owners = _service.getAllOwners();
    _ownerTable->setRowCount((int)owners.size());
    for (int i = 0; i < (int)owners.size(); i++) {
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(owners[i].getName()));
        nameItem->setData(Qt::UserRole, owners[i].getId());
        _ownerTable->setItem(i, 0, nameItem);
        _ownerTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(owners[i].getContactInfo())));
    }
}

void ClientsTab::refreshDetail(int ownerId) {
    loadAnimals(ownerId);
    loadVisits(ownerId);
}

void ClientsTab::loadAnimals(int ownerId) {
    auto animals = _service.getAnimalsByOwner(ownerId);
    _animalTable->setRowCount((int)animals.size());
    for (int i = 0; i < (int)animals.size(); i++) {
        Animal* a = animals[i];
        auto* typeItem = new QTableWidgetItem(QString::fromStdString(a->getTypeTag()));
        typeItem->setData(Qt::UserRole, a->getId());
        _animalTable->setItem(i, 0, typeItem);
        _animalTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(a->getName())));
        _animalTable->setItem(i, 2, new QTableWidgetItem(QString::number(a->getAge()) + " yrs"));
        _animalTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(a->getSpeciesInfo())));
    }
}

void ClientsTab::loadVisits(int ownerId) {
    auto visits = _service.getVisitsByOwner(ownerId);
    const auto& animals = _service.getAllAnimals();
    _visitTable->setRowCount((int)visits.size());
    for (int i = 0; i < (int)visits.size(); i++) {
        const Visit& v = visits[i];
        QString animalName = QString::number(v.getAnimalId());
        for (Animal* a : animals)
            if (a->getId() == v.getAnimalId()) { animalName = QString::fromStdString(a->getName()); break; }

        QString serviceNames;
        for (int sid : v.getServiceIds()) {
            try {
                if (!serviceNames.isEmpty()) serviceNames += ", ";
                serviceNames += QString::fromStdString(_service.getAllServices()[0]->getName());
                for (Service* s : _service.getAllServices())
                    if (s->getId() == sid) { serviceNames = serviceNames.left(serviceNames.lastIndexOf(", ") < 0 ? 0 : serviceNames.lastIndexOf(", ")); if (!serviceNames.isEmpty()) serviceNames += ", "; serviceNames += QString::fromStdString(s->getName()); break; }
            } catch (...) {}
        }
        // simpler: just show count
        int svcCount = (int)v.getServiceIds().size();
        QString svcStr = svcCount > 0 ? QString("%1 service(s)").arg(svcCount) : "—";

        auto* visitItem = new QTableWidgetItem(animalName);
        visitItem->setData(Qt::UserRole, v.getId());
        _visitTable->setItem(i, 0, visitItem);
        _visitTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(v.getDate())));
        _visitTable->setItem(i, 2, new QTableWidgetItem(svcStr));
        _visitTable->setItem(i, 3, new QTableWidgetItem(QString::number(v.getTotalCost(), 'f', 2)));
    }
}

// ── Helpers ──────────────────────────────────────────

int ClientsTab::selectedOwnerId() const {
    int row = _ownerTable->currentRow();
    if (row < 0) return -1;
    return _ownerTable->item(row, 0)->data(Qt::UserRole).toInt();
}

int ClientsTab::selectedAnimalId() const {
    int row = _animalTable->currentRow();
    if (row < 0) return -1;
    return _animalTable->item(row, 0)->data(Qt::UserRole).toInt();
}

int ClientsTab::selectedVisitId() const {
    int row = _visitTable->currentRow();
    if (row < 0) return -1;
    return _visitTable->item(row, 0)->data(Qt::UserRole).toInt();
}

void ClientsTab::onOwnerSelected() {
    int id = selectedOwnerId();
    if (id < 0) return;
    QString name = _ownerTable->item(_ownerTable->currentRow(), 0)->text();
    QString contact = _ownerTable->item(_ownerTable->currentRow(), 1)->text();
    _detailHeader->setText(name + "   |   " + contact);
    refreshDetail(id);
}

// ── Owner CRUD ───────────────────────────────────────

void ClientsTab::onOwnerAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Client");
    dlg.setMinimumWidth(300);
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("Full name");
    form.addRow("Name:", nameEdit);

    QLineEdit* contactEdit = new QLineEdit(&dlg);
    contactEdit->setPlaceholderText("Phone or email");
    form.addRow("Contact:", contactEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Name cannot be empty."); return;
    }
    try {
        _service.addOwner(nameEdit->text().trimmed().toStdString(),
                          contactEdit->text().trimmed().toStdString());
        loadOwners();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ClientsTab::onOwnerEdit() {
    int id = selectedOwnerId();
    if (id < 0) { QMessageBox::information(this, "Edit", "Select a client first."); return; }
    int row = _ownerTable->currentRow();

    QDialog dlg(this);
    dlg.setWindowTitle("Edit Client");
    dlg.setMinimumWidth(300);
    QFormLayout form(&dlg);

    QLineEdit* nameEdit    = new QLineEdit(_ownerTable->item(row, 0)->text(), &dlg);
    QLineEdit* contactEdit = new QLineEdit(_ownerTable->item(row, 1)->text(), &dlg);
    form.addRow("Name:",    nameEdit);
    form.addRow("Contact:", contactEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) return;
    try {
        Owner o = _service.getOwner(id);
        o.setName(nameEdit->text().trimmed().toStdString());
        o.setContactInfo(contactEdit->text().trimmed().toStdString());
        _service.updateOwner(o);
        loadOwners();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ClientsTab::onOwnerDelete() {
    int id = selectedOwnerId();
    if (id < 0) { QMessageBox::information(this, "Delete", "Select a client first."); return; }
    QString name = _ownerTable->item(_ownerTable->currentRow(), 0)->text();

    if (QMessageBox::question(this, "Delete Client",
        QString("Delete \"%1\" and all their records?").arg(name),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    try {
        _service.removeOwner(id);
        loadOwners();
        _animalTable->setRowCount(0);
        _visitTable->setRowCount(0);
        _detailHeader->setText("← Select a client");
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

// ── Animal CRUD ──────────────────────────────────────

void ClientsTab::onAnimalAdd() {
    int ownerId = selectedOwnerId();
    if (ownerId < 0) { QMessageBox::information(this, "Add Animal", "Select a client first."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("Add Animal");
    dlg.setMinimumWidth(300);
    QFormLayout form(&dlg);

    QComboBox* typeBox = new QComboBox(&dlg);
    typeBox->addItems({"Dog", "Cat", "Bird", "Reptile"});
    form.addRow("Type:", typeBox);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("Animal name");
    form.addRow("Name:", nameEdit);

    QSpinBox* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1, 100);
    ageSpin->setSuffix(" years");
    form.addRow("Age:", ageSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Name cannot be empty."); return;
    }
    try {
        _service.addAnimal(typeBox->currentText().toUpper().toStdString(),
                           nameEdit->text().trimmed().toStdString(),
                           ageSpin->value(), ownerId);
        loadAnimals(ownerId);
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ClientsTab::onAnimalEdit() {
    int ownerId  = selectedOwnerId();
    int animalId = selectedAnimalId();
    if (ownerId < 0 || animalId < 0) {
        QMessageBox::information(this, "Edit", "Select a client and an animal first."); return;
    }
    int row = _animalTable->currentRow();

    QDialog dlg(this);
    dlg.setWindowTitle("Edit Animal");
    dlg.setMinimumWidth(280);
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(_animalTable->item(row, 1)->text(), &dlg);
    form.addRow("Name:", nameEdit);

    QSpinBox* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1, 100);
    ageSpin->setSuffix(" years");
    ageSpin->setValue(_animalTable->item(row, 2)->text().toInt());
    form.addRow("Age:", ageSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) return;
    try {
        Animal* a = _service.getAnimal(animalId);
        a->setName(nameEdit->text().trimmed().toStdString());
        a->setAge(ageSpin->value());
        _service.updateAnimal(a);
        loadAnimals(ownerId);
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ClientsTab::onAnimalDelete() {
    int ownerId  = selectedOwnerId();
    int animalId = selectedAnimalId();
    if (ownerId < 0 || animalId < 0) {
        QMessageBox::information(this, "Delete", "Select a client and an animal first."); return;
    }
    QString name = _animalTable->item(_animalTable->currentRow(), 1)->text();
    if (QMessageBox::question(this, "Delete Animal",
        QString("Delete \"%1\"?").arg(name),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    try {
        _service.removeAnimal(animalId);
        loadAnimals(ownerId);
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

// ── Visit CRUD ───────────────────────────────────────

void ClientsTab::onVisitAdd() {
    int ownerId = selectedOwnerId();
    if (ownerId < 0) { QMessageBox::information(this, "New Visit", "Select a client first."); return; }

    auto ownerAnimals = _service.getAnimalsByOwner(ownerId);
    if (ownerAnimals.empty()) {
        QMessageBox::warning(this, "New Visit", "This client has no animals. Add an animal first."); return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("New Visit");
    dlg.setMinimumWidth(340);
    QFormLayout form(&dlg);

    QComboBox* animalBox = new QComboBox(&dlg);
    for (Animal* a : ownerAnimals)
        animalBox->addItem(QString::fromStdString(a->getName()) +
                           " (" + QString::fromStdString(a->getTypeTag()) + ")", a->getId());
    form.addRow("Animal:", animalBox);

    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Date:", dateEdit);

    const auto& allServices = _service.getAllServices();
    QListWidget* svcList = new QListWidget(&dlg);
    svcList->setSelectionMode(QAbstractItemView::MultiSelection);
    svcList->setMaximumHeight(120);
    for (Service* s : allServices) {
        auto* item = new QListWidgetItem(
            QString::fromStdString(s->getName()) +
            QString("  —  $%1").arg(s->getFinalPrice(), 0, 'f', 2));
        item->setData(Qt::UserRole, s->getId());
        svcList->addItem(item);
    }
    if (allServices.empty()) svcList->addItem("(no services in catalog)");
    form.addRow("Services:", svcList);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int animalId = animalBox->currentData().toInt();
    vector<int> serviceIds;
    for (QListWidgetItem* item : svcList->selectedItems())
        serviceIds.push_back(item->data(Qt::UserRole).toInt());

    try {
        _service.createVisit(animalId, ownerId,
            dateEdit->date().toString("yyyy-MM-dd").toStdString(), serviceIds);
        loadVisits(ownerId);
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ClientsTab::onVisitCancel() {
    int ownerId = selectedOwnerId();
    int visitId = selectedVisitId();
    if (ownerId < 0 || visitId < 0) {
        QMessageBox::information(this, "Cancel Visit", "Select a visit first."); return;
    }
    if (QMessageBox::question(this, "Cancel Visit", "Cancel this visit?",
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    try {
        _service.cancelVisit(visitId);
        loadVisits(ownerId);
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
