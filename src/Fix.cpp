
#include "Fix.h"


namespace fix
{

	Fix::Fix(std::string url, std::string port, 
		std::string senderCompID, std::string targetCompID, std::string account, uint8_t HeartBtInt) :
		url(url), port(port), senderCompID(senderCompID), 
		targetCompID(targetCompID), account(account), seqNum(1), sendBuffer(new char[2048]), recvBuffer(new char[2048]),
		sendBufferSizeReserved(2048), sendBufferSize(0), HeartBtInt(HeartBtInt),
		checkSum(0), bodyLength(0), recvBufferSizeReserved(2048), lastTestReqID()
	{
		this->writeToBuffer(beginString);
		this->sendBufferSize = getConstArrLen(beginString);
		this->checkSum = getConstArrSum(beginString);
		
	}

	Fix::~Fix()
	{
		delete[] this->sendBuffer;
		delete[] this->recvBuffer;
	}

	int Fix::start()
	{
		/*
		
		WSADATA wsaData;
		this->sock = INVALID_SOCKET;
		struct addrinfo hints {}, * serverInfo = nullptr;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "Error en WSAStartup\n";
			return 1;
		}

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(this->url.c_str(), this->port.c_str(), &hints, &serverInfo) != 0) {
			std::cerr << "Error al resolver el nombre de dominio.\n";
			WSACleanup();
			return 1;
		}

		this->sock = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
		if (this->sock == INVALID_SOCKET) {
			std::cerr << "Error al crear socket: " << WSAGetLastError() << "\n";
			freeaddrinfo(serverInfo);
			WSACleanup();
			return 1;
		}

		if (connect(this->sock, serverInfo->ai_addr, (int)serverInfo->ai_addrlen) == SOCKET_ERROR) {
			std::cerr << "Error al conectar: " << WSAGetLastError() << "\n";
			closesocket(this->sock);
			freeaddrinfo(serverInfo);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(serverInfo);
	
		this->logOn();
		return 0;
		*/
		return 1;
	}

	void Fix::sendMessage()
	{
		/*
		if (send(this->sock, this->sendBuffer, this->sendBufferSize, 0) == SOCKET_ERROR)
		{
			std::cout << "Error al enviar: " << WSAGetLastError() << std::endl;
		}
		*/
		for (size_t i = 0; i < this->sendBufferSize; i++)
		{
			std::cout << (this->sendBuffer[i] == SOH ? '|' : this->sendBuffer[i]);
		}
		std::cout << std::endl;
		this->sendBuffer[17] = 'G';
		this->sendBufferSize = getConstArrLen(beginString);
		
		this->bodyLength = 0;
		this->checkSum = getConstArrSum(beginString);
		this->seqNum++;
	}

	void Fix::logOn()
	{
		this->write<"A">("98=0|108=", this->HeartBtInt,
						"|141=Y|");
		this->sendMessage();
	}

	void Fix::logOut()
	{
		this->write<"5">();
		this->sendMessage();
	}

	void Fix::heartbeat()
	{
		this->write<"0">();
		this->sendMessage();
	}

	void Fix::heartbeat(std::string& testReqId)
	{
		this->write<"0">("112=", testReqId, "|");
		this->sendMessage();
	}

	void Fix::testRequest()
	{
		auto now = std::chrono::high_resolution_clock::now();
		uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		this->lastTestReqID = timestamp;
		this->write<"0">("112=", timestamp, "|");
		this->sendMessage();
	}

	void Fix::resendRequest(size_t beginSeqNo, size_t endSeqNo)
	{
		this->write<"0">("7=", beginSeqNo, "|", 
						"16=", endSeqNo, "|");
		this->sendMessage();
	}

	void Fix::sequenceReset(size_t newSeqNo)
	{
		this->write<"Y">("123=N|36=", newSeqNo, "|");
		this->sendMessage();
		this->seqNum = newSeqNo;
	}

	void Fix::gapFill(size_t seqNo, size_t newSeqNo)
	{
		size_t seqNumTmp = this->seqNum;
		this->seqNum = seqNo;
		this->write<"Y">("123=Y|36=", newSeqNo, "|");
		this->sendMessage();
		this->seqNum = newSeqNo;
	}

	void Fix::reject(size_t refSeqNo, std::string& text)
	{
		this->write<"3">("45=", refSeqNo, "|", 
						"58=", text, "|");
		this->sendMessage();
	}

	template<>
	void Fix::newOrderSingle<InstrumentType::FOREX, OrderType::MARKET>(std::string symbol, std::string currency, char side, Decimal qty, char IsFXConv, std::string orderID)
	{
		this->write<"D">("1=", this->account,
			"|11=", orderID,
			"|15=", currency, 
			"|55=", symbol, 
			"|54=", side,
			"|58=", qty, 
			"|21=2|40=1|204=0|100=IDEALPRO|167=CASH|6707=", IsFXConv, "|"
			);
		this->sendMessage();
	}

