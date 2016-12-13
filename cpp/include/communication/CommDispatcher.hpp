// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __COMM_DISPATCHER__
#define __COMM_DISPATCHER__

#include "IMessage.hpp"

#include "SignalData.hpp"

class CommDispatcher
{
public:
	CommDispatcher(void);
	~CommDispatcher(void);

	// resets all buffers and data
	void reset(void);

	const IMessage::PreambleType& getPreambleType(void) const;
	const unsigned char* getDataBuffer(void) const;
	const unsigned char* getSignalDataBuffer(void) const;

	void cleanSignalDataBuffer(void);

	unsigned getSucessfullReceptionCounter(void) const;
	unsigned getFailedReceptionCounter(void) const;

	void clearCounters(void);

	IMessage::PreambleType putChar(unsigned char data);

private:
	unsigned char preambleBuffer[IMessage::PREAMBLE_SIZE - 1];
	unsigned char dataBuffer[IMessage::MAX_DATA_SIZE];

	unsigned preambleBufferCounter;
	unsigned dataBufferCounter;

	bool isPreambleActive;
	IMessage::PreambleType activePreambleType;

	unsigned targetDataBufferCounter;

	unsigned failedReceptionCounter;
	unsigned sucessfullReceptionCounter;

	IMessage::PreambleType updatePreamble(unsigned char data);
	void activatePreamble(IMessage::PreambleType preambleType);

	void updataTargetDataSizeWithCommand(void);

	bool receivingSignalData;
	SignalData::Command receivedSignalData;
	unsigned char* signalDataBuffer;
	unsigned signalDataPacketsToReceive;
	unsigned signalDataPacketsReceived;

	void initSignalDataPayloadReception(const SignalData::Command& command, const unsigned short allPackets);
	void handleSignalDataPayloadReception(void);
	bool isSignalDataComplete(void);

	bool isValidMessageCrc(void) const;

	void deactivatePreamble(void);
};

#endif // __COMM_DISPATCHER__
