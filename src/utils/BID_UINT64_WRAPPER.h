#pragma once
#include <iostream>
#include <array>
#include <bid_conf.h>
#include <bid_functions.h>

namespace fix
{
    class Decimal
    {
    public:
        Decimal() : num(0) {}

        Decimal(BID_UINT64 value) : num(value) {}

        Decimal(const std::string& str)
        {
            unsigned int flags;
            num = __bid64_from_string(const_cast<char*>(str.c_str()), 0, &flags);
        }

        Decimal(const Decimal& other) : num(other.num) {}

        Decimal(double d)
        {
            unsigned int flags;
            num = __binary64_to_bid64(d, 0, &flags);
        }

        BID_UINT64 getValue() const
        {
            return num;
        }

        Decimal& operator=(const Decimal& other)
        {
            if (this != &other)
            {
                num = other.num;
            }
            return *this;
        }

        Decimal& operator=(BID_UINT64 value)
        {
            num = value;
            return *this;
        }

        Decimal& operator=(double d)
        {
            unsigned int flags;
            num = __binary64_to_bid64(d, 0, &flags);
            return *this;
        }

        Decimal operator+(const Decimal& other) const
        {
            unsigned int flags;
            Decimal result;
            return __bid64_add(this->num, other.num, 0, &flags);
        }

        void operator+=(const Decimal& other)
        {
            unsigned int flags;
            this->num = __bid64_add(this->num, other.num, 0, &flags);
        }

        Decimal operator-(const Decimal& other) const
        {
            unsigned int flags;
            Decimal result;
            return __bid64_sub(this->num, other.num, 0, &flags);
        }

        void operator-=(const Decimal& other)
        {
            unsigned int flags;
            this->num = __bid64_sub(this->num, other.num, 0, &flags);
        }

        Decimal operator*(const Decimal& other) const
        {
            unsigned int flags;
            Decimal result;
            return __bid64_mul(this->num, other.num, 0, &flags);
        }

        void operator*=(const Decimal& other)
        {
            unsigned int flags;
            this->num = __bid64_mul(this->num, other.num, 0, &flags);
        }

        Decimal operator/(const Decimal& other) const
        {
            unsigned int flags;
            Decimal result;
            return __bid64_div(this->num, other.num, 0, &flags);
        }

        void operator/=(const Decimal& other)
        {
            unsigned int flags;
            this->num = __bid64_div(this->num, other.num, 0, &flags);
        }

        bool operator==(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_quiet_equal(this->num, other.num, &flags);
        }

        bool operator!=(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_quiet_equal(this->num, other.num, &flags) == 0;
        }

        bool operator<(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_signaling_less(this->num, other.num, &flags);
        }

        bool operator<=(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_signaling_less_equal(this->num, other.num, &flags);
        }

        bool operator>(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_signaling_greater(this->num, other.num, &flags);
        }

        bool operator>=(const Decimal& other) const
        {
            unsigned int flags;
            return __bid64_signaling_greater_equal(this->num, other.num, &flags);
        }

        uint8_t toString(char* buffer, size_t& space) const
        {
            uint64_t sign = (num >> 63) & 0x1;
            uint64_t exponent = 398 - (num >> 53) & 0x3FF;
            uint64_t coefficient = num & 0x1FFFFFFFFFFFFF;
            

            if (coefficient == 0)
            {
                buffer[0] = '0';
                space++;
                return '0';
            }

            size_t i = 0;

            uint8_t checkSum = 0;

            if (sign) 
            {
                buffer[i] = '-';
                buffer++;
                space++;
                checkSum += '-';
            }

            bool nonZeroWasFound = false;

            while (exponent > 0) 
            {
                uint64_t digit = coefficient % 10;
                exponent--;
                coefficient /= 10;
                if (digit != 0 && !nonZeroWasFound) nonZeroWasFound = true;
                if (!nonZeroWasFound) continue;
                char c = '0' + digit;
                buffer[i++] = c;
                space++;
                checkSum += c;
            }
            
            if (coefficient == 0) 
            {
                buffer[i++] = '.';
                buffer[i++] = '0';
                space++;
                checkSum += '.' + '0';
                goto swap;
            }

            if (nonZeroWasFound) buffer[i++] = '.'; space++; checkSum += '.';

            while (coefficient > 0) 
            {
                uint64_t digit = coefficient % 10;
                char c = '0' + digit;
                buffer[i++] = c;
                coefficient /= 10;
                space++;
                checkSum += c;
            }
            

            swap:
            for (int j = 0; j < i / 2; j++) 
            {
                std::swap(buffer[j], buffer[i - 1 - j]);
            }
            return checkSum;
        }

    private:
        BID_UINT64 num;
    };
};