#include "ServicesTab.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QStackedWidget>

using namespace std;

ServicesTab::ServicesTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _svc(service)
{
    setupUI();
    loadData();
}

void ServicesTab::setupUI()
{
    // --- top bar ---
    QLabel* typeLabel = new QLabel("Type:", this);
    _typeFilter = new QComboBox(this);
    _typeFilter->addItems({"All", "Consultation", "Vaccination", "Surgery", "Grooming"});

    _addBtn    = new QPushButton("Add Service", this);
    _deleteBtn = new QPushButton("Delete",      this);

    QHBoxLayout* topBar = new QHBoxLayout;
    topBar->addWidget(typeLabel);
    topBar->addWidget(_typeFilter);
    topBar->addStretch();
    topBar->addWidget(_addBtn);
    topBar->addWidget(_deleteBtn);

    // --- table ---
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels(
        {"Type", "Name", "Base Price ($)", "Final Price ($)", "Description"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setAlternatingRowColors(true);
    _table->horizontalHeader()->setStretchLastSection(true);
    _table->verticalHeader()->hide();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(_table);

    connect(_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ServicesTab::onFilterChanged);
    connect(_addBtn,    &QPushButton::clicked, this, &ServicesTab::onAdd);
    connect(_deleteBtn, &QPushButton::clicked, this, &ServicesTab::onDelete);
}

void ServicesTab::loadData()
{
    const auto& services = _svc.getAllServices();
    QString filterTag = _typeFilter->currentText().toUpper();
    if (filterTag == "ALL") filterTag = "";

    // Collect matching
    vector<Service*> shown;
    for (Service* s : services) {
        QString tag = QString::fromStdString(s->getTypeTag()).toUpper();
        if (filterTag.isEmpty() || tag == filterTag)
            shown.push_back(s);
    }

    _table->setRowCount((int)shown.size());
    for (int i = 0; i < (int)shown.size(); ++i) {
        Service* s = shown[i];
        QTableWidgetItem* typeItem = new QTableWidgetItem(
            QString::fromStdString(s->getTypeTag()));
        typeItem->setData(Qt::UserRole, s->getId());
        _table->setItem(i, 0, typeItem);
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s->getName())));
        _table->setItem(i, 2, new QTableWidgetItem(
            QString("$%1").arg(s->getBasePrice(), 0, 'f', 2)));
        _table->setItem(i, 3, new QTableWidgetItem(
            QString("$%1").arg(s->getFinalPrice(), 0, 'f', 2)));
        _table->setItem(i, 4, new QTableWidgetItem(
            QString::fromStdString(s->getDescription())));
    }

    _table->resizeColumnsToContents();
    _table->horizontalHeader()->setStretchLastSection(true);
}

void ServicesTab::onFilterChanged()
{
    loadData();
}

void ServicesTab::onAdd()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Add Service");
    dlg.setMinimumWidth(420);
    QFormLayout form(&dlg);

    // Type selector
    QComboBox* typeBox = new QComboBox(&dlg);
    typeBox->addItems({"Consultation", "Vaccination", "Surgery", "Grooming"});
    form.addRow("Type:", typeBox);

    // Name
    QLineEdit* nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("Service name");
    form.addRow("Name:", nameEdit);

    // Base price
    QDoubleSpinBox* priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0.01, 99999.99);
    priceSpin->setDecimals(2);
    priceSpin->setValue(50.00);
    priceSpin->setPrefix("$ ");
    form.addRow("Base Price:", priceSpin);

    // --- Extra widgets (one set per type) ---

    // Consultation: duration spin
    QLabel*   consultLabel = new QLabel("Duration (min):", &dlg);
    QSpinBox* consultSpin  = new QSpinBox(&dlg);
    consultSpin->setRange(5, 480);
    consultSpin->setValue(30);
    form.addRow(consultLabel, consultSpin);

    // Vaccination: vaccine type line edit
    QLabel*    vaccLabel = new QLabel("Vaccine type:", &dlg);
    QLineEdit* vaccEdit  = new QLineEdit(&dlg);
    vaccEdit->setPlaceholderText("e.g. Rabies, Parvovirus");
    form.addRow(vaccLabel, vaccEdit);

    // Surgery: surgery type + anesthesia checkbox
    QLabel*    surgTypeLabel = new QLabel("Surgery type:", &dlg);
    QLineEdit* surgTypeEdit  = new QLineEdit(&dlg);
    surgTypeEdit->setPlaceholderText("e.g. Spay, Neuter");
    form.addRow(surgTypeLabel, surgTypeEdit);

    QLabel*    anesthLabel = new QLabel("Requires anesthesia:", &dlg);
    QCheckBox* anesthBox   = new QCheckBox(&dlg);
    anesthBox->setChecked(true);
    form.addRow(anesthLabel, anesthBox);

    // Grooming: level combo
    QLabel*    groomLabel = new QLabel("Level:", &dlg);
    QComboBox* groomBox   = new QComboBox(&dlg);
    groomBox->addItems({"Basic", "Standard", "Premium"});
    form.addRow(groomLabel, groomBox);

    // Helper: show/hide correct extra rows
    auto updateExtras = [&](const QString& type) {
        bool isConsult  = (type == "Consultation");
        bool isVacc     = (type == "Vaccination");
        bool isSurg     = (type == "Surgery");
        bool isGroom    = (type == "Grooming");

        consultLabel->setVisible(isConsult);
        consultSpin->setVisible(isConsult);
        vaccLabel->setVisible(isVacc);
        vaccEdit->setVisible(isVacc);
        surgTypeLabel->setVisible(isSurg);
        surgTypeEdit->setVisible(isSurg);
        anesthLabel->setVisible(isSurg);
        anesthBox->setVisible(isSurg);
        groomLabel->setVisible(isGroom);
        groomBox->setVisible(isGroom);
    };

    // Initial state
    updateExtras(typeBox->currentText());

    connect(typeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int) { updateExtras(typeBox->currentText()); });

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Name cannot be empty.");
        return;
    }

    // Build extra string based on type
    QString type  = typeBox->currentText();
    QString extra;

    if (type == "Consultation") {
        extra = QString::number(consultSpin->value());                       // duration minutes
    } else if (type == "Vaccination") {
        extra = vaccEdit->text().trimmed();
    } else if (type == "Surgery") {
        extra = surgTypeEdit->text().trimmed() + "|" +
                QString(anesthBox->isChecked() ? "1" : "0");
    } else if (type == "Grooming") {
        extra = groomBox->currentText();                                     // Basic/Standard/Premium
    }

    try {
        _svc.addService(type.toUpper().toStdString(),
                        nameEdit->text().trimmed().toStdString(),
                        priceSpin->value(),
                        extra.toStdString());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ServicesTab::onDelete()
{
    int row = _table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Delete", "Select a service row first.");
        return;
    }

    QTableWidgetItem* item = _table->item(row, 0);
    if (!item) return;
    int id = item->data(Qt::UserRole).toInt();
    QString name = _table->item(row, 1)->text();

    auto reply = QMessageBox::question(this, "Delete Service",
        QString("Delete service \"%1\"?").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _svc.removeService(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Cannot Delete",
            QString("Service is in use or an error occurred:\n%1").arg(e.what()));
    }
}
