#include "dialog.h"
#include "ingredientitem.h"
#include <QApplication>
#include <QStandardItemModel>

QStandardItem* buildItem(QString name, double calories, double fat, double protein, double carbs)
{
    QStandardItem* item = new QStandardItem(name);
    item->setData(calories, IngredientItem::CaloriesRole);
    item->setData(fat, IngredientItem::FatRole);
    item->setData(protein, IngredientItem::ProteinRole);
    item->setData(carbs, IngredientItem::CarbsRole);
    item->setData(1.0, IngredientItem::QuantityRole);
    return item;
}

QStandardItemModel *createSourceIngredientModel(QObject* parent)
{
    QStandardItemModel* model = new QStandardItemModel(parent);
    model->appendRow(buildItem("Carrot", 1,2,3,4));
    model->appendRow(buildItem("Cheese", 2,3,4,5));
    model->appendRow(buildItem("popcorn", 3,4,5,6));
    model->appendRow(buildItem("potato chip", 4,5,6,7));
    model->appendRow(buildItem("ground beef", 7,8,9,10));

    return model;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setSourceIngredientModel(createSourceIngredientModel(&w));
    w.show();

    return a.exec();
}
