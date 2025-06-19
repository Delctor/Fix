#pragma once
#include <iostream>
#include <type_traits>

template <typename T>
struct isLambda 
{
private:
    template <typename U>
    static auto check(U*) -> decltype(&U::operator(), std::true_type{});

    template <typename>
    static std::false_type check(...) { return {}; }

public:
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

template <typename Func, typename = std::enable_if_t<isLambda<Func>::value>>
struct Lambda
{
    const Func lambda;
    const size_t size;

    Lambda(const Func lambda, const int size) : lambda(lambda), size(size) {}
};