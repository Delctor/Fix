#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <charconv> 
#include <ctime>
#include "../../utils/consts.h"
#include "../../utils/switch.h"
#include "../fixMsg/FixMsg.h"
#include "../fixMsg/MsgType.h"

#ifndef SOH
#define SOH '\x01'
#endif

namespace fix
{
    class Parser
    {
    public:
        using FuctParseBody = void (Parser::*)(unsigned long, const std::string_view&, FixMsg&, uint8_t&);

        Parser(size_t(*read)(size_t), char* recvBuffer, size_t recvBufferSizeReserved, std::queue<FixMsg>& queue, 
                std::mutex& mtx, std::atomic<bool>& continueParsing) : read(read), recvBuffer(recvBuffer), 
                recvBufferSizeReserved(recvBufferSizeReserved), queue(queue), mtx(mtx), continueParsing(continueParsing) {}
        
        void parse()
        {
            std::string_view bufferView(recvBuffer, this->read(0));

            size_t start = 0;
            size_t end = 0;
            size_t lastEnd = 0;

            while (this->continueParsing)
            {
                std::string_view bufferView(recvBuffer + start, this->read(0));
                FixMsg fixMsg;

                uint8_t parseState = 0; // 0: Header, 1: Body, 2: Trailer

                FuctParseBody parseBody = nullptr;

                auto inner = [&]() -> void 
                {
                    while (true)
                    {
                        if ((end = bufferView.find(SOH, start)) == std::string_view::npos)
                        {
                            if (lastEnd == 0)
                            {
                                this->appendError(fixMsg, SOH_NOT_FOUND_ERROR);
                                start = 0;
                                return;
                            }
                            else
                            {
                                start = lastEnd + 1;
                                memcpy(this->recvBuffer, this->recvBuffer + lastEnd + 1, bufferView.size() - (lastEnd + 1));
                                bufferView = std::string_view(recvBuffer, this->read(bufferView.size() - (lastEnd + 1)));
                                continue;
                            }
                        }

                        const std::string_view msg = bufferView.substr(start, end - start);
                        start = end + 1;
                        lastEnd = end;

                        unsigned long tagNum;
                        std::string_view value;

                        if (!this->isTagOk(msg, tagNum, value))
                        {
                            this->appendError(fixMsg, TAG_ERROR);
                            return;
                        }

                        switch (parseState)
                        {
                        case 0:
                            if (this->parseHeader(tagNum, value, fixMsg, parseState, parseBody)) continue;
                            else return;
                        case 1:
                        default:
                            break;
                        }
                    }
                };
            }
        }

        // Header

        bool parseHeader(unsigned long tagNum, const std::string_view& value, FixMsg& fixMsg, uint8_t& parseState, FuctParseBody& parseBody)
        {
            if (value.size() == 0) return false;
            switch (tagNum)
            {
            case 8: // BeginString
                if (this->parseBeginString(value, fixMsg)) return true;
                this->appendError(fixMsg, BEGIN_STRING_ERROR);
                return false;
            case 9: // BodyLength
                if (this->parseBodyLength(value, fixMsg)) return true;
                this->appendError(fixMsg, BODY_LENGHT_ERROR);
                return false;
            case 35: // MsgType
                if (this->parseMsgType(value, fixMsg)) return true;
                this->appendError(fixMsg, MSG_TYPE_ERROR);
                return false;
            case 49: // SenderCompID
                if (fixMsg.header.setSenderCompID(value)) return true;
                this->appendError(fixMsg, SENDER_COMP_ID_ERROR);
                return false;
            case 50: // SenderSubID
                if (fixMsg.header.setSenderSubID(value)) return true;
                this->appendError(fixMsg, SENDER_SUB_ID_ERROR);
                return false;
            case 56: // TargetCompID
                if (fixMsg.header.setTargetCompID(value)) return true;
                this->appendError(fixMsg, TARGET_COMP_ID_ERROR);
                return false;
            case 57: // TargetSubID
                if (fixMsg.header.setTargetSubID(value)) return true;
                this->appendError(fixMsg, TARGET_SUB_ID_ERROR);
                return false;
            case 34: // MsgSeqNum
                if (this->parseMsgSeqNum(value, fixMsg)) return true;
                this->appendError(fixMsg, SEQ_NUM_ERROR);
                return false;
            case 43: // PossDupFlag
                if (this->parsePossDupFlag(value, fixMsg)) return true;
                this->appendError(fixMsg, POSS_DUP_FLAG_ERROR);
                return false;
            case 97: // PossResend
                if (this->parsePossResend(value, fixMsg)) return true;
                this->appendError(fixMsg, POSS_RESEND_ERROR);
                return false;
            case 52: // SendingTime
                if (this->parseSendingTime(value, fixMsg)) return true;
                this->appendError(fixMsg, SENDING_TIME_ERROR);
                return false;
            case 122: // OrigSendingTime
                if (this->parseOrigSendingTime(value, fixMsg)) return true;
                this->appendError(fixMsg, ORIG_SENDING_TIME_ERROR);
                return false;
            default:
                ERROR error = fixMsg.header.verifyRequirements();
                if (error)
                {
                    this->appendError(fixMsg, error);
                    return false;
                }
                parseState = 2; // Body
                return true;
            }
        }

