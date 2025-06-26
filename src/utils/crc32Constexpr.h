#pragma once
#include <iostream>
#include <cstdint>
#include <array>

constexpr uint32_t CRC32C_POLY = 0x82F63B78u;

constexpr std::array<uint32_t, 256> generate_crc32c_table() 
{
    std::array<uint32_t, 256> table = {};
    for (uint32_t i = 0; i < 256; ++i) 
    {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j)
            c = (c >> 1) ^ ((c & 1) ? CRC32C_POLY : 0);
        table[i] = c;
    }
    return table;
}

constexpr auto crc32c_table = generate_crc32c_table();

constexpr uint32_t crc32c_constexpr(uint64_t data, uint32_t crc = 0) 
{

    for (int i = 0; i < 8; ++i) {
        uint8_t byte = static_cast<uint8_t>(data >> (i * 8));
        crc = crc32c_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

constexpr uint64_t perfectHashConstexpr(uint64_t key, uint32_t crc = 0)
{
    return crc32c_constexpr(key, crc);
}