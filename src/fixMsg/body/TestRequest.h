#pragma once
#include <iostream>
#include <optional>
#include "Body.h"

namespace fix
{
    class Parser;

    class TestRequest : public Body
    {
    public:
        TestRequest() {}

        const std::string& getTestReqID() const { return testReqID; }

    private:
        std::string testReqID;

        friend class Parser;
    };
};