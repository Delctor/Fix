#pragma once
#include <iostream>
#include <cstdint>
#include <optional>
#include "Body.h"
#include "../utils/BID_UINT64_WRAPPER.h"

namespace fix
{
    class Parser;

    class ExecutionReport : public Body
    {
    public:
        ExecutionReport() : orderID(), clOrdID(std::nullopt), execID(), execTransType(),
                            execRefID(std::nullopt), ordStatus(), ordRejReason(std::nullopt), execType(),
                            leavesQty(), settlmntTyp(std::nullopt), symbol(),
                            side(0), orderQty(), ordType(std::nullopt),
                            price(std::nullopt), stopPx(std::nullopt),
                            timeInForce(std::nullopt), rule80A(std::nullopt),
                            lastShares(), lastPx(0.0), cumQty(),
                            avgPx(0.0), transactTime(std::nullopt),
                            text(std::nullopt), clearingFirm(),
                            clearingAccount(std::nullopt),
                            securityType(std::nullopt), putOrCall(std::nullopt),
                            strikePrice(std::nullopt),
                            maturityMonthYear(std::nullopt),
                            maturityDay(std::nullopt),
                            securityExchange(std::nullopt),
                            exDestination(std::nullopt), lastMkt(std::nullopt),
                            customerOrFirm(std::nullopt),
                            comboLegInfo(std::nullopt), pipExchanges(std::nullopt) {}
    private:
        std::string orderID;
        std::optional<std::string> clOrdID;
        std::string execID;
        std::string execTransType;
        std::optional<std::string> execRefID;
        std::string ordStatus;
        std::optional<std::string> ordRejReason;
        std::string execType;
        Decimal leavesQty;
        std::optional<uint8_t> settlmntTyp;
        std::string symbol;
        uint8_t side;
        Decimal orderQty;
        std::optional<uint64_t> ordType;
        std::optional<std::string> price;
        std::optional<std::string> stopPx;
        std::optional<uint8_t> timeInForce;
        std::optional<uint8_t> rule80A;
        Decimal lastShares;
        double lastPx;
        Decimal cumQty;
        double avgPx;
        std::optional<uint64_t> transactTime;
        std::optional<std::string> text;
        std::string clearingFirm;
        std::optional<std::string> clearingAccount;
        std::optional<std::string> securityType;
        std::optional<uint8_t> putOrCall;
        std::optional<double> strikePrice;
        std::optional<uint64_t> maturityMonthYear;
        std::optional<uint8_t> maturityDay;
        std::optional<std::string> securityExchange;
        std::optional<std::string> exDestination;
        std::optional<std::string> lastMkt;
        std::optional<uint8_t> customerOrFirm;
        std::optional<std::string> comboLegInfo;
        std::optional<std::string> pipExchanges;

        friend class Parser;
    };
};