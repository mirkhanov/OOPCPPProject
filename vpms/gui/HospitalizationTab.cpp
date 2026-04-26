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
        {"ID", "Animal ID", "Ward", "Admit Date", "Daily Rate", "Status", "Total Bill"});
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
    _table->setRowCount((int)records.size());
    for (int i = 0; i < (int)records.size(); i++) {
        const HospitalizationRecord& r = records[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(r.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::number(r.getAnimalId())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r.getWard())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(r.getAdmitDate())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(r.getDailyRate(), 'f', 2)));
        _table->setItem(i, 5, new QTableWidgetItem(r.isActive() ? "Active" : "Discharged"));
        _table->setItem(i, 6, new QTableWidgetItem(
            r.isActive() ? "-" : QString::number(r.getTotalBill(), 'f', 2)));

        if (r.isActive()) {
            for (int col = 0; col < 7; col++)
                _table->item(i, col)->setBackground(QBrush(QColor(220, 255, 220)));
        }
    }
}

void HospitalizationTab::onAdmit() {
    QDialog dlg(this);
    dlg.setWindowTitle("Admit Animal");
    QFormLayout form(&dlg);

    QSpinBox* animalSpin = new QSpinBox(&dlg);
    animalSpin->setRange(1, 99999);
    form.addRow("Animal ID:", animalSpin);

    QLineEdit* wardEdit = new QLineEdit(&dlg);
    wardEdit->setPlaceholderText("e.g. Ward A");
    form.addRow("Ward:", wardEdit);

    QLineEdit* dateEdit = new QLineEdit(&dlg);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    form.addRow("Admit Date:", dateEdit);

    QDoubleSpinBox* rateSpin = new QDoubleSpinBox(&dlg);
    rateSpin->setRange(0.01, 99999.99);
    rateSpin->setDecimals(2);
    rateSpin->setValue(100.0);
    form.addRow("Daily Rate:", rateSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (wardEdit->text().trimmed().isEmpty() || dateEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Ward and date cannot be empty.");
        return;
    }

    try {
        _service.admitAnimal(animalSpin->value(), wardEdit->text().toStdString(),
                             dateEdit->text().toStdString(), rateSpin->value());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void HospitalizationTab::onDischarge() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Discharge", "Select a row first."); return; }
    if (_table->item(row, 5)->text() != "Active") {
        QMessageBox::information(this, "Discharge", "This animal is already discharged.");
        return;
    }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    QString date = QInputDialog::getText(this, "Discharge", "Discharge Date (YYYY-MM-DD):",
                                         QLineEdit::Normal, "", &ok);
    if (!ok || date.trimmed().isEmpty()) return;

    try {
        double bill = _service.dischargeAnimal(id, date.toStdString());
        loadData();
        QMessageBox::information(this, "Discharged",
            QString("Total bill: $%1").arg(bill, 0, 'f', 2));
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void HospitalizationTab::onAddService() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Add Service", "Select a row first."); return; }
    if (_table->item(row, 5)->text() != "Active") {
        QMessageBox::information(this, "Add Service", "Cannot add service to discharged record.");
        return;
    }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    int serviceId = QInputDialog::getInt(this, "Add Service", "Service ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    try {
        _service.addServiceToHospitalization(id, serviceId);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
