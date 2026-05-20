#include "ClientsTab.h"
#include "../animals/Dog.h"
#include "../animals/Cat.h"
#include "../animals/Bird.h"
#include "../animals/Reptile.h"
#include "../core/exceptions.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QDate>

static QHBoxLayout* btnRow(std::initializer_list<QPushButton*> btns) {
    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 2, 0, 4);
    row->addStretch();
    for (auto* b : btns) row->addWidget(b);
    return row;
}

static QString typeLabel(const string& tag) {
    if (tag == "DOG")     return "Dog";
    if (tag == "CAT")     return "Cat";
    if (tag == "BIRD")    return "Bird";
    if (tag == "REPTILE") return "Reptile";
    return QString::fromStdString(tag);
}

static QString svcName(ClinicService& svc, int sid) {
    for (Service* s : svc.getAllServices())
        if (s->getId() == sid) return QString::fromStdString(s->getName());
    return QString("#%1").arg(sid);
}

ClientsTab::ClientsTab(ClinicService& svc, QWidget* parent)
    : QWidget(parent), _svc(svc)
{
    setupUI();
    loadOwners();
}

void ClientsTab::setupUI()
{
    // ─── Left: owners ─────────────────────────────────────────
    _ownerSearch = new QLineEdit(this);
    _ownerSearch->setPlaceholderText("Search clients…");
    _ownerSearch->setClearButtonEnabled(true);

    _ownerList = new QListWidget(this);

    _ownerAddBtn    = new QPushButton("Add",    this);
    _ownerEditBtn   = new QPushButton("Edit",   this);
    _ownerDeleteBtn = new QPushButton("Delete", this);
    _ownerEditBtn->setEnabled(false);
    _ownerDeleteBtn->setEnabled(false);

    auto* lw = new QWidget(this);
    auto* ll = new QVBoxLayout(lw);
    ll->setContentsMargins(6,6,6,6); ll->setSpacing(4);
    ll->addWidget(new QLabel("<b>Clients</b>"));
    ll->addWidget(_ownerSearch);
    ll->addWidget(_ownerList, 1);
    ll->addLayout(btnRow({_ownerAddBtn, _ownerEditBtn, _ownerDeleteBtn}));

    // ─── Middle: animals ──────────────────────────────────────
    _animalsHeader = new QLabel("<i>\xe2\x86\x90 Select a client</i>", this);

    _animalTable = new QTableWidget(0, 4, this);
    _animalTable->setHorizontalHeaderLabels({"Name","Type","Age","Info"});
    _animalTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _animalTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _animalTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _animalTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    _animalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _animalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _animalTable->setAlternatingRowColors(true);
    _animalTable->verticalHeader()->hide();

    _animalAddBtn    = new QPushButton("Add",    this);
    _animalEditBtn   = new QPushButton("Edit",   this);
    _animalDeleteBtn = new QPushButton("Delete", this);
    _animalAddBtn->setEnabled(false);
    _animalEditBtn->setEnabled(false);
    _animalDeleteBtn->setEnabled(false);

    auto* mw = new QWidget(this);
    auto* ml = new QVBoxLayout(mw);
    ml->setContentsMargins(6,6,6,6); ml->setSpacing(4);
    ml->addWidget(new QLabel("<b>Animals</b>"));
    ml->addWidget(_animalsHeader);
    ml->addWidget(_animalTable, 1);
    ml->addLayout(btnRow({_animalAddBtn, _animalEditBtn, _animalDeleteBtn}));

    // ─── Right: visits ────────────────────────────────────────
    _visitsHeader = new QLabel("<i>\xe2\x86\x90 Select an animal</i>", this);

    _visitTable = new QTableWidget(0, 3, this);
    _visitTable->setHorizontalHeaderLabels({"Date","Services","Cost ($)"});
    _visitTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _visitTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _visitTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _visitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _visitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _visitTable->setAlternatingRowColors(true);
    _visitTable->verticalHeader()->hide();

    _visitNewBtn    = new QPushButton("New Visit",    this);
    _visitCancelBtn = new QPushButton("Cancel Visit", this);
    _visitNewBtn->setEnabled(false);
    _visitCancelBtn->setEnabled(false);

    auto* rw = new QWidget(this);
    auto* rl = new QVBoxLayout(rw);
    rl->setContentsMargins(6,6,6,6); rl->setSpacing(4);
    rl->addWidget(new QLabel("<b>Visit History</b>"));
    rl->addWidget(_visitsHeader);
    rl->addWidget(_visitTable, 1);
    rl->addLayout(btnRow({_visitNewBtn, _visitCancelBtn}));

    // ─── Splitter ─────────────────────────────────────────────
    auto* spl = new QSplitter(Qt::Horizontal, this);
    spl->addWidget(lw);
    spl->addWidget(mw);
    spl->addWidget(rw);
    spl->setSizes({260,360,420});

    auto* main = new QVBoxLayout(this);
    main->setContentsMargins(6,6,6,6);
    main->addWidget(spl);

    // ─── Connections ──────────────────────────────────────────
    connect(_ownerSearch, &QLineEdit::textChanged,     this, &ClientsTab::onOwnerSearch);
    connect(_ownerList,   &QListWidget::currentItemChanged,
            this, [this](QListWidgetItem*) { onOwnerSelected(); });
    connect(_animalTable, &QTableWidget::currentCellChanged,
            this, [this](int,int,int,int) { onAnimalSelected(); });
    connect(_visitTable,  &QTableWidget::currentCellChanged,
            this, [this](int row,int,int,int) { _visitCancelBtn->setEnabled(row>=0); });

    connect(_ownerAddBtn,     &QPushButton::clicked, this, &ClientsTab::onOwnerAdd);
    connect(_ownerEditBtn,    &QPushButton::clicked, this, &ClientsTab::onOwnerEdit);
    connect(_ownerDeleteBtn,  &QPushButton::clicked, this, &ClientsTab::onOwnerDelete);
    connect(_animalAddBtn,    &QPushButton::clicked, this, &ClientsTab::onAnimalAdd);
    connect(_animalEditBtn,   &QPushButton::clicked, this, &ClientsTab::onAnimalEdit);
    connect(_animalDeleteBtn, &QPushButton::clicked, this, &ClientsTab::onAnimalDelete);
    connect(_visitNewBtn,     &QPushButton::clicked, this, &ClientsTab::onVisitNew);
    connect(_visitCancelBtn,  &QPushButton::clicked, this, &ClientsTab::onVisitCancel);
}

