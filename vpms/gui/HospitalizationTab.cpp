#include "HospitalizationTab.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDate>
#include <QBrush>
#include <QColor>
#include <QGroupBox>
#include <QSplitter>

using namespace std;

HospitalizationTab::HospitalizationTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _svc(service)
{
    setupUI();
    loadTables();
}

void HospitalizationTab::setupUI()
{
    // --- Active Patients table ---
    _activeTable = new QTableWidget(this);
    _activeTable->setColumnCount(5);
    _activeTable->setHorizontalHeaderLabels(
        {"Animal", "Ward", "Admitted", "Daily Rate ($)", "Bill So Far ($)"});
    _activeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _activeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _activeTable->setAlternatingRowColors(true);
    _activeTable->horizontalHeader()->setStretchLastSection(true);
    _activeTable->verticalHeader()->hide();

    // --- Discharged table ---
    _dischargedTable = new QTableWidget(this);
    _dischargedTable->setColumnCount(5);
    _dischargedTable->setHorizontalHeaderLabels(
        {"Animal", "Ward", "Admitted", "Discharged", "Final Bill ($)"});
    _dischargedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _dischargedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _dischargedTable->setAlternatingRowColors(true);
    _dischargedTable->horizontalHeader()->setStretchLastSection(true);
    _dischargedTable->verticalHeader()->hide();

    // --- Group boxes for clear labeling ---
    QGroupBox* activeGroup = new QGroupBox("Active Patients", this);
    QVBoxLayout* activeLayout = new QVBoxLayout(activeGroup);
    activeLayout->addWidget(_activeTable);

    QGroupBox* dischargedGroup = new QGroupBox("Discharged", this);
    QVBoxLayout* dischargedLayout = new QVBoxLayout(dischargedGroup);
    dischargedLayout->addWidget(_dischargedTable);

    // --- Buttons ---
    _admitBtn     = new QPushButton("Admit Animal", this);
    _dischargeBtn = new QPushButton("Discharge",    this);
    _dischargeBtn->setEnabled(false);

    QHBoxLayout* btnBar = new QHBoxLayout;
    btnBar->addWidget(_admitBtn);
    btnBar->addWidget(_dischargeBtn);
    btnBar->addStretch();

    // --- Splitter so both tables share vertical space ---
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(activeGroup);
    splitter->addWidget(dischargedGroup);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnBar);
    mainLayout->addWidget(splitter);

    connect(_admitBtn,     &QPushButton::clicked, this, &HospitalizationTab::onAdmit);
    connect(_dischargeBtn, &QPushButton::clicked, this, &HospitalizationTab::onDischarge);
    connect(_activeTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &HospitalizationTab::onActiveSelectionChanged);
}

