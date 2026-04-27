#include "AnimalsTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>

AnimalsTab::AnimalsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void AnimalsTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(6);
    _table->setHorizontalHeaderLabels({"ID", "Type", "Name", "Age", "Owner", "Species Info"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn    = new QPushButton("Add",    this);
    _editBtn   = new QPushButton("Edit",   this);
    _deleteBtn = new QPushButton("Delete", this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_editBtn);
    btnLayout->addWidget(_deleteBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_addBtn,    &QPushButton::clicked, this, &AnimalsTab::onAdd);
    connect(_editBtn,   &QPushButton::clicked, this, &AnimalsTab::onEdit);
    connect(_deleteBtn, &QPushButton::clicked, this, &AnimalsTab::onDelete);
}

void AnimalsTab::loadData() {
    const auto& animals = _service.getAllAnimals();
    const auto& owners  = _service.getAllOwners();
    _table->setRowCount((int)animals.size());
    for (int i = 0; i < (int)animals.size(); i++) {
        Animal* a = animals[i];
        QString ownerName = QString::number(a->getOwnerId());
        for (const Owner& o : owners)
            if (o.getId() == a->getOwnerId()) { ownerName = QString::fromStdString(o.getName()); break; }
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(a->getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(a->getTypeTag())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(a->getName())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::number(a->getAge())));
        _table->setItem(i, 4, new QTableWidgetItem(ownerName));
        _table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(a->getSpeciesInfo())));
    }
}

void AnimalsTab::onAdd() {
    const auto& owners = _service.getAllOwners();
    if (owners.empty()) {
        QMessageBox::warning(this, "Error", "No owners registered. Add an owner first.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Add Animal");
    dlg.setMinimumWidth(320);
    QFormLayout form(&dlg);

    QComboBox* typeBox = new QComboBox(&dlg);
    typeBox->addItems({"Dog", "Cat", "Bird", "Reptile"});
    form.addRow("Type:", typeBox);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("e.g. Rex");
    form.addRow("Name:", nameEdit);

    QSpinBox* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1, 100);
    ageSpin->setSuffix(" years");
    form.addRow("Age:", ageSpin);

    QComboBox* ownerBox = new QComboBox(&dlg);
    for (const Owner& o : owners)
        ownerBox->addItem(QString::fromStdString(o.getName()), o.getId());
    form.addRow("Owner:", ownerBox);

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
                           ageSpin->value(),
                           ownerBox->currentData().toInt());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void AnimalsTab::onEdit() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Edit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    QDialog dlg(this);
    dlg.setWindowTitle("Edit Animal");
    dlg.setMinimumWidth(300);
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(_table->item(row, 2)->text(), &dlg);
    form.addRow("Name:", nameEdit);

    QSpinBox* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1, 100);
    ageSpin->setSuffix(" years");
    ageSpin->setValue(_table->item(row, 3)->text().toInt());
    form.addRow("Age:", ageSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) return;

    try {
        Animal* a = _service.getAnimal(id);
        a->setName(nameEdit->text().trimmed().toStdString());
        a->setAge(ageSpin->value());
        _service.updateAnimal(a);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void AnimalsTab::onDelete() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Delete", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();
    QString name = _table->item(row, 2)->text();

    auto reply = QMessageBox::question(this, "Delete",
        QString("Delete animal \"%1\"?").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.removeAnimal(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