void ClientsTab::loadOwners(const QString& filter)
{
    int prevId = selectedOwnerId();
    _ownerList->blockSignals(true);
    _ownerList->clear();
    for (const Owner& o : _svc.getAllOwners()) {
        QString name = QString::fromStdString(o.getName());
        if (!filter.isEmpty() && !name.contains(filter, Qt::CaseInsensitive)) continue;
        auto* item = new QListWidgetItem(
            name + "   " + QString::fromStdString(o.getContactInfo()), _ownerList);
        item->setData(Qt::UserRole, o.getId());
    }
    for (int i = 0; i < _ownerList->count(); ++i) {
        if (_ownerList->item(i)->data(Qt::UserRole).toInt() == prevId) {
            _ownerList->setCurrentRow(i); break;
        }
    }
    _ownerList->blockSignals(false);
    bool sel = (_ownerList->currentItem() != nullptr);
    _ownerEditBtn->setEnabled(sel);
    _ownerDeleteBtn->setEnabled(sel);
    _animalAddBtn->setEnabled(sel);
}

void ClientsTab::loadAnimals(int ownerId)
{
    clearVisits();
    _animalTable->setRowCount(0);
    try {
        Owner& o = _svc.getOwner(ownerId);
        _animalsHeader->setText(
            QString("<b>%1</b>   <span style='color:#555'>%2</span>")
                .arg(QString::fromStdString(o.getName()))
                .arg(QString::fromStdString(o.getContactInfo())));
    } catch (...) { return; }

    for (Animal* a : _svc.getAnimalsByOwner(ownerId)) {
        int row = _animalTable->rowCount();
        _animalTable->insertRow(row);
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(a->getName()));
        nameItem->setData(Qt::UserRole, a->getId());
        _animalTable->setItem(row, 0, nameItem);
        _animalTable->setItem(row, 1, new QTableWidgetItem(typeLabel(a->getTypeTag())));
        _animalTable->setItem(row, 2, new QTableWidgetItem(QString::number(a->getAge()) + " yr"));
        _animalTable->setItem(row, 3, new QTableWidgetItem(
            QString::fromStdString(a->getSpeciesInfo())));
    }
}

