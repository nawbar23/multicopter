#include "CommDispatcher.hpp"

#include <string.h>

#ifdef STM32F40_41xxx
#include "Tracer.h"
#endif

CommDispatcher::CommDispatcher(void)
{
	reset();
}

CommDispatcher::~CommDispatcher(void)
{
	cleanSignalDataBuffer();
}

void CommDispatcher::reset(void)
{
	preambleBufferCounter = 0;
	dataBufferCounter = 0;

	isPreambleActive = false;
	activePreambleType = IMessage::EMPTY;

	targetDataBufferCounter = 0;

	failedReceptionCounter = 0;
	sucessfullReceptionCounter = 0;

	receivingSignalData = false;
	receivedSignalData = SignalData::DUMMY;
	signalDataBuffer = NULL;
	signalDataPacketsToReceive = 0;
	signalDataPacketsReceived = 0;
}

const IMessage::PreambleType& CommDispatcher::getPreambleType(void) const
{
	return activePreambleType;
}

const unsigned char* CommDispatcher::getDataBuffer(void) const
{
	return dataBuffer;
}

const unsigned char* CommDispatcher::getSignalDataBuffer(void) const
{
	return signalDataBuffer;
}

void CommDispatcher::cleanSignalDataBuffer(void)
{
	if (signalDataBuffer != NULL)
	{
		delete[] signalDataBuffer;
		signalDataBuffer = NULL;
	}
}

unsigned CommDispatcher::getSucessfullReceptionCounter(void) const
{
	return sucessfullReceptionCounter;
}

unsigned CommDispatcher::getFailedReceptionCounter(void) const
{
	return failedReceptionCounter;
}

void CommDispatcher::clearCounters(void)
{
	sucessfullReceptionCounter = 0;
	failedReceptionCounter = 0;
}

IMessage::PreambleType CommDispatcher::putChar(unsigned char data)
{
	// check for new preamble
	const IMessage::PreambleType newPreamble = updatePreamble(data);
	if (newPreamble != IMessage::EMPTY)
	{
		if (isPreambleActive)
		{
			// new preamble received when previous reception not ready, some fail
			//std::cout << "\nFAIL:  new preamble received when previous reception not ready\n\n";
#ifdef TRACER_H_
			Tracer::Trace("New preamble received when previous reception not ready");
#endif // TRACER_H_
			failedReceptionCounter++;
		}
		activatePreamble(newPreamble);
		return IMessage::EMPTY;
	}

	if (isPreambleActive)
	{
		// proceed processing data
		dataBuffer[dataBufferCounter] = data;
		dataBufferCounter++;

		if (dataBufferCounter >= targetDataBufferCounter)
		{
			// enough data in data buffer

			// check signal message condition
			if (activePreambleType == IMessage::SIGNAL
				&& dataBufferCounter == IMessage::SIGNAL_CONSTRAINT_SIZE)
			{
				// command from signal message just received, update target
				updataTargetDataSizeWithCommand();
				return IMessage::EMPTY;
			}

			// check CRC condition
			if (isValidMessageCrc())
			{
				// data received succesfully!
				sucessfullReceptionCounter++;
				IMessage::PreambleType result = activePreambleType;
				if (activePreambleType == IMessage::SIGNAL &&
					SignalData::hasPayload(SignalData::parseCommand(dataBuffer)))
				{
					handleSignalDataPayloadReception();
					if (!isSignalDataComplete())
					{
						result = IMessage::EMPTY;
					}
					else
					{
						receivingSignalData = false;
					}
				}
				else
				{
					if (receivingSignalData)
					{
						//std::cout << "\nFAIL: receiving SignalData not ready\n\n";
						failedReceptionCounter++;
#ifdef TRACER_H_
						Tracer::Trace("Receiving SignalData not ready");
#endif // TRACER_H_
					}
					receivingSignalData = false;
				}
				deactivatePreamble();
				return result;
			}
			else
			{
				// something gone wrong, reset processor
				//std::cout << "\nFAIL: wrong CRC\n\n";
				failedReceptionCounter++;
#ifdef TRACER_H_
				Tracer::Trace("Wrong CRC");
#endif // TRACER_H_
				deactivatePreamble();
				return IMessage::EMPTY;
			}
		}
	}
	return IMessage::EMPTY;
}

