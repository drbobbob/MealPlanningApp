#include "dialog.h"
#include "ui_dialog.h"
#include "ingredientitem.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QItemSelection>
#include <QDebug>

namespace
{
    const char* NormalStyleString = "QLabel { color : black; }";
    const char* YellowStyleString = "QLabel { background-color : yellow; color : red; }";
    const char* RedStyleString = "QLabel { background-color : red; color : yellow; }";
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    proxyModel(new QSortFilterProxyModel(this)),
    usedModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->UnusedIngreditentsList->setModel(proxyModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->UsedIngredientsList->setModel(usedModel);

    connect(ui->FilterInputEdit, &QLineEdit::textChanged, this, &Dialog::filterChanged);
    connect(ui->UnusedIngreditentsList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Dialog::unusedItemClicked);
    connect(ui->UsedIngredientsList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Dialog::usedItemClicked);
    connect(ui->AddButton, &QPushButton::clicked, this, &Dialog::addButtonClicked);
    connect(ui->RemoveButton, &QPushButton::clicked, this, &Dialog::removeButtonClicked);
    connect(ui->ItemQuantity, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Dialog::quantityChanged);

    connect(ui->TargetCalories, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Dialog::targetChanged);
    connect(ui->TargetCarbs, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Dialog::targetChanged);
    connect(ui->TargetFat, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Dialog::targetChanged);
    connect(ui->TargetProtein, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Dialog::targetChanged);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setSourceIngredientModel(QStandardItemModel *model)
{
    unsuedSourceModel = model;
    proxyModel->setSourceModel(model);
    proxyModel->sort(0);
}

void Dialog::filterChanged()
{
    proxyModel->setFilterWildcard(ui->FilterInputEdit->text());
}

void Dialog::unusedItemClicked(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    if(selected.length())
    {
        ui->CurrentCalories->setText(proxyModel->data(selected.at(0).indexes().at(0), IngredientItem::CaloriesRole).toString());
        ui->CurrentCarbs->setText(proxyModel->data(selected.at(0).indexes().at(0), IngredientItem::CarbsRole).toString());
        ui->CurrentFat->setText(proxyModel->data(selected.at(0).indexes().at(0), IngredientItem::FatRole).toString());
        ui->CurrentProtein->setText(proxyModel->data(selected.at(0).indexes().at(0), IngredientItem::ProteinRole).toString());
        ui->ItemQuantity->setValue(proxyModel->data(selected.at(0).indexes().at(0), IngredientItem::QuantityRole).toDouble());
        ui->UsedIngredientsList->clearSelection();
    }
    else
    {

        qDebug() << "Nothing selected";
    }
}

void Dialog::usedItemClicked(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    if(selected.length())
    {
        ui->CurrentCalories->setText(usedModel->data(selected.at(0).indexes().at(0), IngredientItem::CaloriesRole).toString());
        ui->CurrentCarbs->setText(usedModel->data(selected.at(0).indexes().at(0), IngredientItem::CarbsRole).toString());
        ui->CurrentFat->setText(usedModel->data(selected.at(0).indexes().at(0), IngredientItem::FatRole).toString());
        ui->CurrentProtein->setText(usedModel->data(selected.at(0).indexes().at(0), IngredientItem::ProteinRole).toString());
        ui->ItemQuantity->setValue(usedModel->data(selected.at(0).indexes().at(0), IngredientItem::QuantityRole).toDouble());
        ui->UnusedIngreditentsList->clearSelection();
    }
    else
    {

        qDebug() << "Nothing selected";
    }

}

void Dialog::addButtonClicked()
{
    QItemSelection itemSelection = ui->UnusedIngreditentsList->selectionModel()->selection();
    if(itemSelection.length())
    {
        QItemSelection sourceSelection = proxyModel->mapSelectionToSource(itemSelection);
        QStandardItem * item = unsuedSourceModel->itemFromIndex(sourceSelection.at(0).indexes().at(0));
        QStandardItem* newItem = item->clone();
        usedModel->appendRow(newItem);
        unsuedSourceModel->removeRow(sourceSelection.at(0).indexes().at(0).row());
        ui->UnusedIngreditentsList->clearSelection();
        ui->UsedIngredientsList->clearSelection();
        updateTotalLabels();
    }
}

void Dialog::removeButtonClicked()
{
    QItemSelection sourceSelection = ui->UsedIngredientsList->selectionModel()->selection();
    if(sourceSelection.length())
    {
        QStandardItem * item = usedModel->itemFromIndex(sourceSelection.at(0).indexes().at(0));
        QStandardItem* newItem = item->clone();
        unsuedSourceModel->appendRow(newItem);
        usedModel->removeRow(sourceSelection.at(0).indexes().at(0).row());
        ui->UnusedIngreditentsList->clearSelection();
        ui->UsedIngredientsList->clearSelection();
        updateTotalLabels();
    }

}

void Dialog::quantityChanged(double newVal)
{
    QItemSelection unusedSelection = ui->UnusedIngreditentsList->selectionModel()->selection();
    QItemSelection usedSelection = ui->UsedIngredientsList->selectionModel()->selection();
    if(unusedSelection.length())
    {
        proxyModel->setData(unusedSelection.at(0).indexes().at(0), newVal, IngredientItem::QuantityRole);
    }
    else if(usedSelection.length())
    {
        usedModel->setData(usedSelection.at(0).indexes().at(0), newVal, IngredientItem::QuantityRole);
        updateTotalLabels();
    }
    else
    {
        qDebug() << "value changed when nothing selected";
    }
}

void Dialog::targetChanged(double /*newVal*/)
{
    updateTotalLabels();
}

static void setLabel(QLabel* label, double value, double targetValue)
{
    label->setText(QString::number(value));
    if(value > targetValue * 1.1)
    {
        label->setStyleSheet(RedStyleString);
    }
    else if(value < targetValue * 0.9)
    {
        label->setStyleSheet(YellowStyleString);
    }
    else
    {
        label->setStyleSheet(NormalStyleString);
    }

}

void Dialog::updateTotalLabels()
{
    double caloriesValue = 0;
    double carbsValue = 0;
    double fatValue = 0;
    double proteinValue = 0;
    for(int i = 0; i < usedModel->rowCount(); i++)
    {
        QModelIndex index = usedModel->index(i, 0);
        double itemQuantity = usedModel->data(index, IngredientItem::QuantityRole).toDouble();
        caloriesValue += usedModel->data(index, IngredientItem::CaloriesRole).toDouble() * itemQuantity;
        carbsValue += usedModel->data(index, IngredientItem::CarbsRole).toDouble() * itemQuantity;
        fatValue += usedModel->data(index, IngredientItem::FatRole).toDouble() * itemQuantity;
        proteinValue += usedModel->data(index, IngredientItem::ProteinRole).toDouble() * itemQuantity;
    }
    setLabel(ui->TotalCalories, caloriesValue, ui->TargetCalories->value());
    setLabel(ui->TotalCarbs, carbsValue, ui->TargetCarbs->value());
    setLabel(ui->TotalFat, fatValue, ui->TargetFat->value());
    setLabel(ui->TotalProtein, proteinValue, ui->TargetProtein->value());
}
