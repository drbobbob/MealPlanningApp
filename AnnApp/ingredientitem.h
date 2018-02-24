#ifndef INGREDIENTITEM_H
#define INGREDIENTITEM_H

#include <QStandardItem>

namespace IngredientItem {
enum IngredientRoles { CaloriesRole = Qt::UserRole,
                           FatRole,
                           ProteinRole,
                           CarbsRole,
                           QuantityRole
                         };

}

#endif // INGREDIENTITEM_H
