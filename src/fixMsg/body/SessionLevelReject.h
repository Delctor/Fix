#pragma once

#include <iostream>
#include <optional>
#include "Body.h"

namespace fix
{
    class Parser;
    
    class SessionLevelReject : public Body
    {
    public:
        SessionLevelReject() {}

        size_t getRefSeqNo() const { return refSeqNo; }

        const std::optional<std::string>& getText() const { return text; }

    private:
        size_t refSeqNo; // 45
        std::optional<std::string> text; // 58

        friend class Parser;
    };
}