	template<ConstStr msgType, typename ...Args>
	void Fix::write(const Args&... args)
	{
		if constexpr (sizeof...(args))
		{
			this->bodyLength += (getConstArrLen(args) + ...);
			this->bodyLength += (getStrLen(args) + ...);
			this->bodyLength += (getNDigits(args) + ...);
			this->bodyLength += (getLambdaLen(args) + ...);
			this->checkSum += (getConstArrSum(args) + ...);
		}
		// Header

		this->bodyLength += getLen("35=", msgType.arr,
			"|49=", this->senderCompID,
			"|56=", this->targetCompID,
			"|34=", this->seqNum,
			"|52=", Lambda([&]() { this->writeTime(); }, 21)) + sizeof(SOH);

		this->checkSum += getSum("9=", this->bodyLength, "|35=", msgType.arr,
			"|49=", this->senderCompID,
			"|56=", this->targetCompID,
			"|34=",
			"|52=") + SOH;
	
		this->writeToBuffer("9=", this->bodyLength, "|35=", msgType.arr,
			"|49=", this->senderCompID,
			"|56=", this->targetCompID,
			"|34=", this->seqNum,
			"|52=", Lambda([&]() { this->writeTime(); }, 21), "|");

		// Body

		if constexpr (sizeof...(args))
		{
			(this->writeToBufferHelper(args), ...);
		}

		// Trailer

		this->writeToBufferHelper("10=");
		this->writeToBufferHelper(this->checkSum);
		this->writeToBufferHelper("|");
	}

	template<typename ...Args>
	void Fix::writeToBuffer(const Args&... args)
	{
		(this->writeToBufferHelper(args), ...);
	}

	template<size_t N>
	void Fix::writeToBufferHelper(const char(&arr)[N])
	{
		constexpr size_t size = N - 1;

		constexpr size_t n8Bytes = size / 8;
		constexpr size_t remainer8Bytes = size % 8;
		constexpr size_t nActual8Bytes = remainer8Bytes > 4 ? n8Bytes + 1 : n8Bytes;
		constexpr size_t actualRemainer8Bytes = remainer8Bytes > 4 ? 0 : remainer8Bytes;

		constexpr size_t n4Bytes = actualRemainer8Bytes / 4;
		constexpr size_t remainer4Bytes = actualRemainer8Bytes % 4;
		constexpr size_t nActual4Bytes = n4Bytes > 2 ? n4Bytes + 1 : n4Bytes;
		constexpr size_t actualRemainer4Bytes = n4Bytes > 2 ? 0 : remainer4Bytes;

		constexpr size_t n2Bytes = actualRemainer4Bytes / 2;
		constexpr size_t remainer2Bytes = actualRemainer4Bytes % 2;

		constexpr size_t n1Bytes = remainer2Bytes;
		constexpr size_t remainer1Bytes = 0;

		constexpr size_t start8Bytes = 0;
		constexpr size_t end8Bytes = size - actualRemainer8Bytes;

		constexpr size_t start4Bytes = end8Bytes;
		constexpr size_t end4Bytes = size - actualRemainer4Bytes;

		constexpr size_t start2Bytes = end4Bytes;
		constexpr size_t end2Bytes = size - remainer2Bytes;

		constexpr size_t start1Bytes = end2Bytes;
		constexpr size_t end1Bytes = size;

		if constexpr (remainer8Bytes > 4) this->unpack<uint64_t>(arr, makeIndexSequenceRange<start8Bytes, end8Bytes, 8>{}, generateSequence<nActual8Bytes != 0 ? nActual8Bytes - 1 : 0, 8, nActual8Bytes != 0 ? 1 : 0, remainer8Bytes>());
		else this->unpack<uint64_t>(arr, makeIndexSequenceRange<start8Bytes, end8Bytes, 8>{}, generateSequence<nActual8Bytes, 8, 0, 0>());
	
		if constexpr (remainer4Bytes > 2) this->unpack<uint32_t>(arr, makeIndexSequenceRange<start4Bytes, end4Bytes, 4>{}, generateSequence<nActual4Bytes != 0 ? nActual4Bytes - 1 : 0, 4, nActual4Bytes != 0 ? 1 : 0, remainer4Bytes>());
		else this->unpack<uint32_t>(arr, makeIndexSequenceRange<start4Bytes, end4Bytes, 4>{}, generateSequence<nActual4Bytes, 4, 0, 0>());
	
		this->unpack<uint16_t>(arr, makeIndexSequenceRange<start2Bytes, end2Bytes, 2>{},
			generateSequence<n2Bytes, 2, 0, 0>()
			);
		this->unpack<uint8_t>(arr, makeIndexSequenceRange<start1Bytes, end1Bytes, 1>{},
			generateSequence<n1Bytes, 1, 0, 0>()
			);
	
	}

