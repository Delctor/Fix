#pragma once
#include <iostream>

template <size_t N>
struct ConstStr
{
    char arr[N];
    size_t size;

    constexpr ConstStr(const char(&str)[N])
    {
        size = N;
        for (size_t i = 0; i < N; i++)
        {
            arr[i] = str[i];
        }
    }

    constexpr char operator[](size_t i) const
    {
        return arr[i];
    }

    template<size_t sizeOther>
    constexpr bool operator==(const ConstStr<sizeOther> other) const
    {
        if constexpr (N == sizeOther)
        {
            for (size_t i = 0; i < N; i++)
            {
                if (arr[i] != other[i])
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }
};