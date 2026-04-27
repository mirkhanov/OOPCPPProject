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
    _table->setHorizontalHeaderLabels({"ID", "Type", "Name", "Age", "Owner ID", "Species Info"});
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
    _table->setRowCount((int)animals.size());
    for (int i = 0; i < (int)animals.size(); i++) {
        Animal* a = animals[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(a->getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(a->getTypeTag())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(a->getName())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::number(a->getAge())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(a->getOwnerId())));
        _table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(a->getSpeciesInfo())));
    }
}

void AnimalsTab::onAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Animal");
    QFormLayout form(&dlg);

    QComboBox* typeBox = new QComboBox(&dlg);
    typeBox->addItems({"Dog", "Cat", "Bird", "Reptile"});
    form.addRow("Type:", typeBox);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    form.addRow("Name:", nameEdit);

    QSpinBox* ageSpin = new QSpinBox(&dlg);
    ageSpin->setRange(1, 100);
    form.addRow("Age:", ageSpin);

    QSpinBox* ownerSpin = new QSpinBox(&dlg);
    ownerSpin->setRange(1, 99999);
    form.addRow("Owner ID:", ownerSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, "Error", "Name cannot be empty."); return; }

    try {
        _service.addAnimal(typeBox->currentText().toUpper().toStdString(),
                           nameEdit->text().toStdString(),
                           ageSpin->value(),
                           ownerSpin->value());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void AnimalsTab::onEdit() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Edit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    QString name = QInputDialog::getText(this, "Edit Animal", "Name:",
        QLineEdit::Normal, _table->item(row, 2)->text(), &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    int age = QInputDialog::getInt(this, "Edit Animal", "Age:",
        _table->item(row, 3)->text().toInt(), 1, 100, 1, &ok);
    if (!ok) return;

    try {
        Animal* a = _service.getAnimal(id);
        a->setName(name.toStdString());
        a->setAge(age);
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

    auto reply = QMessageBox::question(this, "Delete", "Delete this animal?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.removeAnimal(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
