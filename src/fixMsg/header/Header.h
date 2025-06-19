#pragma once
#include <iostream>
#include <optional>
#include "../MsgType.h"

namespace fix
{
    class Parser;

    class Header
    {
    public:

        Header() {}

        uint64_t getBeginString() const { return this->beginString; }

        size_t getBodyLength() const { return this->bodyLenght; }

        MsgType getMsgType() const { return this->msgType; }

        const std::string& getSenderCompID() const { return this->senderCompID; }

        const std::string& getTargetCompID() const { return this->targetCompID; }

        const std::optional<std::string>& getSenderSubID() const { return this->senderSubID; }

        const std::optional<std::string>& getTargetSubID() const { return this->targetSubID; }

        size_t getSeqNum() const { return this->seqNum; }

        const std::optional<std::string>& getPossDupFlag() const { return this->possDupFlag; }

        const std::optional<std::string>& getPossResend() const { return this->possResend; }

        uint64_t getSendingTime() const { return this->sendingTime; }

        const std::optional<uint64_t>& getOrigSendingTime() const { return this->origSendingTime; }

    private:
        uint64_t beginString;
        size_t bodyLenght;
        MsgType msgType;
        std::string senderCompID;
        std::string targetCompID;
        std::optional<std::string> senderSubID;
        std::optional<std::string> targetSubID;
        size_t seqNum;
        std::optional<std::string> possDupFlag;
        std::optional<std::string> possResend;
        uint64_t sendingTime;
        std::optional<uint64_t> origSendingTime;

        friend class Parser;
    };
};