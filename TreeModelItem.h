#ifndef TREEMODELITEM_H
#define TREEMODELITEM_H

#include <QList>
#include <QVariant>
#include <QPixmap>
#include <QSharedPointer>
#include <QMap>

class TreeModel;
class TreeModelItem;
typedef QSharedPointer<TreeModelItem> TreeModelItemPtr;

class TreeModelItem
{
public:
    explicit                TreeModelItem(int columns);
    virtual                 ~TreeModelItem() {}

    virtual bool            setData(int column, const QVariant &data, int role) { Q_UNUSED(column); Q_UNUSED(data); Q_UNUSED(role); return false; }
    virtual QVariant        data(int column, int role) const { Q_UNUSED(column); Q_UNUSED(role); return QVariant(); }
    virtual Qt::ItemFlags   flags(int column) const;

    TreeModelItem           *child(int number);
    const TreeModelItem     *child(int number) const;
    int                     childCount() const;
    int                     childPosition(const TreeModelItem *item) const;
    TreeModelItem           *parent();
    const TreeModelItem     *parent() const;
    bool                    removeChildren(int position, int count);
    void                    removeChildren();
    void                    moveHere(TreeModelItemPtr item, int pos);
    TreeModelItemPtr        takeChild(int number);
    void                    detachChild(TreeModelItemPtr ptr);

    void                    prependChild(TreeModelItem *childItem);
    void                    appendChild(TreeModelItem *childItem);
    void                    insertChild(int pos, TreeModelItem *childItem);
    void                    insertChild(int pos, TreeModelItemPtr childItem);
    void                    setParent(TreeModelItem *parent);

    int                     columnCount() const;
    void                    setColumnCount(int count);

    void                    propagateModel(TreeModel *model);
    QModelIndex             index() const;
    void                    update() const;
    void                    updateRecursive() const;
    void                    update(int column) const;

private:
    int                     m_column_count;
    TreeModelItem           *m_parent = 0;
    QList<TreeModelItemPtr> children;
    TreeModel               *m_model = 0;
};

#endif // TREEMODELITEM_H
