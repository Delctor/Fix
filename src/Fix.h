#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <immintrin.h>
#include <type_traits>
#include <inttypes.h>
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <ctime>
#include "./utils/helpers.h"
#include "./utils/Number.h"
#include "./utils/Lambda.h"
#include "./utils/ConstStr.h"
#include "./utils/consts.h"
#include "./utils/InstrumentType.h"
#include "./utils/OrderType.h"
#include "./utils/BID_UINT64_WRAPPER.h"
#include "bid_conf.h"
#include "bid_functions.h"

#ifndef SOH
#define SOH '\x01'
#endif

namespace fix
{
	class Fix
	{
	public:
		Fix(std::string url, std::string port, std::string senderCompID, std::string targetCompID, std::string account, uint8_t HeartBtInt);

		~Fix();

		int start();

	//private:

		void logOn();

		void logOut();

		void heartbeat();

		void heartbeat(std::string& testReqId);

		void testRequest();

		void resendRequest(size_t beginSeqNo, size_t endSeqNo);

		void sequenceReset(size_t newSeqNo);

		void gapFill(size_t seqNo, size_t newSeqNo);

		void reject(size_t refSeqNo, std::string& text);

		template<InstrumentType instrumentType, OrderType orderType>
		typename std::enable_if<orderType == OrderType::MARKET && instrumentType == InstrumentType::FOREX>::type newOrderSingle(std::string symbol, std::string currency, char side, Decimal qty, char IsFXConv, std::string orderID);

		template<InstrumentType instrumentType, OrderType orderType>
		typename std::enable_if<orderType == OrderType::LIMIT && instrumentType == InstrumentType::FOREX>::type newOrderSingle();

		template<InstrumentType instrumentType, OrderType orderType>
		typename std::enable_if<orderType == OrderType::STOP && instrumentType == InstrumentType::FOREX>::type newOrderSingle();

		template<InstrumentType instrumentType, OrderType orderType>
		typename std::enable_if<orderType == OrderType::STOPLIMIT && instrumentType == InstrumentType::FOREX>::type newOrderSingle();

		template<ConstStr msgType, typename ...Args>
		void write(const Args&... args);

		template<typename ...Args>
		void writeToBuffer(const Args&... args);

		template<size_t N>
		void writeToBufferHelper(const char(&arr)[N]);

		void writeToBufferHelper(const std::string& str);

		template<typename T>
		typename std::enable_if<std::is_same<T, char>::value>::type writeToBufferHelper(T c);

		template<typename T>
		void writeToBufferHelper(const Lambda<T>& lambda);

		template<typename T, size_t ...Indices, size_t ...Sizes, size_t N>
		void unpack(const char(&arr)[N], std::index_sequence<Indices...>, std::integer_sequence<size_t, Sizes...>);

		void writeTime();

		template<size_t size, typename T>
		void writeBytes(T value);

		template <typename T>
		typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, char>::value, void>::type writeToBufferHelper(T num);

		void writeToBufferHelper(const Double& num);

		void writeToBufferHelper(const Decimal& num);

		void reAllocateSendBuffer(size_t sizeNeeded = 1024);

		void computeCheckSum(size_t n);

		void resetCheckSumAndSize();

		inline void sendMessage();

		inline void reciveMessage();

		std::string url, port, senderCompID, targetCompID, account;
		//SOCKET sock;
		size_t seqNum;
		char* sendBuffer;
		char* recvBuffer;
		size_t sendBufferSize;
		size_t sendBufferSizeReserved;
		size_t recvBufferSizeReserved;
		uint8_t checkSum;
		size_t bodyLength;
		uint8_t HeartBtInt;
		time_t lastTestReqID;
	};
};