IMessage::PreambleType CommDispatcher::updatePreamble(unsigned char data)
{
	// new preamble is arrived when all preamble bytes are the same
	// and equals to known preamlbe type
	// and the last received byte is equal to 0
	IMessage::PreambleType result = IMessage::EMPTY;
	if (data == 0)
	{
		bool allEquals = true;
		for (unsigned i = 0; i < IMessage::PREAMBLE_SIZE - 1; i++)
		{
			if (preambleBuffer[0] != preambleBuffer[i])
			{
				allEquals = false;
				break;
			}
		}
		if (allEquals)
		{
			result = IMessage::getPreabmleTypeByChar(preambleBuffer[0]);
		}
	}

 	preambleBuffer[preambleBufferCounter] = data;
	preambleBufferCounter++;
	if (preambleBufferCounter >= IMessage::PREAMBLE_SIZE - 1)
	{
		preambleBufferCounter = 0;
	}

	return result;
}

void CommDispatcher::activatePreamble(IMessage::PreambleType preambleType)
{
	for (unsigned i = 0; i < IMessage::PREAMBLE_SIZE - 1; i++)
	{
		preambleBuffer[0] = 0;
	}

	isPreambleActive = true;
	activePreambleType = preambleType;

	dataBufferCounter = 0;
	targetDataBufferCounter = IMessage::getPayloadSizeByType(preambleType);
	if (activePreambleType != IMessage::SIGNAL)
	{
		targetDataBufferCounter += IMessage::CRC_SIZE;
	}
}

void CommDispatcher::updataTargetDataSizeWithCommand(void)
{
	const SignalData::Command command = SignalData::parseCommand(dataBuffer);
	if (SignalData::hasPayload(command))
	{
		targetDataBufferCounter += IMessage::SIGNAL_DATA_PAYLOAD_SIZE + IMessage::CRC_SIZE;
	}
	else
	{
		targetDataBufferCounter += IMessage::CRC_SIZE;
	}
}

void CommDispatcher::initSignalDataPayloadReception(const SignalData::Command& command, const unsigned short allPackets)
{
	cleanSignalDataBuffer();
	receivedSignalData = command;
	signalDataBuffer = new unsigned char[allPackets * IMessage::SIGNAL_DATA_PAYLOAD_SIZE];
	signalDataPacketsToReceive = allPackets;
	receivingSignalData = true;
	signalDataPacketsReceived = 0;
}

void CommDispatcher::handleSignalDataPayloadReception(void)
{
	const SignalData::Command command = SignalData::parseCommand(dataBuffer);
	const unsigned short allPackets = SignalData::parseAllPacketsNumber(dataBuffer);
	const unsigned short packetNumber = SignalData::parseActualPacketNumber(dataBuffer);

	if (!receivingSignalData || receivedSignalData != command)
	{
		// new data or another data was being received
		initSignalDataPayloadReception(command, allPackets);
	}
	// put data to bufer in to reported position
	memcpy(signalDataBuffer + packetNumber * IMessage::SIGNAL_DATA_PAYLOAD_SIZE,
		dataBuffer + IMessage::SIGNAL_CONSTRAINT_SIZE,
		IMessage::SIGNAL_DATA_PAYLOAD_SIZE);
	signalDataPacketsReceived++;
}

bool CommDispatcher::isSignalDataComplete(void)
{
	return signalDataPacketsToReceive == signalDataPacketsReceived;
}


bool CommDispatcher::isValidMessageCrc(void) const
{
	const unsigned short crc = IMessage::computeCrc16(dataBuffer, dataBufferCounter - 2);
	unsigned char crc1 = (unsigned char)(crc & 0xff);
	unsigned char crc2 = (unsigned char)((crc >> 8) & 0xff);
	return	dataBuffer[targetDataBufferCounter - 2] == crc1
		&& dataBuffer[targetDataBufferCounter - 1] == crc2;
}

void CommDispatcher::deactivatePreamble(void)
{
	isPreambleActive = false;
	activePreambleType = IMessage::EMPTY;
	dataBufferCounter = 0;
	targetDataBufferCounter = 0;
}
