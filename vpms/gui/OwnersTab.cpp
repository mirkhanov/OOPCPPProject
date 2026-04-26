#include "OwnersTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>

OwnersTab::OwnersTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void OwnersTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(3);
    _table->setHorizontalHeaderLabels({"ID", "Name", "Contact"});
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

    connect(_addBtn,    &QPushButton::clicked, this, &OwnersTab::onAdd);
    connect(_editBtn,   &QPushButton::clicked, this, &OwnersTab::onEdit);
    connect(_deleteBtn, &QPushButton::clicked, this, &OwnersTab::onDelete);
}

void OwnersTab::loadData() {
    const auto& owners = _service.getAllOwners();
    _table->setRowCount((int)owners.size());
    for (int i = 0; i < (int)owners.size(); i++) {
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(owners[i].getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(owners[i].getName())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(owners[i].getContactInfo())));
    }
}

void OwnersTab::onAdd() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Owner", "Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString contact = QInputDialog::getText(this, "Add Owner", "Contact:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    try {
        _service.addOwner(name.toStdString(), contact.toStdString());
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void OwnersTab::onEdit() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Edit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    bool ok;
    QString name = QInputDialog::getText(this, "Edit Owner", "Name:",
        QLineEdit::Normal, _table->item(row, 1)->text(), &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString contact = QInputDialog::getText(this, "Edit Owner", "Contact:",
        QLineEdit::Normal, _table->item(row, 2)->text(), &ok);
    if (!ok) return;

    try {
        Owner o = _service.getOwner(id);
        o.setName(name.toStdString());
        o.setContactInfo(contact.toStdString());
        _service.updateOwner(o);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void OwnersTab::onDelete() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Delete", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Delete", "Delete this owner?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.removeOwner(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
