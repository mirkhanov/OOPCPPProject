#include "ServicesTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>

ServicesTab::ServicesTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void ServicesTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"ID", "Type", "Name", "Base Price", "Description"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn    = new QPushButton("Add",    this);
    _deleteBtn = new QPushButton("Delete", this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_deleteBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_addBtn,    &QPushButton::clicked, this, &ServicesTab::onAdd);
    connect(_deleteBtn, &QPushButton::clicked, this, &ServicesTab::onDelete);
}

void ServicesTab::loadData() {
    const auto& services = _service.getAllServices();
    _table->setRowCount((int)services.size());
    for (int i = 0; i < (int)services.size(); i++) {
        Service* s = services[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(s->getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s->getTypeTag())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s->getName())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::number(s->getFinalPrice(), 'f', 2)));
        _table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(s->getDescription())));
    }
}

void ServicesTab::onAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Service");
    QFormLayout form(&dlg);

    QComboBox* typeBox = new QComboBox(&dlg);
    typeBox->addItems({"Consultation", "Vaccination", "Surgery", "Grooming"});
    form.addRow("Type:", typeBox);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    form.addRow("Name:", nameEdit);

    QDoubleSpinBox* priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0.01, 99999.99);
    priceSpin->setDecimals(2);
    priceSpin->setValue(50.0);
    form.addRow("Price:", priceSpin);

    QLineEdit* extraEdit = new QLineEdit(&dlg);
    extraEdit->setPlaceholderText("e.g. vaccine name, complexity, breed");
    form.addRow("Extra info:", extraEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, "Error", "Name cannot be empty."); return; }

    try {
        _service.addService(typeBox->currentText().toLower().toStdString(),
                            nameEdit->text().toStdString(),
                            priceSpin->value(),
                            extraEdit->text().toStdString());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void ServicesTab::onDelete() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Delete", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Delete", "Delete this service?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.removeService(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
