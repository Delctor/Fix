#pragma once
#include <iostream>
#include <optional>
#include <cstdint>
#include <type_traits>
#include "../error.h"
#include "../MsgType.h"

struct RequiredTagBase 
{
    static inline int requieredCount = 0;
};

template<typename T, bool = std::is_class_v<T>>
struct RequiredTag;

template<typename T>
struct RequiredTag<T, true> : public T, public RequiredTagBase
{
    using T::T;
    using T::operator=;
    bool hasValue = false;

    void setHasValue()
    {
        hasValue = true;
    }

    operator T&() { return *this; }
    operator const T&() const { return *this; }
};

template<typename T>
struct RequiredTag<T, false> : RequiredTagBase
{
    RequiredTag() = default;
    T value;
    bool hasValue = false;

    void setHasValue()
    {
        hasValue = true;
    }

    RequiredTag<T, false>& operator=(T other)
    {
        value = other;
    }

    operator T&() { return *this; }
    operator const T&() const { return *this; }
};

namespace fix
{
    typedef uint64_t ERROR;

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

        const std::optional<bool>& getPossDupFlag() const { return this->possDupFlag; }

        const std::optional<bool>& getPossResend() const { return this->possResend; }

        uint64_t getSendingTime() const { return this->sendingTime; }

        const std::optional<uint64_t>& getOrigSendingTime() const { return this->origSendingTime; }

    private:

        bool setBeginString(uint64_t value)
        {
            if (this->beginString.hasValue) return false;
            this->beginString = value;
            this->beginString.setHasValue();
            return true;
        }

        bool setBodyLenght(size_t value)
        {
            if (this->bodyLenght.hasValue) return false;
            this->bodyLenght = value;
            this->bodyLenght.setHasValue();
            return true;
        }

        bool setMsgType(MsgType msgType)
        {
            if (this->msgType.hasValue) return false;
            this->msgType = msgType;
            this->msgType.setHasValue();
            return true;
        }

        bool setSenderCompID(const std::string_view& senderCompID)
        {
            if(this->senderCompID.hasValue) return false;
            this->senderCompID = senderCompID;
            this->senderCompID.setHasValue();
            return true;
        }

        bool setTargetCompID(const std::string_view& targetCompID)
        {
            if(this->targetCompID.hasValue) return false;
            this->targetCompID = targetCompID;
            this->targetCompID.setHasValue();
            return true;
        }

        bool setSenderSubID(const std::string_view& senderSubID)
        {
            if (this->senderSubID.has_value()) return false;
            this->senderSubID = senderSubID;
            return true;
        }

        bool setTargetSubID(const std::string_view& targetSubID)
        {
            if (this->targetSubID.has_value()) return false;
            this->targetSubID = targetSubID;
            return true;
        }

        bool setSeqNum(size_t seqNum)
        {
            if (this->seqNum.hasValue) return false;
            this->seqNum = seqNum;
            this->seqNum.setHasValue();
            return true;
        }

        bool setPossDupFlag(bool possDupFlag)
        {
            if (this->possDupFlag.has_value()) return false;
            this->possDupFlag = possDupFlag;
            return true;
        }

        bool setPossResend(bool possResend)
        {
            if (this->possResend.has_value()) return false;
            this->possResend = possResend;
            return true;
        }

        bool setSendingTime(uint64_t sendingTime)
        {
            if (this->sendingTime.hasValue) return false;
            this->sendingTime = sendingTime;
            this->sendingTime.setHasValue();
            return true;
        }

        bool setOrigSendingTime(bool origSendingTime)
        {
            if (this->origSendingTime.has_value()) return false;
            this->origSendingTime = origSendingTime;
            return true;
        }

        ERROR verifyRequirements()
        {
            if (RequiredTagBase::requieredCount == 7)
            {
                RequiredTagBase::requieredCount = 0;
                return NO_ERROR;
            }

            if (!this->beginString.hasValue) return BEGIN_STRING_MISSING_ERROR;
            if (!this->bodyLenght.hasValue) return BODY_LENGHT_MISSING_ERROR;
            if (!this->msgType.hasValue) return MSG_TYPE_MISSING_ERROR;
            if (!this->senderCompID.hasValue) return SENDER_COMP_ID_MISSING_ERROR;
            if (!this->targetCompID.hasValue) return TARGET_COMP_ID_MISSING_ERROR;
            if (!this->seqNum.hasValue) return SEQ_NUM_MISSING_ERROR;
            if (!this->sendingTime.hasValue) return SENDING_TIME_MISSING_ERROR;
            return TAG_ERROR;
        }

        RequiredTag<uint64_t> beginString;
        RequiredTag<size_t> bodyLenght;
        RequiredTag<MsgType> msgType;
        RequiredTag<std::string, true> senderCompID;
        RequiredTag<std::string> targetCompID;
        std::optional<std::string> senderSubID;
        std::optional<std::string> targetSubID;
        RequiredTag<size_t> seqNum;
        std::optional<bool> possDupFlag;
        std::optional<bool> possResend;
        RequiredTag<uint64_t> sendingTime;
        std::optional<uint64_t> origSendingTime;

        friend class Parser;
    };

    
};