#pragma once
#include <iostream>
#include <optional>
#include "Body.h"

namespace fix
{
    class Parser;

    class Logon : public Body
    {
    public:
        Logon() {}

        uint8_t getEncryptMethod() const { return encryptMethod; }

        uint64_t getHeartBtInt() const { return heartBtInt; }

        const std::optional<bool>& getResetSeqNumFlag() const { return resetSeqNumFlag; }

    private:
        uint8_t encryptMethod; // 98
        uint64_t heartBtInt; // 108
        std::optional<bool> resetSeqNumFlag; // 141

        friend class Parser;
    };
};