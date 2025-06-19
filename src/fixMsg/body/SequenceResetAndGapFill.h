#pragma once

#include <iostream>
#include <optional>
#include "Body.h"

namespace fix
{
    class Parser;

    class SequenceResetAndGapFill : public Body
    {
    public:
        SequenceResetAndGapFill() {}

        const std::optional<bool>& getGapFillFlag() const { return gapFillFlag; }

        size_t getNewSeqNo() const { return newSeqNo; }

    private:
        std::optional<bool> gapFillFlag; // 123
        size_t newSeqNo; // 36

        friend class Parser;
    };
}