void HospitalizationTab::loadTables()
{
    const auto& all     = _svc.getAllHospitalizations();
    const auto& animals = _svc.getAllAnimals();

    // Helper: resolve animal name
    auto animalName = [&](int id) -> QString {
        for (Animal* a : animals)
            if (a->getId() == id)
                return QString::fromStdString(a->getName()) +
                       " (" + QString::fromStdString(a->getTypeTag()) + ")";
        return QString::number(id);
    };

    QDate today = QDate::currentDate();
    QColor activeColor(0xff, 0xfd, 0xe7); // #fffde7 light yellow

    // Split into active / discharged
    vector<const HospitalizationRecord*> active, discharged;
    for (const HospitalizationRecord& r : all)
        (r.isActive() ? active : discharged).push_back(&r);

    // --- Active table ---
    _activeTable->setRowCount((int)active.size());
    for (int i = 0; i < (int)active.size(); ++i) {
        const HospitalizationRecord* r = active[i];

        // Calculate approximate days in hospital
        QDate admitDate = QDate::fromString(
            QString::fromStdString(r->getAdmitDate()), "yyyy-MM-dd");
        int days = admitDate.isValid() ? admitDate.daysTo(today) : 0;
        if (days < 0) days = 0;
        double billSoFar = days * r->getDailyRate();

        QTableWidgetItem* nameItem = new QTableWidgetItem(animalName(r->getAnimalId()));
        nameItem->setData(Qt::UserRole, r->getId());
        _activeTable->setItem(i, 0, nameItem);
        _activeTable->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(r->getWard())));
        _activeTable->setItem(i, 2, new QTableWidgetItem(
            QString::fromStdString(r->getAdmitDate())));
        _activeTable->setItem(i, 3, new QTableWidgetItem(
            QString("$%1").arg(r->getDailyRate(), 0, 'f', 2)));
        _activeTable->setItem(i, 4, new QTableWidgetItem(
            QString("$%1  (%2 days)").arg(billSoFar, 0, 'f', 2).arg(days)));

        // Highlight light yellow with black text for contrast in dark mode
        for (int col = 0; col < 5; ++col) {
            if (_activeTable->item(i, col)) {
                _activeTable->item(i, col)->setBackground(QBrush(activeColor));
                _activeTable->item(i, col)->setForeground(QBrush(Qt::black));
            }
        }
    }
    _activeTable->resizeColumnsToContents();
    _activeTable->horizontalHeader()->setStretchLastSection(true);

    // --- Discharged table ---
    _dischargedTable->setRowCount((int)discharged.size());
    for (int i = 0; i < (int)discharged.size(); ++i) {
        const HospitalizationRecord* r = discharged[i];

        QTableWidgetItem* nameItem = new QTableWidgetItem(animalName(r->getAnimalId()));
        nameItem->setData(Qt::UserRole, r->getId());
        _dischargedTable->setItem(i, 0, nameItem);
        _dischargedTable->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(r->getWard())));
        _dischargedTable->setItem(i, 2, new QTableWidgetItem(
            QString::fromStdString(r->getAdmitDate())));
        _dischargedTable->setItem(i, 3, new QTableWidgetItem(
            QString::fromStdString(r->getDischargeDate())));
        _dischargedTable->setItem(i, 4, new QTableWidgetItem(
            QString("$%1").arg(r->getTotalBill(), 0, 'f', 2)));
    }
    _dischargedTable->resizeColumnsToContents();
    _dischargedTable->horizontalHeader()->setStretchLastSection(true);

    // Update discharge button state
    _dischargeBtn->setEnabled(_activeTable->currentRow() >= 0);
}

void HospitalizationTab::onActiveSelectionChanged()
{
    _dischargeBtn->setEnabled(_activeTable->currentRow() >= 0);
}

void HospitalizationTab::onAdmit()
{
    const auto& animals = _svc.getAllAnimals();
    if (animals.empty()) {
        QMessageBox::warning(this, "Admit Animal", "No animals registered. Add a client first.");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Admit Animal");
    dlg.setMinimumWidth(360);
    QFormLayout form(&dlg);

    QComboBox* animalBox = new QComboBox(&dlg);
    for (Animal* a : animals)
        animalBox->addItem(
            QString::fromStdString(a->getName()) +
            " (" + QString::fromStdString(a->getTypeTag()) + ")",
            a->getId());
    form.addRow("Animal:", animalBox);

    QLineEdit* wardEdit = new QLineEdit(&dlg);
    wardEdit->setPlaceholderText("e.g. Ward A, ICU");
    form.addRow("Ward:", wardEdit);

    QDateEdit* dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    form.addRow("Admit Date:", dateEdit);

    QDoubleSpinBox* rateSpin = new QDoubleSpinBox(&dlg);
    rateSpin->setRange(1.00, 99999.99);
    rateSpin->setDecimals(2);
    rateSpin->setValue(150.00);
    rateSpin->setPrefix("$ ");
    form.addRow("Daily Rate:", rateSpin);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    if (wardEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Ward cannot be empty.");
        return;
    }

    try {
        _svc.admitAnimal(animalBox->currentData().toInt(),
                         wardEdit->text().trimmed().toStdString(),
                         dateEdit->date().toString("yyyy-MM-dd").toStdString(),
                         rateSpin->value());
        loadTables();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void HospitalizationTab::onDischarge()
{
    int row = _activeTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Discharge", "Select an active patient row first.");
        return;
    }

    QTableWidgetItem* item = _activeTable->item(row, 0);
    if (!item) return;
    int    recordId   = item->data(Qt::UserRole).toInt();
    QString animalName = item->text();

    QDialog dlg(this);
    dlg.setWindowTitle("Discharge " + animalName);
    dlg.setMinimumWidth(300);
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
        double bill = _svc.dischargeAnimal(recordId,
            dateEdit->date().toString("yyyy-MM-dd").toStdString());
        loadTables();
        QMessageBox::information(this, "Discharged",
            QString("%1 has been discharged.\nTotal bill: $%2")
                .arg(animalName).arg(bill, 0, 'f', 2));
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
