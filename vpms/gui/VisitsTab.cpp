#include "VisitsTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>

VisitsTab::VisitsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void VisitsTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"ID", "Animal ID", "Owner ID", "Date", "Total Cost"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn    = new QPushButton("New Visit",     this);
    _cancelBtn = new QPushButton("Cancel Visit",  this);

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
    const auto& visits = _service.getAllVisits();
    _table->setRowCount((int)visits.size());
    for (int i = 0; i < (int)visits.size(); i++) {
        const Visit& v = visits[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(v.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::number(v.getAnimalId())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::number(v.getOwnerId())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(v.getDate())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(v.getTotalCost(), 'f', 2)));
    }
}

void VisitsTab::onAdd() {
    const auto& animals = _service.getAllAnimals();
    const auto& services = _service.getAllServices();

    if (animals.empty()) {
        QMessageBox::warning(this, "Error", "No animals registered. Add an animal first.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("New Visit");
    QFormLayout form(&dlg);

    QComboBox* animalBox = new QComboBox(&dlg);
    for (Animal* a : animals)
        animalBox->addItem(QString("[%1] %2 (Owner: %3)")
            .arg(a->getId()).arg(QString::fromStdString(a->getName())).arg(a->getOwnerId()),
            a->getId());
    form.addRow("Animal:", animalBox);

    QLineEdit* dateEdit = new QLineEdit(&dlg);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    form.addRow("Date:", dateEdit);

    QListWidget* serviceList = new QListWidget(&dlg);
    serviceList->setSelectionMode(QAbstractItemView::MultiSelection);
    serviceList->setMaximumHeight(120);
    for (Service* s : services) {
        QListWidgetItem* item = new QListWidgetItem(
            QString("[%1] %2 — $%3").arg(s->getId())
                .arg(QString::fromStdString(s->getName()))
                .arg(s->getFinalPrice(), 0, 'f', 2));
        item->setData(Qt::UserRole, s->getId());
        serviceList->addItem(item);
    }
    form.addRow("Services:", serviceList);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (dateEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, "Error", "Date cannot be empty."); return; }

    int animalId = animalBox->currentData().toInt();
    int ownerId  = _service.getAnimal(animalId)->getOwnerId();

    vector<int> serviceIds;
    for (QListWidgetItem* item : serviceList->selectedItems())
        serviceIds.push_back(item->data(Qt::UserRole).toInt());

    try {
        _service.createVisit(animalId, ownerId,
                             dateEdit->text().toStdString(), serviceIds);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void VisitsTab::onCancel() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Cancel Visit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Cancel Visit", "Cancel this visit?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.cancelVisit(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
