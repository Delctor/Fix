#pragma once

#include <iostream>
#include <optional>
#include "Body.h"

namespace fix
{
    class Parser;
    class LogOut : public Body
    {
    public:
        LogOut() {}

        const std::optional<std::string>& getText() const { return text; }

    private:
        std::optional<std::string> text; // 58

        friend class Parser;
    };
}