#include "Parser.h"

namespace fix
{
    Parser::Parser(const std::string& message) : msg(message) {}

    void Parser::parse() 
    {
        size_t start = 0;
        size_t end;

        while ((end = this->msg.find(SOH, start)) != std::string_view::npos) 
        {
            
            start = end + 1;
        }
    }
};