        bool parseBeginString(const std::string_view& value, FixMsg& fixMsg)
        {
            if (value != std::string_view(beginString, sizeof(beginString) - 1))
            {
                return false;
            }
            return fixMsg.header.setBeginString(42);
        }

        bool parseBodyLength(const std::string_view& value, FixMsg& fixMsg)
        {
            size_t tmp;
            auto result = std::from_chars(value.data(), value.data() + value.size(), tmp);
            if (result.ec != std::errc()) return false;
            return fixMsg.header.setBodyLenght(tmp);
        }

        bool parseMsgType(const std::string_view& value, FixMsg& fixMsg)
        {
            uint64_t msgTypeBytes = 0;
            for (size_t i = 0; i < value.size(); ++i)
            {
                msgTypeBytes |= static_cast<uint64_t>(value[i]) << (8 * i);
            }

            MsgType msgType = static_cast<MsgType>(msgTypeBytes);

            switch (msgType)
            {
                case MsgType::Heartbeat:
                    break; 
                case MsgType::TestRequest:
                    break;
                case MsgType::ResendRequest:
                    break;
                case MsgType::Reject: 
                    break;
                case MsgType::SequenceReset:
                    break;
                case MsgType::Logout:
                    break;
                case MsgType::ExecutionReport:
                    break;
                case MsgType::OrderCancelReject:
                    break;
                case MsgType::Logon:
                    break;
                case MsgType::NewsBulletinMessage:
                    break;
                case MsgType::OrderSingle:
                    break;
                case MsgType::OrderCancelRequest:
                    break;
                case MsgType::OrderCancelReplaceRequest:
                    break;
                case MsgType::OrderStatusRequest:
                    break;
                case MsgType::Allocation:
                    break;
                case MsgType::AllocationAck:
                    break;
                case MsgType::NewOrderMultiLeg:
                    break;
                case MsgType::MultilegCancelReplaceRequest:
                    break;
                default:
                    return false;
                break;
            }
            
            return fixMsg.header.setMsgType(msgType);
        }

        bool parseMsgSeqNum(const std::string_view& value, FixMsg& fixMsg)
        {
            size_t tmp;
            auto result = std::from_chars(value.data(), value.data() + value.size(), tmp);
            if (result.ec != std::errc())
            {
                return false;
            }
            return fixMsg.header.setSeqNum(tmp);
        }

        bool parsePossDupFlag(const std::string_view& value, FixMsg& fixMsg)
        {
            if (value.size() == 1)
            {
                if (value[0] == 'Y')
                {
                    return fixMsg.header.setPossDupFlag(true);
                }
                else if (value[0] == 'N')
                {
                    return fixMsg.header.setPossDupFlag(false);
                }
            }
            return false;
        }

