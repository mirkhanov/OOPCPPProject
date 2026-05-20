#include "VisitsTab.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QDateEdit>
#include <QDate>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>

using namespace std;

VisitsTab::VisitsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _svc(service)
{
    setupUI();
    loadVisits();
}

void VisitsTab::setupUI()
{
    // --- top bar ---
    QLabel* filterLabel = new QLabel("Filter by client:", this);
    _ownerFilter = new QComboBox(this);
    _ownerFilter->addItem("All Clients", -1);
    for (const Owner& o : _svc.getAllOwners())
        _ownerFilter->addItem(QString::fromStdString(o.getName()), o.getId());

    _newVisitBtn = new QPushButton("New Visit",    this);
    _cancelBtn   = new QPushButton("Cancel Visit", this);

    QHBoxLayout* topBar = new QHBoxLayout;
    topBar->addWidget(filterLabel);
    topBar->addWidget(_ownerFilter);
    topBar->addStretch();
    topBar->addWidget(_newVisitBtn);
    topBar->addWidget(_cancelBtn);

    // --- table ---
    _visitTable = new QTableWidget(this);
    _visitTable->setColumnCount(5);
    _visitTable->setHorizontalHeaderLabels({"Date", "Client", "Animal", "Services", "Cost ($)"});
    _visitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _visitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _visitTable->setAlternatingRowColors(true);
    _visitTable->horizontalHeader()->setStretchLastSection(true);
    _visitTable->verticalHeader()->hide();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(_visitTable);

    connect(_ownerFilter,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VisitsTab::onFilterChanged);
    connect(_newVisitBtn,  &QPushButton::clicked, this, &VisitsTab::onNewVisit);
    connect(_cancelBtn,    &QPushButton::clicked, this, &VisitsTab::onCancelVisit);
}

void VisitsTab::loadVisits()
{
    // Refresh owner filter (preserve selection)
    int selectedOwnerId = _ownerFilter->currentData().toInt();
    _ownerFilter->blockSignals(true);
    _ownerFilter->clear();
    _ownerFilter->addItem("All Clients", -1);
    for (const Owner& o : _svc.getAllOwners())
        _ownerFilter->addItem(QString::fromStdString(o.getName()), o.getId());
    // Restore previous selection
    int idx = _ownerFilter->findData(selectedOwnerId);
    _ownerFilter->setCurrentIndex(idx >= 0 ? idx : 0);
    _ownerFilter->blockSignals(false);

    const auto& allVisits  = _svc.getAllVisits();
    const auto& allAnimals = _svc.getAllAnimals();
    const auto& allOwners  = _svc.getAllOwners();
    const auto& allSvcs    = _svc.getAllServices();

    int filterOwner = _ownerFilter->currentData().toInt();

    // Collect matching visits
    vector<const Visit*> visits;
    for (const Visit& v : allVisits) {
        if (filterOwner == -1 || v.getOwnerId() == filterOwner)
            visits.push_back(&v);
    }

    _visitTable->setRowCount((int)visits.size());

    for (int i = 0; i < (int)visits.size(); ++i) {
        const Visit* v = visits[i];

        // Resolve names
        QString animalName = QString::number(v->getAnimalId());
        QString ownerName  = QString::number(v->getOwnerId());
        for (Animal* a : allAnimals)
            if (a->getId() == v->getAnimalId()) { animalName = QString::fromStdString(a->getName()); break; }
        for (const Owner& o : allOwners)
            if (o.getId() == v->getOwnerId()) { ownerName = QString::fromStdString(o.getName()); break; }

        // Build service name list
        QStringList svcNames;
        for (int sid : v->getServiceIds()) {
            for (Service* s : allSvcs)
                if (s->getId() == sid) { svcNames << QString::fromStdString(s->getName()); break; }
        }

        // Col 0 stores the visit ID in UserRole
        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(v->getDate()));
        dateItem->setData(Qt::UserRole, v->getId());
        _visitTable->setItem(i, 0, dateItem);
        _visitTable->setItem(i, 1, new QTableWidgetItem(ownerName));
        _visitTable->setItem(i, 2, new QTableWidgetItem(animalName));
        _visitTable->setItem(i, 3, new QTableWidgetItem(svcNames.join(", ")));
        _visitTable->setItem(i, 4, new QTableWidgetItem(
            QString("$%1").arg(v->getTotalCost(), 0, 'f', 2)));
    }

    _visitTable->resizeColumnsToContents();
    _visitTable->horizontalHeader()->setStretchLastSection(true);
}

