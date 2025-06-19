#pragma once

#include <iostream>
#include "Body.h"

namespace fix
{
    class Parser;
    class ResendRequest : public Body
    {
    public:
        ResendRequest() {}

        size_t getBeginSeqNo() const { return beginSeqNo; }
        size_t getEndSeqNo() const { return endSeqNo; }

    private:
        size_t beginSeqNo; // 7
        size_t endSeqNo;   // 16

        friend class Parser;
    };
}