        bool parsePossResend(const std::string_view& value, FixMsg& fixMsg)
        {
            if (value.size() == 1)
            {
                if (value[0] == 'Y')
                {
                    return fixMsg.header.setPossResend(true);
                }
                else if (value[0] == 'N')
                {
                    return fixMsg.header.setPossResend(false);
                }
            }
            return false;
        }

        bool parseSendingTime(const std::string_view& value, FixMsg& fixMsg)
        {
            if (value.size() == 21)
            {
                int year, month, day, hour, minute, second, millisecond;
                int matched = std::sscanf(value.data(), "%4d%2d%2d-%2d:%2d:%2d.%3d",
                              &year, &month, &day, &hour, &minute, &second, &millisecond);
                
                if (month < 1 || month > 12 ||
                    day   < 1 || day   > 31 ||
                    hour  < 0 || hour  > 23 ||
                    minute< 0 || minute> 59 ||
                    second< 0 || second> 60 ||
                    millisecond < 0 || millisecond > 999) return false;

                std::tm tm_time = {};
                tm_time.tm_year = year - 1900;
                tm_time.tm_mon  = month - 1;
                tm_time.tm_mday = day;
                tm_time.tm_hour = hour;
                tm_time.tm_min  = minute;
                tm_time.tm_sec  = second;

                time_t seconds_since_epoch = timegm(&tm_time);
                if (seconds_since_epoch < 0) return false;

                uint64_t timestamp_ms = static_cast<uint64_t>(seconds_since_epoch) * 1000 + millisecond;

                return fixMsg.header.setSendingTime(timestamp_ms);
            }
            return false;
        }

        bool parseOrigSendingTime(const std::string_view& value, FixMsg& fixMsg)
        {
            if (value.size() == 21)
            {
                int year, month, day, hour, minute, second, millisecond;
                int matched = std::sscanf(value.data(), "%4d%2d%2d-%2d:%2d:%2d.%3d",
                              &year, &month, &day, &hour, &minute, &second, &millisecond);
                
                if (month < 1 || month > 12 ||
                    day   < 1 || day   > 31 ||
                    hour  < 0 || hour  > 23 ||
                    minute< 0 || minute> 59 ||
                    second< 0 || second> 60 ||
                    millisecond < 0 || millisecond > 999) return false;

                std::tm tm_time = {};
                tm_time.tm_year = year - 1900;
                tm_time.tm_mon  = month - 1;
                tm_time.tm_mday = day;
                tm_time.tm_hour = hour;
                tm_time.tm_min  = minute;
                tm_time.tm_sec  = second;

                time_t seconds_since_epoch = timegm(&tm_time);
                if (seconds_since_epoch < 0) return false;

                uint64_t timestamp_ms = static_cast<uint64_t>(seconds_since_epoch) * 1000 + millisecond;

                return fixMsg.header.setOrigSendingTime(timestamp_ms);
            }
            return false;
        }

        // Utils

        bool isTagOk(const std::string_view& msg, unsigned long& tagNum, std::string_view& value)
        {
            if (msg.empty()) return false;
            
            size_t equalPoss = msg.find('=');
            if (equalPoss == std::string_view::npos) return false;

            std::string_view tag = msg.substr(0, equalPoss);
            value = msg.substr(equalPoss + 1, msg.size() - equalPoss - 1);

            if (tag.empty() || value.empty()) return false;

            auto result = std::from_chars(tag.data(), tag.data() + tag.size(), tagNum);

            if (result.ec != std::errc() || tagNum == 0) return false;
            
            return true;
        }

        void appendToQueue(const FixMsg& fixMsg)
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(fixMsg);
        }

        void appendError(FixMsg& fixMsg, ERROR error)
        {
            fixMsg.error = error;
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(fixMsg);
        }

    private:
        size_t(*read)(size_t);
        char* recvBuffer;
        size_t recvBufferSizeReserved;
        std::queue<FixMsg> queue;
        std::mutex& mtx;
        std::atomic<bool>& continueParsing;
    };

};