	void Fix::writeToBufferHelper(const std::string& str)
	{
		size_t size = str.size();

		for (size_t i = 0; i < size; i++)
		{
			this->sendBuffer[this->sendBufferSize] = str[i];
			this->checkSum += str[i];
		
			this->sendBufferSize++;
		}
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, char>::value>::type Fix::writeToBufferHelper(T c)
	{
		this->sendBuffer[this->sendBufferSize++] = c;
		this->checkSum += c;
	}

	template<typename T>
	void Fix::writeToBufferHelper(const Lambda<T>& lambda)
	{
		lambda.lambda();
	}

	template<size_t size, typename T>
	void Fix::writeBytes(T value)
	{
		T* ptr = reinterpret_cast<T*>(&this->sendBuffer[this->sendBufferSize]);

		*ptr = value;
		this->sendBufferSize += size;
	}

	template<typename T, size_t ...Indices, size_t ...Sizes, size_t N>
	void Fix::unpack(const char(&arr)[N], std::index_sequence<Indices...>, std::integer_sequence<size_t, Sizes...>)
	{
		((this->writeBytes<Sizes>(getBytes<Indices, Indices + Sizes, T>(arr))), ...);
	}

	void Fix::writeTime()
	{
		using namespace std::chrono;

		auto now = system_clock::now();
		uint64_t millis = duration_cast<milliseconds>(now.time_since_epoch()).count();
		int millisTruncated = millis % 1000;
		auto time_t_now = system_clock::to_time_t(now);
		std::tm tm_utc;
		gmtime_r(&time_t_now, &tm_utc);
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-truncation"
		snprintf(&this->sendBuffer[this->sendBufferSize], 22, "%04d%02d%02d-%02d:%02d:%02d.%03d",
			tm_utc.tm_year + 1900, tm_utc.tm_mon + 1, tm_utc.tm_mday,
			tm_utc.tm_hour, tm_utc.tm_min, tm_utc.tm_sec,
			millisTruncated);
        #pragma GCC diagnostic pop
		this->computeCheckSum(21);
	}

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, char>::value>::type Fix::writeToBufferHelper(T num)
	{
		size_t i = 0;
		if (num == 0)
		{
			this->sendBuffer[this->sendBufferSize++] = '0';
			this->checkSum += '0';
			return;
		}

		while (num > 0)
		{
			char c = '0' + num % 10;
			this->sendBuffer[this->sendBufferSize + i] = c;
			this->checkSum += c;
			num /= 10;
			i++;
		}

		for (size_t j = 0; j < i / 2; j++)
		{
			std::swap(this->sendBuffer[this->sendBufferSize + j],
				this->sendBuffer[this->sendBufferSize + i - 1 - j]);
		}
		this->sendBufferSize += i;
	}

	void Fix::writeToBufferHelper(const Double& num)
	{
		size_t space = this->sendBufferSizeReserved - this->sendBufferSize;
		switch (num.nDigits)
		{
		case 1:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.1f", num.num);
			break;
		case 2:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.2f", num.num);
			break;
		case 3:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.3f", num.num);
			break;
		case 4:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.4f", num.num);
			break;
		case 5:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.5f", num.num);
			break;
		case 6:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.6f", num.num);
			break;
		case 7:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.7f", num.num);
			break;
		case 8:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.8f", num.num);
			break;
		case 9:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.9f", num.num);
			break;
		case 10:
			snprintf(&this->sendBuffer[this->sendBufferSize], space, "%.10f", num.num);
			break;
		default:
			break;
		}

		this->computeCheckSum(space);
	}

	void Fix::writeToBufferHelper(const Decimal& num)
	{
		size_t space = 0;
		this->checkSum += num.toString(&this->sendBuffer[this->sendBufferSize], space);
		this->sendBufferSize += space;
	}

	void Fix::reAllocateSendBuffer(size_t sizeNeeded)
	{
		size_t newsendBufferSizeReserved = std::max(this->sendBufferSizeReserved + this->sendBufferSizeReserved / 2, sizeNeeded);
		char* newSendBuffer = new char[newsendBufferSizeReserved];
		memcpy(newSendBuffer, this->sendBuffer, this->sendBufferSize);
		this->sendBuffer = newSendBuffer;
		this->sendBufferSizeReserved = newsendBufferSizeReserved;
	}

	void Fix::computeCheckSum(size_t n)
	{
		size_t newMsgSize = this->sendBufferSize + n;

		for (size_t i = this->sendBufferSize; i < newMsgSize; i++)
		{
			this->checkSum += this->sendBuffer[i];
		}

		this->sendBufferSize = newMsgSize;
	}

	void Fix::reciveMessage()
	{
		/*
		int result = recv(this->sock, this->recvBuffer, this->recvBufferSizeReserved, NULL);

		if (result > 0)
		{

		}
		else if (result == 0)
		{
			std::cerr << "Connection closed" << std::endl;
		}
		else
		{
			std::cerr << "recv failed: %d\n" << WSAGetLastError() << std::endl;
		}
		*/
	}

};