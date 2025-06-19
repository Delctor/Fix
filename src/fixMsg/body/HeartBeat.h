#pragma once

#include <iostream>
#include <optional>
#include "Body.h"


namespace fix
{
    class Parser;

    class HeartBeat : public Body
    {
    public:
        HeartBeat() {}

        const std::optional<std::string>& getTestReqID() const { return testReqID; }

    private:
        std::optional<std::string> testReqID; // 112

        friend class Parser;
    };
}