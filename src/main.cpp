#include <iostream>
#include <bitset>
#include "Fix.h"
#include <optional>
#include "utils/BID_UINT64_WRAPPER.h"

int main() 
{

    std::optional<int> optInt;

    std::cout << optInt.has_value() << std::endl; // Should print 0 (false)

    optInt = 2;

    std::cout << optInt.has_value() << std::endl; // Should print 1 (true)

    //fix::Fix app("google.com", "8880", "Delolau", "IB", "123", 30);

    //app.newOrderSingle<fix::InstrumentType::FOREX, fix::OrderType::MARKET>("EUR", "USD", fix::OrderSide::buy, fix::Decimal("195.5"), fix::OrderForex::IsFXConv, "order1");

    return 0;
}
