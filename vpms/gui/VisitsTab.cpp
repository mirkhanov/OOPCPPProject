#include "VisitsTab.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>

VisitsTab::VisitsTab(ClinicService& service, QWidget* parent)
    : QWidget(parent), _service(service)
{
    setupUI();
    loadData();
}

void VisitsTab::setupUI() {
    _table = new QTableWidget(this);
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"ID", "Animal ID", "Owner ID", "Date", "Total Cost"});
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->horizontalHeader()->setStretchLastSection(true);

    _addBtn    = new QPushButton("New Visit",     this);
    _cancelBtn = new QPushButton("Cancel Visit",  this);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(_addBtn);
    btnLayout->addWidget(_cancelBtn);
    btnLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_table);
    mainLayout->addLayout(btnLayout);

    connect(_addBtn,    &QPushButton::clicked, this, &VisitsTab::onAdd);
    connect(_cancelBtn, &QPushButton::clicked, this, &VisitsTab::onCancel);
}

void VisitsTab::loadData() {
    const auto& visits = _service.getAllVisits();
    _table->setRowCount((int)visits.size());
    for (int i = 0; i < (int)visits.size(); i++) {
        const Visit& v = visits[i];
        _table->setItem(i, 0, new QTableWidgetItem(QString::number(v.getId())));
        _table->setItem(i, 1, new QTableWidgetItem(QString::number(v.getAnimalId())));
        _table->setItem(i, 2, new QTableWidgetItem(QString::number(v.getOwnerId())));
        _table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(v.getDate())));
        _table->setItem(i, 4, new QTableWidgetItem(QString::number(v.getTotalCost(), 'f', 2)));
    }
}

void VisitsTab::onAdd() {
    QDialog dlg(this);
    dlg.setWindowTitle("New Visit");
    QFormLayout form(&dlg);

    QSpinBox* animalSpin = new QSpinBox(&dlg);
    animalSpin->setRange(1, 99999);
    form.addRow("Animal ID:", animalSpin);

    QSpinBox* ownerSpin = new QSpinBox(&dlg);
    ownerSpin->setRange(1, 99999);
    form.addRow("Owner ID:", ownerSpin);

    QLineEdit* dateEdit = new QLineEdit(&dlg);
    dateEdit->setPlaceholderText("YYYY-MM-DD");
    form.addRow("Date:", dateEdit);

    QLineEdit* servicesEdit = new QLineEdit(&dlg);
    servicesEdit->setPlaceholderText("e.g. 1,2,3  (comma-separated IDs)");
    form.addRow("Service IDs:", servicesEdit);

    QDialogButtonBox* btns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;
    if (dateEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, "Error", "Date cannot be empty."); return; }

    vector<int> serviceIds;
    if (!servicesEdit->text().trimmed().isEmpty()) {
        for (const QString& part : servicesEdit->text().split(',')) {
            bool ok;
            int sid = part.trimmed().toInt(&ok);
            if (ok) serviceIds.push_back(sid);
        }
    }

    try {
        _service.createVisit(animalSpin->value(), ownerSpin->value(),
                             dateEdit->text().toStdString(), serviceIds);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}

void VisitsTab::onCancel() {
    int row = _table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Cancel Visit", "Select a row first."); return; }

    int id = _table->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(this, "Cancel Visit", "Cancel this visit?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    try {
        _service.cancelVisit(id);
        loadData();
    } catch (const exception& e) {
        QMessageBox::warning(this, "Error", e.what());
    }
}
