#pragma once

namespace fix
{
	enum class OrderType
	{
		MARKET,
		LIMIT,
		STOP,
		STOPLIMIT
	};

	namespace OrderSide
	{
		constexpr char buy = '1';
		constexpr char sell = '2';
		constexpr char sellShort = '5';
		constexpr char sellShortExempt = '6';
	};

	namespace OrderForex
	{
		constexpr char IsFXConv = '1';
		constexpr char IsNotFXConv = '0';
	};
};