#pragma once
#include <iostream>
#include <utility>
#include <math.h> 
#include "BID_UINT64_WRAPPER.h"
#include "./Number.h"
#include "./Lambda.h"
#include "./OrderType.h"

#define SOH '\x01'

template <typename T>
struct isInstanceOfLambda : std::false_type {};

template <typename Func, typename Enable>
struct isInstanceOfLambda<Lambda<Func, Enable>> : std::true_type {};

template<size_t N>
constexpr uint8_t computeSum(const char (&arr)[N])
{
	uint8_t sum = 0;
	for (size_t i = 0; i < N - 1; i++)
	{
		sum += arr[i] == '|' ? '\x01' : arr[i];
	}
	return sum;
}

template<typename T>
constexpr size_t getConstArrLen(const T& arg)
{
	if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
	{
		return sizeof(T) - 1;
	}
	else
	{
		return 0;
	}
}

template<typename T>
constexpr uint8_t getConstArrSum(const T& arg)
{
	if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
	{
		return computeSum(arg);
	}
	else
	{
		return 0;
	}
}

template<typename T>
size_t getStrLen(const T& arg)
{
	if constexpr (std::is_same_v<T, std::string>)
	{
		return arg.size();
	}
	else
	{
		return 0;
	}
}

template <typename T>
size_t getLambdaLen(const T& arg) 
{
	if constexpr (isInstanceOfLambda<std::decay_t<T>>::value) {
		return arg.size;
	}
	else {
		return 0;
	}
}

template <size_t N, size_t A, size_t M, size_t B, size_t... IndicesA, size_t... IndicesB>
constexpr auto generateSequenceHelper(std::index_sequence<IndicesA...>, std::index_sequence<IndicesB...>)
{
	return std::integer_sequence<size_t, ((void)IndicesA, A)..., ((void)IndicesB, B)...>{};
}

template <size_t N, size_t A, size_t M, size_t B>
constexpr auto generateSequence()
{
	return generateSequenceHelper<N, A, M, B>(
		std::make_index_sequence<N>{},
		std::make_index_sequence<M>{}
	);
}

template <size_t Start, size_t Step, size_t... Indices>
constexpr auto makeIndexSequenceRangeHelper(std::index_sequence<Indices...>) {
	return std::index_sequence<(Start + Indices * Step)...>{};
}

template <size_t Start, size_t End, size_t Step = 1>
using makeIndexSequenceRange = decltype(makeIndexSequenceRangeHelper<Start, Step>(
	std::make_index_sequence<(End - Start + Step - 1) / Step>{}
));

template<typename T>
size_t getNDigits(T num)
{
	if constexpr (std::is_integral<T>::value && !std::is_same<T, char>::value)
	{
		if (num >= 10) return static_cast<size_t>(log10(num)) + 1;
		else return 1;
	}
	else if constexpr (std::is_same<T, Double>::value)
	{
		if (num.num >= 10) return static_cast<size_t>(log10(num.num)) + 1 + num.nDigits + 1;
		else return 1 + num.nDigits;
	}
	else if constexpr (std::is_same<T, fix::Decimal>::value)
	{
		BID_UINT64 value = num.getValue();
		uint64_t sign = (value >> 63) & 0x1;
		uint64_t exponent = 398 - (value >> 53) & 0x3FF;
		uint64_t coefficient = value & 0x1FFFFFFFFFFFFF;
		
		size_t size = 0;

		if (coefficient == 0) return 1;
		if (sign) size++;
		
		bool nonZeroWasFound = false;
            
		while (exponent > 0) 
		{
			uint64_t digit = coefficient % 10;
			exponent--;
			coefficient /= 10;
			if (digit != 0 && !nonZeroWasFound) nonZeroWasFound = true;
			if (!nonZeroWasFound) continue;
			size++;
		}

		if (coefficient == 0) 
		{
			size += 2;
			return size; // for "0."
		} 

		if (nonZeroWasFound) size++; // for '.'
		
		while (coefficient > 0) 
		{
			uint64_t digit = coefficient % 10;
			size++;
			coefficient /= 10;
		}
		
		return size;
	}
	else if constexpr (std::is_same<T, char>::value)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

template<typename ...Args>
size_t getLen(const Args&... args)
{
	return (getConstArrLen(args) + ...) + (getStrLen(args) + ...) + (getLambdaLen(args) + ...) + (getNDigits(args) + ...);
}

template<typename ...Args>
size_t getSum(const Args&... args)
{
	return (getConstArrSum(args) + ...);
}

template<size_t start, size_t end, typename T, size_t N>
constexpr T getBytes(const char(&arr)[N])
{
	T num = 0;
	for (size_t i = end + 1; i-- > start; )
	{
		num = (num << 8) | (arr[i] == '|' ? SOH : arr[i]);
	}
	return num;
}