#ifndef FUNCTION_TRAITS
#define FUNCTION_TRAITS

#include <functional>
#include "VariadicTypedef.h"

namespace details
{

template <typename ReturnType, typename... Args>
struct function_traits_helper
{
    typedef variadic_typedef<Args...> args;

    enum { arity = sizeof...(Args) };
    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
};

template<typename T>
struct lambda_traits: public lambda_traits<decltype(&T::operator())> {};

template<typename ClassType, typename ReturnType, typename... Args>
struct lambda_traits<ReturnType(ClassType::*)(Args...) const> : public function_traits_helper<ReturnType, Args...>
{
    typedef std::function<ReturnType(Args...)> std_function;
    typedef ReturnType (*function)(Args...);
    static ReturnType call(function f, Args&&... a)
    {
        return (*f)(std::forward<Args>(a)...);
    }
};

}

template <typename T>
struct function_traits
    : public details::lambda_traits<T> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>: public details::function_traits_helper<ReturnType, Args...>
{
    typedef ClassType class_type;
    typedef const ClassType object_type;
    typedef ReturnType(ClassType::*function)(Args...) const;

    static ReturnType call(object_type *object, function f, Args&&... a)    {return (object->*f)(std::forward<Args>(a)...);}
    static ReturnType call(object_type &object, function f, Args&&... a)    {return (object.*f)(std::forward<Args>(a)...);}
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>: public details::function_traits_helper<ReturnType, Args...>
{
    typedef ClassType class_type;
    typedef ClassType object_type;
    typedef ReturnType(ClassType::*function)(Args...);

    static ReturnType call(object_type *object, function f, Args&&... a)    {return (object->*f)(std::forward<Args>(a)...);}
    static ReturnType call(object_type &object, function f, Args&&... a)    {return (object.*f)(std::forward<Args>(a)...);}
};

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>: public details::function_traits_helper<ReturnType, Args...>
{
    typedef std::function<ReturnType(Args...)> std_function;
    typedef ReturnType (*function)(Args...);

    static ReturnType call(function f, Args&&... a)
    {
        (*f)(std::forward<Args>(a)...);
    }
};

#endif // FUNCTION_TRAITS
