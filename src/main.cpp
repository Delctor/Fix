#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <iomanip>
#include <optional>
#include <functional>
#include <type_traits>
#include <tuple>
#include "utils/crc32Constexpr.h"
#include "utils/switch.h"


int main()
{
    using namespace fix;

    uint64_t num;
    std::cin >> num;


    Switch(num).setCases<
    Case(1, +[](const uint64_t& a) { std::cout << "a" << std::endl; }),
    Case(2, +[](const uint64_t& a) { std::cout << "b" << std::endl; }),
    Default(+[](const uint64_t& a) { std::cout << "default" << std::endl; })>
    (2);
    
    
    
    return 0;
}