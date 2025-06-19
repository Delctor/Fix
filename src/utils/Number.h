#pragma once
#include <iostream>
#include <type_traits>


struct Double
{
	Double(double num, size_t nDigits) : num(num), nDigits(nDigits) {}

	double num;
	size_t nDigits;
};