void ClientsTab::loadVisits(int animalId)
{
    _visitTable->setRowCount(0);
    try {
        Animal* a = _svc.getAnimal(animalId);
        _visitsHeader->setText(
            QString("<b>%1</b>   <span style='color:#555'>%2</span>")
                .arg(QString::fromStdString(a->getName()))
                .arg(typeLabel(a->getTypeTag())));
    } catch (...) { return; }

    for (const Visit& v : _svc.getVisitsByAnimal(animalId)) {
        int row = _visitTable->rowCount();
        _visitTable->insertRow(row);
        QStringList names;
        for (int sid : v.getServiceIds()) names << svcName(_svc, sid);
        auto* di = new QTableWidgetItem(QString::fromStdString(v.getDate()));
        di->setData(Qt::UserRole, v.getId());
        _visitTable->setItem(row, 0, di);
        _visitTable->setItem(row, 1, new QTableWidgetItem(names.join(", ")));
        _visitTable->setItem(row, 2, new QTableWidgetItem(
            QString::number(v.getTotalCost(), 'f', 2)));
    }
    _visitCancelBtn->setEnabled(false);
}

void ClientsTab::clearAnimals()
{
    _animalTable->setRowCount(0);
    _animalsHeader->setText("<i>\xe2\x86\x90 Select a client</i>");
    _animalAddBtn->setEnabled(false);
    _animalEditBtn->setEnabled(false);
    _animalDeleteBtn->setEnabled(false);
    clearVisits();
}

void ClientsTab::clearVisits()
{
    _visitTable->setRowCount(0);
    _visitsHeader->setText("<i>\xe2\x86\x90 Select an animal</i>");
    _visitNewBtn->setEnabled(false);
    _visitCancelBtn->setEnabled(false);
}

int ClientsTab::selectedOwnerId() const {
    auto* item = _ownerList->currentItem();
    return item ? item->data(Qt::UserRole).toInt() : -1;
}
int ClientsTab::selectedAnimalId() const {
    int row = _animalTable->currentRow(); if (row < 0) return -1;
    auto* item = _animalTable->item(row, 0);
    return item ? item->data(Qt::UserRole).toInt() : -1;
}
int ClientsTab::selectedVisitId() const {
    int row = _visitTable->currentRow(); if (row < 0) return -1;
    auto* item = _visitTable->item(row, 0);
    return item ? item->data(Qt::UserRole).toInt() : -1;
}

void ClientsTab::onOwnerSearch(const QString& text) { loadOwners(text); }

void ClientsTab::onOwnerSelected() {
    int id = selectedOwnerId(); bool sel = id >= 0;
    _ownerEditBtn->setEnabled(sel); _ownerDeleteBtn->setEnabled(sel);
    _animalAddBtn->setEnabled(sel);
    if (sel) loadAnimals(id); else clearAnimals();
}

void ClientsTab::onAnimalSelected() {
    int id = selectedAnimalId(); bool sel = id >= 0;
    _animalEditBtn->setEnabled(sel); _animalDeleteBtn->setEnabled(sel);
    _visitNewBtn->setEnabled(sel);
    if (sel) loadVisits(id); else clearVisits();
}

