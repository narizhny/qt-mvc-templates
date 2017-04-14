#include "TreeModel.h"
#include "TreeModelItem.h"

TreeModel::TreeModel(QObject *parent):
    QAbstractItemModel(parent),
    rootItem(new TreeModelItem(0)){}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return getItem(parent)->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    return getItem(index)->data(index.column(), role);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    return getItem(index)->setData(index.column(), value, role);
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    TreeModelItem *parentItem = getItem(parent);
    TreeModelItem *childItem = parentItem->child(row);

    return createIndex(row, column, childItem);
}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeModelItem *childItem = getItem(child);
    if(!childItem)
        return QModelIndex();
    TreeModelItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    TreeModelItem *grandParent = parentItem->parent();
    if(!grandParent)
        return QModelIndex();

    return createIndex(grandParent->childPosition(parentItem), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent)->childCount();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    return getItem(index)->flags(index.column());
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
        return rootItem->data(section, role);
    return QAbstractItemModel::headerData(section, orientation, role);
}

void TreeModel::setRootItem(TreeModelItem *rootItem)
{
    beginResetModel();
    this->rootItem.reset(rootItem);
    rootItem->propagateModel(this);
    endResetModel();
}

const TreeModelItem *TreeModel::getRootItem() const
{
    return rootItem.data();
}

TreeModelItem *TreeModel::getRootItem()
{
    return rootItem.data();
}

QModelIndex TreeModel::indexFromItem(const TreeModelItem *item) const
{
    if(!item || item == rootItem.data())
        return QModelIndex();

    return createIndex(item->parent()->childPosition(item), 0, const_cast<TreeModelItem *>(item));
}

TreeModelItem *TreeModel::itemFromIndex(const QModelIndex &index)
{
    return index.isValid()? getItem(index): 0;
}

const TreeModelItem *TreeModel::itemFromIndex(const QModelIndex &index) const
{
    return index.isValid()? getItem(index): 0;
}

TreeModelItem *TreeModel::getItem(const QModelIndex &index) const
{
    return index.isValid() ?
                static_cast<TreeModelItem*>(index.internalPointer()) :
                rootItem.data();
}


bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TreeModelItem *parentItem;

    if (parent.isValid()) {
        parentItem = itemFromIndex(parent);
    } else {
        parentItem = getRootItem();
    }

    if (!(row < parentItem->childCount() && (row + count) <= parentItem->childCount())) {
        return false;
    }

    bool result = parentItem->removeChildren(row, count);
    parentItem->updateRecursive();

    return result;
}
