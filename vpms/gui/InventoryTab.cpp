#include "InventoryTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QBrush>
#include <QColor>

InventoryTab::InventoryTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void InventoryTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"ID", "Name", "Quantity", "Unit Price", "Category"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn      = new QPushButton("Add Item",  this);
    _stockInBtn  = new QPushButton("Stock In",  this);
    _stockOutBtn = new QPushButton("Stock Out", this);
    _deleteBtn   = new QPushButton("Delete",    this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_stockInBtn);
    btnLayout->addWidget(_stockOutBtn);
    btnLayout->addWidget(_deleteBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_addBtn,      &QPushButton::clicked, this, &InventoryTab::onAdd);
    connect(_stockInBtn,  &QPushButton::clicked, this, &InventoryTab::onStockIn);
    connect(_stockOutBtn, &QPushButton::clicked, this, &InventoryTab::onStockOut);
    connect(_deleteBtn,   &QPushButton::clicked, this, &InventoryTab::onDelete);
}

void InventoryTab::loadData() {
    const auto& items = _service.getAllInventory();
    _table->setRowCount((int)items.size());
    for (int i = 0; i < (int)items.size(); i++) {
        const InventoryItem& item = items[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(item.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(item.getName())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::number(item.getUnitPrice(), 'f', 2)));
        _table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(item.getCategory())));

        if (item.getQuantity() <= 10) {
            for (int col = 0; col < 5; col++)
                _table->item(i, col)->setBackground(QBrush(QColor(255, 220, 220)));
        }
    }
}

void InventoryTab::onAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Inventory Item");
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    form.addRow("Name:", nameEdit);

    QSpinBox* qtySpin = new QSpinBox(&dlg);
    qtySpin->setRange(0, 99999);
    form.addRow("Quantity:", qtySpin);

    QDoubleSpinBox* priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0.01, 99999.99);
    priceSpin->setDecimals(2);
    priceSpin->setValue(1.0);
    form.addRow("Unit Price:", priceSpin);

    QLineEdit* catEdit = new QLineEdit(&dlg);
    catEdit->setPlaceholderText("e.g. Medication, Equipment");
    form.addRow("Category:", catEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (nameEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, "Error", "Name cannot be empty."); return; }

    try {
        _service.addInventoryItem(nameEdit->text().toStdString(), qtySpin->value(),
                                  priceSpin->value(), catEdit->text().toStdString());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void InventoryTab::onStockIn() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Stock In", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    int qty = QInputDialog::getInt(this, "Stock In", "Quantity to add:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    try {
        _service.stockIn(id, qty);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void InventoryTab::onStockOut() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Stock Out", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    int qty = QInputDialog::getInt(this, "Stock Out", "Quantity to remove:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    try {
        _service.stockOut(id, qty);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void InventoryTab::onDelete() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Delete", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Delete", "Delete this item?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.removeInventoryItem(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
