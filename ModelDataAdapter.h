#ifndef MODELDATAADAPTER_H
#define MODELDATAADAPTER_H

#include <QMap>
#include <functional>
#include "function_traits.h"

template<class Value>
class ModelDataAdapter
{
public:
    typedef Value value_type;
    typedef std::function<QVariant(const Value &)> GetterType;
    typedef std::function<bool(Value &, const QVariant &)> SetterType;

    template<class R>
    void                    addGetter(int column,
                                      int role,
                                      R (Value::*m)() const)
    {
        columnGetters[column].insert(role, std::bind(m, std::placeholders::_1));
        columnManipulatorAdded(column);
    }

    template<class Function>
    void                    addGetter(int column,
                                      int role,
                                      const Function &function)
    {
        columnGetters[column].insert(role, [function](const Value &value){return QVariant(function(value));});
        columnManipulatorAdded(column);
    }

    template<class Function>
    void                    addSetter(int column,
                                      int role,
                                      const Function &setter)
    {
        using FT = function_traits<Function>;
        columnSetters[column].insert(role, [setter](Value &value, const QVariant &variant)->bool {setter(value, variant.value<typename FT::std_function::second_argument_type>()); return true;});
    }

    template<class R,
             class A>
    void                    addSetter(int column,
                                      int role,
                                      R (Value::*member)(A))
    {
        columnSetters[column].insert(role, [member](Value &value, const QVariant &variant)->bool
        {
            std::mem_fn(member)(value, variant.value<std::decay<A>::type>());
            return true;
        });
    }

    template<class Arg>
    void                    addCheckingSetter(int column,
                                              int role,
                                              const std::function<bool(Value &, const Arg &)> &setter)
    {
        columnSetters[column].insert(role, [setter](Value &value, const QVariant &variant)->bool {return setter(value, variant.value<Arg>());});
    }

    template<class R,
             class A>
    void                    addCheckingSetter(int column,
                                              int role,
                                              R (Value::*member)(A))
    {
        columnSetters[column].insert(role, [member](Value &value, const QVariant &variant)->bool
        {
            return static_cast<bool>(std::mem_fn(member)(value, variant.value<std::decay<A>::type>()));
        });
    }

    QVariant                data(const Value &value, int column, int role) const
    {
        if(columnGetters.contains(column) && columnGetters[column].contains(role))
            return columnGetters[column][role](value);

        return QVariant();
    }
    bool                    setData(Value *value, int column, const QVariant &data, int role)
    {
        if(!columnSetters.contains(column) ||
                !columnSetters[column].contains(role))
            return false;

        return columnSetters[column][role](*value, data);
    }
    Qt::ItemFlags           getFlags(int column) const
    {
        if(columnSetters.contains(column))
            return Qt::ItemIsEnabled | Qt::ItemIsEditable;

        return Qt::ItemIsEnabled;
    }

protected:
    virtual void columnManipulatorAdded(int column) = 0;

private:
    //role -> getter
    typedef QMap<int, GetterType> RoleGetters;
    typedef QMap<int, SetterType> RoleSetters;


    //column number -> roles
    QMap<int, RoleGetters>  columnGetters;
    QMap<int, RoleSetters>  columnSetters;

};

#endif // MODELDATAADAPTER_H