int VisitsTab::selectedVisitId()
{
    int row = _visitTable->currentRow();
    if (row < 0) return -1;
    QTableWidgetItem* item = _visitTable->item(row, 0);
    if (!item) return -1;
    return item->data(Qt::UserRole).toInt();
}

void VisitsTab::onFilterChanged()
{
    loadVisits();
}

void VisitsTab::onNewVisit()
{
    const auto& allOwners  = _svc.getAllOwners();
    const auto& allAnimals = _svc.getAllAnimals();
    const auto& allSvcs    = _svc.getAllServices();

    if (allOwners.empty()) {
        QMessageBox::warning(this, "New Visit", "No clients registered. Add a client first.");
        return;
    }
    if (allAnimals.empty()) {
        QMessageBox::warning(this, "New Visit", "No animals registered. Add an animal first.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("New Visit");
    dlg.setMinimumWidth(400);
    QFormLayout form(&dlg);

    // Owner dropdown
    QComboBox* ownerBox = new QComboBox(&dlg);
    for (const Owner& o : allOwners)
        ownerBox->addItem(QString::fromStdString(o.getName()), o.getId());
    form.addRow("Client:", ownerBox);

    // Animal dropdown (updates when owner changes)
    QComboBox* animalBox = new QComboBox(&dlg);
    auto repopulateAnimals = [&]() {
        animalBox->clear();
        int oid = ownerBox->currentData().toInt();
        for (Animal* a : allAnimals) {
            if (a->getOwnerId() == oid)
                animalBox->addItem(
                    QString::fromStdString(a->getName()) +
                    " (" + QString::fromStdString(a->getTypeTag()) + ")",
                    a->getId());
        }
        if (animalBox->count() == 0)
            animalBox->addItem("(no animals for this client)", -1);
    };
    repopulateAnimals();
    form.addRow("Animal:", animalBox);

    connect(ownerBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int) { repopulateAnimals(); });

    // Date picker
    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Date:", dateEdit);

    // Services list with checkboxes
    QListWidget* svcList = new QListWidget(&dlg);
    svcList->setSelectionMode(QAbstractItemView::NoSelection);
    svcList->setMaximumHeight(150);
    for (Service* s : allSvcs) {
        QListWidgetItem* item = new QListWidgetItem(
            QString::fromStdString(s->getName()) +
            QString("  ($%1)").arg(s->getFinalPrice(), 0, 'f', 2));
        item->setData(Qt::UserRole, s->getId());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        svcList->addItem(item);
    }
    if (allSvcs.empty())
        svcList->addItem("(no services in catalog)");
    form.addRow("Services:", svcList);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int animalId = animalBox->currentData().toInt();
    if (animalId < 0) {
        QMessageBox::warning(this, "New Visit", "No valid animal selected.");
        return;
    }
    int ownerId = ownerBox->currentData().toInt();

    vector<int> serviceIds;
    for (int r = 0; r < svcList->count(); ++r) {
        QListWidgetItem* item = svcList->item(r);
        if (item->checkState() == Qt::Checked)
            serviceIds.push_back(item->data(Qt::UserRole).toInt());
    }

    if (serviceIds.empty()) {
        QMessageBox::warning(this, "No Services", "Please check at least one service.");
        return;
    }

    try {
        _svc.createVisit(animalId, ownerId,
                         dateEdit->date().toString("yyyy-MM-dd").toStdString(),
                         serviceIds);
        loadVisits();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void VisitsTab::onCancelVisit()
{
    int id = selectedVisitId();
    if (id < 0) {
        QMessageBox::information(this, "Cancel Visit", "Select a visit row first.");
        return;
    }

    int row = _visitTable->currentRow();
    QString animal = _visitTable->item(row, 2)->text();
    QString date   = _visitTable->item(row, 0)->text();

    auto reply = QMessageBox::question(this, "Cancel Visit",
        QString("Cancel the visit for \"%1\" on %2?").arg(animal, date),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _svc.cancelVisit(id);
        loadVisits();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
