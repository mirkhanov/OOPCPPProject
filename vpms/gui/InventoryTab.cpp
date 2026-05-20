#include "InventoryTab.h"
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

using namespace std;

InventoryTab::InventoryTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _svc(service)
{
    setupUI();
    loadItems();
}

void InventoryTab::setupUI()
{
    // --- top button bar ---
    _addBtn      = new QPushButton("Add Item",  this);
    _stockInBtn  = new QPushButton("Stock In",  this);
    _stockOutBtn = new QPushButton("Stock Out", this);
    _deleteBtn   = new QPushButton("Delete",    this);

    QHBoxLayout* btnBar = new QHBoxLayout;
    btnBar->addWidget(_addBtn);
    btnBar->addWidget(_stockInBtn);
    btnBar->addWidget(_stockOutBtn);
    btnBar->addWidget(_deleteBtn);
    btnBar->addStretch();

    // --- table ---
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels(
        {"Name", "Category", "Qty", "Unit Price ($)", "Total Value ($)"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setAlternatingRowColors(true);
    _table->horizontalHeader()->setStretchLastSection(true);
    _table->verticalHeader()->hide();

    // --- bottom total label ---
    _totalLabel = new QLabel("Total value: $0.00", this);
    _totalLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout* bottomBar = new QHBoxLayout;
    bottomBar->addStretch();
    bottomBar->addWidget(_totalLabel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnBar);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(bottomBar);

    connect(_addBtn,      &QPushButton::clicked, this, &InventoryTab::onAdd);
    connect(_stockInBtn,  &QPushButton::clicked, this, &InventoryTab::onStockIn);
    connect(_stockOutBtn, &QPushButton::clicked, this, &InventoryTab::onStockOut);
    connect(_deleteBtn,   &QPushButton::clicked, this, &InventoryTab::onDelete);
}

void InventoryTab::loadItems()
{
    const auto& items = _svc.getAllInventory();
    _table->setRowCount((int)items.size());

    double grandTotal = 0.0;

    for (int i = 0; i < (int)items.size(); ++i) {
        const InventoryItem& it = items[i];
        double rowTotal = it.getQuantity() * it.getUnitPrice();
        grandTotal += rowTotal;

        // Col 0 stores item ID in UserRole
        QTableWidgetItem* nameItem = new QTableWidgetItem(
            QString::fromStdString(it.getName()));
        nameItem->setData(Qt::UserRole, it.getId());
        _table->setItem(i, 0, nameItem);

        _table->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(it.getCategory())));

        QTableWidgetItem* qtyItem = new QTableWidgetItem(
            QString::number(it.getQuantity()));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        _table->setItem(i, 2, qtyItem);

        _table->setItem(i, 3, new QTableWidgetItem(
            QString("$%1").arg(it.getUnitPrice(), 0, 'f', 2)));

        _table->setItem(i, 4, new QTableWidgetItem(
            QString("$%1").arg(rowTotal, 0, 'f', 2)));

        // Highlight low-stock row (qty <= 10)
        if (it.getQuantity() <= 10) {
            QColor bg(0xff, 0xcc, 0xcc);   // #ffcccc
            QColor fg(0x8b, 0x00, 0x00);   // dark red
            for (int col = 0; col < 5; ++col) {
                if (_table->item(i, col)) {
                    _table->item(i, col)->setBackground(QBrush(bg));
                    _table->item(i, col)->setForeground(QBrush(fg));
                }
            }
        }
    }

    _totalLabel->setText(QString("Total value:  $%1").arg(grandTotal, 0, 'f', 2));
    _table->resizeColumnsToContents();
    _table->horizontalHeader()->setStretchLastSection(true);
}

// Helper: get selected item ID (-1 if none)
static int selectedItemId(QTableWidget* table) {
    int row = table->currentRow();
    if (row < 0) return -1;
    QTableWidgetItem* item = table->item(row, 0);
    if (!item) return -1;
    return item->data(Qt::UserRole).toInt();
}

void InventoryTab::onAdd()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Add Inventory Item");
    dlg.setMinimumWidth(340);
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(&dlg);
    nameEdit->setPlaceholderText("Item name");
    form.addRow("Name:", nameEdit);

    QLineEdit* catEdit = new QLineEdit(&dlg);
    catEdit->setPlaceholderText("e.g. Medication, Equipment, Supplies");
    form.addRow("Category:", catEdit);

    QSpinBox* qtySpin = new QSpinBox(&dlg);
    qtySpin->setRange(0, 999999);
    qtySpin->setValue(1);
    form.addRow("Quantity:", qtySpin);

    QDoubleSpinBox* priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0.01, 999999.99);
    priceSpin->setDecimals(2);
    priceSpin->setValue(1.00);
    priceSpin->setPrefix("$ ");
    form.addRow("Unit Price:", priceSpin);

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

    try {
        _svc.addInventoryItem(nameEdit->text().trimmed().toStdString(),
                              qtySpin->value(),
                              priceSpin->value(),
                              catEdit->text().trimmed().toStdString());
        loadItems();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void InventoryTab::onStockIn()
{
    int id = selectedItemId(_table);
    if (id < 0) {
        QMessageBox::information(this, "Stock In", "Select an item row first.");
        return;
    }

    bool ok;
    int qty = QInputDialog::getInt(this, "Stock In",
        "Quantity to add:", 1, 1, 999999, 1, &ok);
    if (!ok) return;

    try {
        _svc.stockIn(id, qty);
        loadItems();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void InventoryTab::onStockOut()
{
    int id = selectedItemId(_table);
    if (id < 0) {
        QMessageBox::information(this, "Stock Out", "Select an item row first.");
        return;
    }

    bool ok;
    int qty = QInputDialog::getInt(this, "Stock Out",
        "Quantity to remove:", 1, 1, 999999, 1, &ok);
    if (!ok) return;

    try {
        _svc.stockOut(id, qty);
        loadItems();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Insufficient Stock",
            QString("Could not remove stock:\n%1").arg(e.what()));
    }
}

void InventoryTab::onDelete()
{
    int id = selectedItemId(_table);
    if (id < 0) {
        QMessageBox::information(this, "Delete", "Select an item row first.");
        return;
    }

    int row = _table->currentRow();
    QString name = _table->item(row, 0)->text();

    auto reply = QMessageBox::question(this, "Delete Item",
        QString("Delete \"%1\" from inventory?").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _svc.removeInventoryItem(id);
        loadItems();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
