#pragma once
#include <iostream>
#include <string>
#include "../fixMsg/FixMsg.h"

#ifndef SOH
#define SOH '\x01'
#endif

namespace fix
{
    class Parser
    {
    public:
        Parser(const std::string& message) : msg(message) {}

        void parse()
        {
            size_t start = 0;
            size_t end;

            while ((end = this->msg.find(SOH, start)) != std::string::npos) 
            {
                // Process the segment between start and end
                std::string segment = this->msg.substr(start, end - start);
                // Here you would typically parse the segment into fields
                std::cout << "Parsed segment: " << segment << std::endl;

                start = end + 1; // Move to the next segment
            }

            // Handle any remaining part of the message after the last SOH
            if (start < this->msg.length()) 
            {
                std::string segment = this->msg.substr(start);
                std::cout << "Parsed final segment: " << segment << std::endl;
            }
        }
        

    private:
        std::string msg;
    };

};