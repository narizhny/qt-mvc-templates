#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <functional>

#include "Hash.h"
#include "ModelDataAdapter.h"

template<class Container,
         class Value = typename Container::value_type>
class TableModel: public QAbstractTableModel,
        public ModelDataAdapter<Value>
{
public:
    explicit TableModel(QObject *parent = 0):
        QAbstractTableModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Horizontal &&
                headers.contains(section) &&
                headers.value(section).contains(role))
            return headers[section][role];
        return QVariant();
    }
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
    {
        if(orientation != Qt::Horizontal)
            return false;
        headers[section][role] = value;
        return true;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        if(parent.isValid())
            return 0;
        return container.size();
    }
    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        if(parent.isValid())
            return 0;
        return m_column_count;
    }
    QVariant data(const QModelIndex &index, int role) const
    {
        return ModelDataAdapter<Value>::data(*(container.begin() + index.row()),
                                             index.column(),
                                             role);
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        bool result = ModelDataAdapter<Value>::setData(&*(container.begin() + index.row()),
                                                index.column(),
                                                value,
                                                role);
        dataChanged(index.sibling(index.row(), 0),
                    index.sibling(index.row(), m_column_count - 1));
        return result;
    }
    void addValue(const value_type &value)
    {
        insertValue(container.size(), value);
    }
    void insertValue(int position, const value_type &value)
    {
        beginInsertRows(QModelIndex(), position, position);
        container.insert(container.begin() + position, value);
        endInsertRows();
    }
    void setValues(const Container &values)
    {
        beginResetModel();
        container = values;
        endResetModel();
    }
    void remove(int row, int count = 1)
    {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        container.erase(container.begin() + row, container.begin() + row + count - 1);
        endRemoveRows();
    }
    void update(int row, int column)
    {
        dataChanged(index(row, column), index(row, column));
    }
    void update(int row)
    {
        dataChanged(index(row, 0), index(row, m_column_count - 1));
    }

    const Container &getContainer() const
    {
        return container;
    }
    const value_type &getValue(int pos) const
    {
        return container.at(pos);
    }
    void setValue(int pos, const value_type &value)
    {
        container[pos] = value;
        emit dataChanged(index(pos, 0), index(pos, m_column_count - 1));
    }


private:
    void columnManipulatorAdded(int column) override
    {
        int count = std::max(column + 1, m_column_count);
        if(count == m_column_count)
            return;

        if(count > m_column_count)
        {
            beginInsertColumns(QModelIndex(), m_column_count, count - 1);
            m_column_count = count;
            endInsertColumns();
        }
        else
        {
            beginRemoveColumns(QModelIndex(), count, m_column_count - 1);
            m_column_count = count;
            endRemoveColumns();
        }
    }

    int             m_column_count = 0;
    Container       container;
};

#endif // TABLEMODEL_H
