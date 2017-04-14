# Qt MVC templates
Library for farst creating trees and tables

### Adding to project
Just add "include(path_to_lib/Model.pri)" in your project file

### Tree

```c++
#include "TreeModelCustomItem.h"
auto *i = new TreeModelCustomItem<Type>(Type(...));

//member function getter
i->addGetter(0, Qt::DisplayRole, &Type::fun);

//functor's getter
i->addGetter(0, Qt::ToolTipRole, [](const Type &t) -> QString {return t.getTip();});

//member function setter
i->addSetter(0, Qt::EditRole, &Type::setText);

//functor's setter
i->addSetter(0, Qt::EditRole, [](Type &t, const QString &value) {t.setText(value);});
i->addCheckingSetter(0, Qt::EditRole, [](Type &t, const QString &value) -> bool {return t.setText(value);});

auto *m = new TreeModel();
m->getRootItem()->appendChild(i);
```

### Table

```c++
#include "TableModel.h"
...
QStringList sl;
sl << "One" << "Two" << "Three";
auto *tableModel = new TableModel<QStringList>();
tableModel->setValues(sl);
tableModel->addGetter(0, Qt::DisplayRole, [](const QString &value){return value;});
tableModel->addGetter(1, Qt::ToolTipRole, &QString::size);
```