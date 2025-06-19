#pragma once
#include <iostream>


namespace fix
{
    class Parser;

    class Trailer
    {
    public:
        Trailer() {}

        size_t getCheckSum() const { return this->checkSum; }

    private:
        size_t checkSum; // 10

        friend class Parser;
    };
}