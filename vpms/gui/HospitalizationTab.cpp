#include "HospitalizationTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QBrush>
#include <QColor>

HospitalizationTab::HospitalizationTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void HospitalizationTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(7);
    _table->setHorizontalHeaderLabels(
        {"ID", "Animal", "Ward", "Admit Date", "Daily Rate ($)", "Status", "Total Bill ($)"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _admitBtn      = new QPushButton("Admit",       this);
    _dischargeBtn  = new QPushButton("Discharge",   this);
    _addServiceBtn = new QPushButton("Add Service", this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_admitBtn);
    btnLayout->addWidget(_dischargeBtn);
    btnLayout->addWidget(_addServiceBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_admitBtn,      &QPushButton::clicked, this, &HospitalizationTab::onAdmit);
    connect(_dischargeBtn,  &QPushButton::clicked, this, &HospitalizationTab::onDischarge);
    connect(_addServiceBtn, &QPushButton::clicked, this, &HospitalizationTab::onAddService);
}

void HospitalizationTab::loadData() {
    const auto& records = _service.getAllHospitalizations();
    const auto& animals = _service.getAllAnimals();
    _table->setRowCount((int)records.size());
    for (int i = 0; i < (int)records.size(); i++) {
        const HospitalizationRecord& r = records[i];
        QString animalName = QString::number(r.getAnimalId());
        for (Animal* a : animals)
            if (a->getId() == r.getAnimalId()) { animalName = QString::fromStdString(a->getName()); break; }
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(r.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(animalName));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r.getWard())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(r.getAdmitDate())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(r.getDailyRate(), 'f', 2)));
        _table->setItem(i, 5, new QTableWidgetItem(r.isActive() ? "Active" : "Discharged"));
        _table->setItem(i, 6, new QTableWidgetItem(
            r.isActive() ? "—" : QString::number(r.getTotalBill(), 'f', 2)));

        if (r.isActive()) {
            for (int col = 0; col < 7; col++)
                _table->item(i, col)->setBackground(QBrush(QColor(220, 255, 220)));
        }
    }
}

void HospitalizationTab::onAdmit() {
    const auto& animals = _service.getAllAnimals();
    if (animals.empty()) {
        QMessageBox::warning(this, "Error", "No animals registered."); return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Admit Animal");
    dlg.setMinimumWidth(320);
    QFormLayout form(&dlg);

    QComboBox* animalBox = new QComboBox(&dlg);
    for (Animal* a : animals)
        animalBox->addItem(QString::fromStdString(a->getName()) +
                           " (" + QString::fromStdString(a->getTypeTag()) + ")", a->getId());
    form.addRow("Animal:", animalBox);

    QLineEdit* wardEdit = new QLineEdit(&dlg);
    wardEdit->setPlaceholderText("e.g. Ward A");
    form.addRow("Ward:", wardEdit);

    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Admit Date:", dateEdit);

    QDoubleSpinBox* rateSpin = new QDoubleSpinBox(&dlg);
    rateSpin->setRange(1.0, 99999.99);
    rateSpin->setDecimals(2);
    rateSpin->setValue(100.0);
    rateSpin->setPrefix("$ ");
    form.addRow("Daily Rate:", rateSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (wardEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Ward cannot be empty."); return;
    }

    try {
        _service.admitAnimal(animalBox->currentData().toInt(),
                             wardEdit->text().trimmed().toStdString(),
                             dateEdit->date().toString("yyyy-MM-dd").toStdString(),
                             rateSpin->value());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void HospitalizationTab::onDischarge() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Discharge", "Select a row first."); return; }
    if (_table->item(row, 5)->text() != "Active") {
        QMessageBox::information(this, "Discharge", "This animal is already discharged."); return;
    }

    int id = _table->item(row, 0)->text().toInt();
    QString animalName = _table->item(row, 1)->text();

    QDialog dlg(this);
    dlg.setWindowTitle("Discharge " + animalName);
    dlg.setMinimumWidth(280);
    QFormLayout form(&dlg);

    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Discharge Date:", dateEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    try {
        double bill = _service.dischargeAnimal(id,
            dateEdit->date().toString("yyyy-MM-dd").toStdString());
        loadData();
        QMessageBox::information(this, "Discharged",
            QString("%1 has been discharged.\nTotal bill: $%2")
                .arg(animalName).arg(bill, 0, 'f', 2));
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void HospitalizationTab::onAddService() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Add Service", "Select a row first."); return; }
    if (_table->item(row, 5)->text() != "Active") {
        QMessageBox::information(this, "Add Service", "Cannot add service to discharged record."); return;
    }

    const auto& services = _service.getAllServices();
    if (services.empty()) {
        QMessageBox::warning(this, "Error", "No services in catalog."); return;
    }

    int id = _table->item(row, 0)->text().toInt();

    QDialog dlg(this);
    dlg.setWindowTitle("Add Service");
    dlg.setMinimumWidth(300);
    QFormLayout form(&dlg);

    QComboBox* svcBox = new QComboBox(&dlg);
    for (Service* s : services)
        svcBox->addItem(QString::fromStdString(s->getName()) +
                        QString(" ($%1)").arg(s->getFinalPrice(), 0, 'f', 2), s->getId());
    form.addRow("Service:", svcBox);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    try {
        _service.addServiceToHospitalization(id, svcBox->currentData().toInt());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
