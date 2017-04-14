#include "TreeModelItem.h"
#include "TreeModel.h"

TreeModelItem::TreeModelItem(int columns):
    m_column_count(columns) {}

TreeModelItem *TreeModelItem::child(int number)
{
    return const_cast<TreeModelItem *>(static_cast<const TreeModelItem &>(*this).child(number));
}

const TreeModelItem *TreeModelItem::child(int number) const
{
    return children.value(number).data();
}

int TreeModelItem::childCount() const
{
    return children.size();
}

int TreeModelItem::childPosition(const TreeModelItem *item) const
{
    int result = -1;
    for(int i = 0; i < children.size(); ++i)
    {
        if(children[i].data() == item)
        {
            result = i;
            break;
        }
    }
    return result;
}

TreeModelItem *TreeModelItem::parent()
{
    return m_parent;
}

const TreeModelItem *TreeModelItem::parent() const
{
    return m_parent;
}

bool TreeModelItem::removeChildren(int position, int count)
{
    if(position + count > children.size())
        return false;

    if(count == 0)
        return true;

    if(m_model)
    {
        QModelIndex i = index();
        m_model->beginRemoveRows(i, position, position + count - 1);
    }

    children.erase(children.begin() + position,
                   children.begin() + position + count);

    if(m_model)
        m_model->endRemoveRows();

    return true;
}

void TreeModelItem::removeChildren()
{
    removeChildren(0, children.size());
}

void TreeModelItem::moveHere(TreeModelItemPtr item, int pos)
{
    TreeModelItem *oldParent = item->parent();
    int oldPos = oldParent->childPosition(item.data());

    oldParent->removeChildren(oldPos, 1);

    if (oldParent == this && pos > oldPos) {
        pos--;
    }

    insertChild(pos, item);

    updateRecursive();
}

TreeModelItemPtr TreeModelItem::takeChild(int number)
{
    if (number < 0 || number > children.size()) {
        throw std::range_error("TreeModelItem::takeChild - Invalid index");
    }

    return children.at(number);
}

void TreeModelItem::detachChild(TreeModelItemPtr ptr)
{
    children.removeOne(ptr);
}

Qt::ItemFlags TreeModelItem::flags(int column) const
{
    Q_UNUSED(column);

    return Qt::ItemIsEnabled;
}

void TreeModelItem::prependChild(TreeModelItem *childItem)
{
    insertChild(0, childItem);
}

void TreeModelItem::appendChild(TreeModelItem *childItem)
{
    insertChild(children.size(), childItem);
}

void TreeModelItem::insertChild(int pos, TreeModelItem *childItem)
{
    insertChild(pos, TreeModelItemPtr(childItem));
}

void TreeModelItem::insertChild(int pos, TreeModelItemPtr childItem)
{
    pos = std::min(children.size(), pos);
    pos = std::max(0, pos);

    if(m_model)
    {
        QModelIndex i = index();
        m_model->beginInsertRows(i, pos, pos);
    }

    childItem->setParent(this);
    children.insert(children.begin() + pos, childItem);

    if(m_model)
    {
        childItem->propagateModel(m_model);
        m_model->endInsertRows();
    }

}

void TreeModelItem::setParent(TreeModelItem *parent)
{
    this->m_parent = parent;
}

int TreeModelItem::columnCount() const
{
    return m_column_count;
}

void TreeModelItem::setColumnCount(int count)
{
    if(m_column_count == count)
        return;

    bool ins = count > m_column_count;
    if(m_model)
    {
        if(ins)
            m_model->beginInsertColumns(index(), m_column_count, count - 1);
        else
            m_model->beginRemoveColumns(index(), count, m_column_count - 1);
    }

    this->m_column_count = count;

    if(m_model)
    {
        if(ins)
            m_model->endInsertColumns();
        else
            m_model->endRemoveColumns();
    }
}

void TreeModelItem::propagateModel(TreeModel *model)
{
    m_model = model;
    for(auto &c: children)
        c->propagateModel(model);
}

QModelIndex TreeModelItem::index() const
{
    if(!m_model)
        return QModelIndex();

    return m_model->indexFromItem(this);
}

void TreeModelItem::update() const
{
    if(m_model)
    {
        QModelIndex i = index();
        m_model->dataChanged(i.sibling(i.row(), 0),
                             i.sibling(i.row(), m_column_count - 1));
    }
}

void TreeModelItem::updateRecursive() const
{
    if(m_model)
    {
        update();

        for(auto &c: children)
            c->updateRecursive();
    }
}

void TreeModelItem::update(int column) const
{
    if(m_model)
    {
        QModelIndex i = index();
        QModelIndex targetIndex = i.sibling(i.row(), column);
        m_model->dataChanged(targetIndex, targetIndex);
    }
}
