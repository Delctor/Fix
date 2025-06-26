#pragma once

#include <iostream>
#include <string>
#include <optional>
#include "MsgType.h"
#include "./body/Body.h"
#include "./header/Header.h"
#include "./trailer/Trailer.h"
#include "error.h"

namespace fix
{

    typedef uint64_t ERROR;

    class Parser;

    class FixMsg
    {
    public:
        FixMsg() : header(), body(nullptr), trailer(), error() {}

        ~FixMsg() { delete body; }

        const Header& getHeader() const { return header; }

        const Body* getBody() const { return body; }

        const Trailer& getTrailer() const { return trailer; }

    private:
        Header header;
        Body* body;
        Trailer trailer;
        std::optional<ERROR> error;

        friend class Parser;
    };
};