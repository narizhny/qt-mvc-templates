#ifndef TREEMODELCUSTOMITEM_H
#define TREEMODELCUSTOMITEM_H

#include "TreeModelItem.h"
#include "ModelDataAdapter.h"

template<class Value>
class TreeModelCustomItem: public TreeModelItem,
                            public ModelDataAdapter<Value>
{
public:
    explicit                TreeModelCustomItem(const Value &value):
        TreeModelItem(0),
        value(value) {}

    void                    setValue(const Value &value)        {this->value = value; update();}
    const Value             &getValue() const                   {return value;}

    QVariant                data(int column, int role) const
    {
        return ModelDataAdapter<Value>::data(value, column, role);
    }
    bool                    setData(int column, const QVariant &data, int role)
    {
        bool result = ModelDataAdapter<Value>::setData(&value, column, data, role);
        update();
        return result;
    }
    Qt::ItemFlags           flags(int column) const
    {
        return this->getFlags(column);
    }

private:
    void columnManipulatorAdded(int column) override
    {
        setColumnCount(std::max(column + 1, columnCount()));
    }

    Value   value;
};

template<class Value>
class TreeModelCustomItem<Value *>: public TreeModelItem,
                                    public ModelDataAdapter<Value>
{
public:
    explicit TreeModelCustomItem(Value *value):
        TreeModelItem(0),
        value(value) {}

    void                    setValue(Value *value)      {this->value = value; update();}
    const Value             *getValue() const           {return value;}

    QVariant                data(int column, int role) const
    {
        return ModelDataAdapter<Value>::data(*value, column, role);
    }
    bool                    setData(int column, const QVariant &data, int role)
    {
        return ModelDataAdapter<Value>::setData(value, column, data, role);
    }
    Qt::ItemFlags           flags(int column) const
    {
        return this->getFlags(column);
    }

private:
    void columnManipulatorAdded(int column) override
    {
        setColumnCount(std::max(column + 1, columnCount()));
    }

    Value *value;
};

#endif // TREEMODELCUSTOMITEM_H
