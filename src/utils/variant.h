#pragma once
#include <iostream>

namespace fix
{
    template<typename... Types>
    class variant
    {
    public:
        variant() = default;

        // Add constructors, destructors, and other member functions as needed

        // Example: a visitor pattern to handle different types
        template<typename Visitor>
        void visit(Visitor&& visitor) const
        {
            // Implement visitation logic here
        }

        // Add more functionality as required
    private:

    };
};