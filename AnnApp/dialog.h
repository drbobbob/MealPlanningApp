#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class Dialog;
}

class QAbstractItemModel;
class QSortFilterProxyModel;
class QStandardItemModel;
class QItemSelection;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void setSourceIngredientModel(QStandardItemModel *model);

public slots:
    void filterChanged();

    void unusedItemClicked(const QItemSelection& selected, const QItemSelection& deselected);
    void usedItemClicked(const QItemSelection& selected, const QItemSelection& deselected);


    void addButtonClicked();
    void removeButtonClicked();

    void quantityChanged(double newVal);
    void targetChanged(double newVal);

private:
    void updateTotalLabels();

    Ui::Dialog *ui;
    QSortFilterProxyModel* proxyModel;
    QStandardItemModel* unsuedSourceModel;
    QStandardItemModel* usedModel;
};

#endif // DIALOG_H