void ClientsTab::onOwnerAdd()
{
    QDialog dlg(this); dlg.setWindowTitle("Add Client"); dlg.setMinimumWidth(320);
    auto* form = new QFormLayout(&dlg);
    auto* nameEdit = new QLineEdit(&dlg);
    auto* contactEdit = new QLineEdit(&dlg);
    contactEdit->setPlaceholderText("Phone / email");
    form->addRow("Full name:",    nameEdit);
    form->addRow("Contact info:", contactEdit);
    auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted) return;
    try {
        _svc.addOwner(nameEdit->text().toStdString(), contactEdit->text().toStdString());
        loadOwners(_ownerSearch->text());
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onOwnerEdit()
{
    int id = selectedOwnerId(); if (id < 0) return;
    try {
        Owner& o = _svc.getOwner(id);
        QDialog dlg(this); dlg.setWindowTitle("Edit Client"); dlg.setMinimumWidth(320);
        auto* form = new QFormLayout(&dlg);
        auto* nameEdit    = new QLineEdit(QString::fromStdString(o.getName()),        &dlg);
        auto* contactEdit = new QLineEdit(QString::fromStdString(o.getContactInfo()), &dlg);
        form->addRow("Full name:",    nameEdit);
        form->addRow("Contact info:", contactEdit);
        auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, &dlg);
        form->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() != QDialog::Accepted) return;
        o.setName(nameEdit->text().toStdString());
        o.setContactInfo(contactEdit->text().toStdString());
        _svc.updateOwner(o);
        loadOwners(_ownerSearch->text());
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onOwnerDelete()
{
    int id = selectedOwnerId(); if (id < 0) return;
    try {
        QString name = QString::fromStdString(_svc.getOwner(id).getName());
        if (QMessageBox::question(this, "Delete Client",
                "Delete \"" + name + "\"?\nAll their animals must be removed first.",
                QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;
        _svc.removeOwner(id);
        clearAnimals();
        loadOwners(_ownerSearch->text());
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onAnimalAdd()
{
    int ownerId = selectedOwnerId(); if (ownerId < 0) return;
    QDialog dlg(this); dlg.setWindowTitle("Add Animal"); dlg.setMinimumWidth(340);
    auto* form = new QFormLayout(&dlg);
    auto* nameEdit  = new QLineEdit(&dlg);
    auto* typeCombo = new QComboBox(&dlg);
    typeCombo->addItems({"Dog","Cat","Bird","Reptile"});
    auto* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1,50); ageSpin->setSuffix(" years");
    auto* extraLbl  = new QLabel("Breed:", &dlg);
    auto* extraEdit = new QLineEdit(&dlg);
    form->addRow("Name:",  nameEdit);
    form->addRow("Type:",  typeCombo);
    form->addRow("Age:",   ageSpin);
    form->addRow(extraLbl, extraEdit);
    static const char* xlbls[] = {"Breed:","Fur type:","Species:","Reptile type:"};
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int i){ extraLbl->setText(xlbls[i]); });
    auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted) return;
    static const char* tags[] = {"DOG","CAT","BIRD","REPTILE"};
    try {
        _svc.addAnimal(tags[typeCombo->currentIndex()],
                       nameEdit->text().toStdString(),
                       ageSpin->value(), ownerId,
                       extraEdit->text().toStdString());
        loadAnimals(ownerId);
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onAnimalEdit()
{
    int animalId = selectedAnimalId(); int ownerId = selectedOwnerId();
    if (animalId < 0 || ownerId < 0) return;
    try {
        Animal* a = _svc.getAnimal(animalId);
        QString tag  = QString::fromStdString(a->getTypeTag());
        QString info = QString::fromStdString(a->getSpeciesInfo());
        QString curExtra = info.section(": ", 1);
        QString extraLblText = (tag=="DOG") ? "Breed:" :
                               (tag=="CAT") ? "Fur type:" :
                               (tag=="BIRD") ? "Species:" : "Reptile type:";
        QDialog dlg(this); dlg.setWindowTitle("Edit Animal"); dlg.setMinimumWidth(340);
        auto* form = new QFormLayout(&dlg);
        auto* nameEdit  = new QLineEdit(QString::fromStdString(a->getName()), &dlg);
        auto* ageSpin   = new QSpinBox(&dlg);
        ageSpin->setRange(1,50); ageSpin->setSuffix(" years"); ageSpin->setValue(a->getAge());
        auto* extraEdit = new QLineEdit(curExtra, &dlg);
        form->addRow("Name:",      nameEdit);
        form->addRow("Age:",       ageSpin);
        form->addRow(extraLblText, extraEdit);
        auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, &dlg);
        form->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() != QDialog::Accepted) return;
        string name = nameEdit->text().toStdString();
        int    age  = ageSpin->value();
        string ext  = extraEdit->text().toStdString();
        Animal* upd = nullptr;
        if (tag=="DOG")        upd = new Dog(animalId, name, age, ownerId, ext);
        else if (tag=="CAT")   upd = new Cat(animalId, name, age, ownerId, ext);
        else if (tag=="BIRD")  upd = new Bird(animalId, name, age, ownerId, ext);
        else                   upd = new Reptile(animalId, name, age, ownerId, ext);
        _svc.updateAnimal(upd);
        loadAnimals(ownerId);
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onAnimalDelete()
{
    int animalId = selectedAnimalId(); int ownerId = selectedOwnerId();
    if (animalId < 0) return;
    try {
        QString name = QString::fromStdString(_svc.getAnimal(animalId)->getName());
        if (QMessageBox::question(this, "Delete Animal",
                "Delete \"" + name + "\"?",
                QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;
        _svc.removeAnimal(animalId);
        clearVisits();
        loadAnimals(ownerId);
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onVisitNew()
{
    int animalId = selectedAnimalId(); int ownerId = selectedOwnerId();
    if (animalId < 0 || ownerId < 0) return;
    const auto& allSvc = _svc.getAllServices();
    if (allSvc.empty()) {
        QMessageBox::information(this, "No Services",
            "Please add services in the Services tab first.");
        return;
    }
    QDialog dlg(this); dlg.setWindowTitle("New Visit"); dlg.setMinimumWidth(400);
    auto* vl = new QVBoxLayout(&dlg);
    auto* form = new QFormLayout;
    auto* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true); dateEdit->setDisplayFormat("yyyy-MM-dd");
    form->addRow("Date:", dateEdit);
    vl->addLayout(form);
    vl->addWidget(new QLabel("Services (check one or more):", &dlg));
    auto* svcList = new QListWidget(&dlg);
    svcList->setSelectionMode(QAbstractItemView::NoSelection);
    for (Service* s : allSvc) {
        auto* item = new QListWidgetItem(
            QString("%1  —  $%2  —  %3")
                .arg(QString::fromStdString(s->getName()))
                .arg(s->getFinalPrice(), 0, 'f', 2)
                .arg(QString::fromStdString(s->getDescription())),
            svcList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, s->getId());
    }
    vl->addWidget(svcList);
    auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, &dlg);
    vl->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted) return;
    vector<int> ids;
    for (int i = 0; i < svcList->count(); ++i)
        if (svcList->item(i)->checkState() == Qt::Checked)
            ids.push_back(svcList->item(i)->data(Qt::UserRole).toInt());
    if (ids.empty()) {
        QMessageBox::warning(this,"No Services","Please check at least one service.");
        return;
    }
    try {
        _svc.createVisit(animalId, ownerId,
                         dateEdit->date().toString("yyyy-MM-dd").toStdString(), ids);
        loadVisits(animalId);
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}

void ClientsTab::onVisitCancel()
{
    int visitId = selectedVisitId(); int animalId = selectedAnimalId();
    if (visitId < 0) return;
    if (QMessageBox::question(this,"Cancel Visit","Remove this visit record?",
            QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;
    try {
        _svc.cancelVisit(visitId);
        loadVisits(animalId);
    } catch (const exception& e) { QMessageBox::warning(this, "Error", e.what()); }
}
