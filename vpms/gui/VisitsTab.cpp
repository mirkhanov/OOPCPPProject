#include "VisitsTab.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QListWidget>
#include <QDateEdit>
#include <QDate>

VisitsTab::VisitsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void VisitsTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"ID", "Animal", "Owner", "Date", "Total ($)"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn    = new QPushButton("New Visit",    this);
    _cancelBtn = new QPushButton("Cancel Visit", this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_cancelBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_addBtn,    &QPushButton::clicked, this, &VisitsTab::onAdd);
    connect(_cancelBtn, &QPushButton::clicked, this, &VisitsTab::onCancel);
}

void VisitsTab::loadData() {
    const auto& visits  = _service.getAllVisits();
    const auto& animals = _service.getAllAnimals();
    const auto& owners  = _service.getAllOwners();
    _table->setRowCount((int)visits.size());
    for (int i = 0; i < (int)visits.size(); i++) {
        const Visit& v = visits[i];
        QString animalName = QString::number(v.getAnimalId());
        QString ownerName  = QString::number(v.getOwnerId());
        for (Animal* a : animals)
            if (a->getId() == v.getAnimalId()) { animalName = QString::fromStdString(a->getName()); break; }
        for (const Owner& o : owners)
            if (o.getId() == v.getOwnerId()) { ownerName = QString::fromStdString(o.getName()); break; }
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(v.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(animalName));
        _table->setItem(i, 2, new QTableWidgetItem(ownerName));
        _table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(v.getDate())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(v.getTotalCost(), 'f', 2)));
    }
}

void VisitsTab::onAdd() {
    const auto& animals  = _service.getAllAnimals();
    const auto& services = _service.getAllServices();

    if (animals.empty()) {
        QMessageBox::warning(this, "Error", "No animals registered. Add a client first.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("New Visit");
    dlg.setMinimumWidth(360);
    QFormLayout form(&dlg);

    QComboBox* animalBox = new QComboBox(&dlg);
    for (Animal* a : animals) {
        QString ownerName;
        for (const Owner& o : _service.getAllOwners())
            if (o.getId() == a->getOwnerId()) { ownerName = QString::fromStdString(o.getName()); break; }
        animalBox->addItem(
            QString::fromStdString(a->getName()) + " (" +
            QString::fromStdString(a->getTypeTag()) + ") — " + ownerName,
            a->getId());
    }
    form.addRow("Animal:", animalBox);

    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Date:", dateEdit);

    QListWidget* serviceList = new QListWidget(&dlg);
    serviceList->setSelectionMode(QAbstractItemView::MultiSelection);
    serviceList->setMaximumHeight(130);
    for (Service* s : services) {
        QListWidgetItem* item = new QListWidgetItem(
            QString::fromStdString(s->getName()) +
            QString("  —  $%1").arg(s->getFinalPrice(), 0, 'f', 2));
        item->setData(Qt::UserRole, s->getId());
        serviceList->addItem(item);
    }
    if (services.empty()) serviceList->addItem("(no services in catalog)");
    form.addRow("Services:", serviceList);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int animalId = animalBox->currentData().toInt();
    int ownerId  = _service.getAnimal(animalId)->getOwnerId();

    vector<int> serviceIds;
    for (QListWidgetItem* item : serviceList->selectedItems())
        serviceIds.push_back(item->data(Qt::UserRole).toInt());

    try {
        _service.createVisit(animalId, ownerId,
                             dateEdit->date().toString("yyyy-MM-dd").toStdString(),
                             serviceIds);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void VisitsTab::onCancel() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Cancel Visit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();
    QString animal = _table->item(row, 1)->text();

    auto reply = QMessageBox::question(this, "Cancel Visit",
        QString("Cancel visit for \"%1\"?").arg(animal),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.cancelVisit(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
