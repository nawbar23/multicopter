#include "ICommHandler.hpp"

#include <string.h>

#ifdef STM32F40_41xxx
#include "Tracer.h"
#endif

ICommHandler::~ICommHandler(void)
{
	// nothing to do here
}

void ICommHandler::initialize(ICommInterface* _commInterface)
{
	commInterface = _commInterface;
    clearCounters();
	commDispatcher.reset();
	sentMessages = 0;
}

const ICommInterface* ICommHandler::getCommHandle(void) const
{
	return commInterface;
}

IMessage::PreambleType ICommHandler::proceedReceiving(void)
{
	unsigned char data;
	while (commInterface->getData(&data))
	{
		IMessage::PreambleType received = commDispatcher.putChar(data);
		if (received != IMessage::EMPTY)
		{
			// if stream processor reports data received return immadetely
			return received;
		}
	}
	return IMessage::EMPTY;
}

bool ICommHandler::send(const unsigned char* data, const unsigned dataSize)
{
	sentMessages++;
	return commInterface->sendData(data, dataSize);
}

bool ICommHandler::send(const IMessage& message)
{
	unsigned char* data = message.createMessage();
	const bool result = send(data, message.getMessageSize());
	// always do cleanup when called createMessage 
	delete[] data;
	return result;
}

bool ICommHandler::send(const ISignalPayloadMessage& message)
{
	unsigned char* data = new unsigned char[message.getDataSize()];
	message.serialize(data);
	const bool result = sendSignalData(data, message.getDataSize(), message.getSignalDataType());
	// cleanup
	delete[] data;
	return result;
}

bool ICommHandler::send(const SignalData& message)
{
	unsigned char data[14];
	data[0] = '%';
	data[1] = '%';
	data[2] = '%';
	data[3] = 0;

	message.serialize(data + 4);

	const unsigned short crcValue = IMessage::computeCrc16(data + 4, 8);
	data[12] = (unsigned char)(crcValue & 0xff);
	data[13] = (unsigned char)((crcValue >> 8) & 0xff);

	return send(data, 14);
}

bool ICommHandler::send(const CalibrationSettings& calibrationSettings)
{
	const unsigned dataSize = sizeof(CalibrationSettings);
	unsigned char data[dataSize];
	calibrationSettings.serialize(data);
	return sendSignalData(data, dataSize, SignalData::CALIBRATION_SETTINGS_DATA);
}

bool ICommHandler::send(const ControlSettings& controlSettings)
{
	const unsigned dataSize = sizeof(ControlSettings);
	unsigned char data[dataSize];
	controlSettings.serialize(data);
	return sendSignalData(data, dataSize, SignalData::CONTROL_SETTINGS_DATA);
}

bool ICommHandler::send(const RouteContainer& routeContainer)
{
	const unsigned dataSize = routeContainer.getBinarySize();
	unsigned char *data = new unsigned char[dataSize];
	routeContainer.serialize(data);
	const bool result = sendSignalData(data, dataSize, SignalData::ROUTE_CONTAINER_DATA);
	// becouse of vary of route size data array has to be dynamic
	delete[] data;
	return result;
}

SignalData ICommHandler::getSignalData(void) const
{
	return SignalData(commDispatcher.getDataBuffer());
}

SignalData::Command ICommHandler::getCommand(void) const
{
	return SignalData::parseCommand(commDispatcher.getDataBuffer());
}

SignalData::CommandParameter ICommHandler::getParameter(void) const
{
	return SignalData::parseCommandParameter(commDispatcher.getDataBuffer() + IMessage::SIGNAL_COMMAND_SIZE);
}

void ICommHandler::getSignalDataObject(ISignalPayloadMessage& data)
{
	switch (data.getSignalDataCommand())
	{
	case SignalData::CALIBRATION_SETTINGS:
		reinterpret_cast<CalibrationSettings&>(data) = CalibrationSettings(commDispatcher.getSignalDataBuffer());
		break;
	case SignalData::CONTROL_SETTINGS:
		reinterpret_cast<ControlSettings&>(data) = ControlSettings(commDispatcher.getSignalDataBuffer());
		break;
	case SignalData::ROUTE_CONTAINER:
		reinterpret_cast<RouteContainer&>(data) = RouteContainer(commDispatcher.getSignalDataBuffer());
		break;
	default:
		// error should never reach this point
		break;
	}
	commDispatcher.cleanSignalDataBuffer();
}

CalibrationSettings ICommHandler::getCalibrationSettings(void)
{
	const CalibrationSettings result(commDispatcher.getSignalDataBuffer());
	commDispatcher.cleanSignalDataBuffer();
	return result;
}

ControlSettings ICommHandler::getControlSettings(void)
{
	const ControlSettings result(commDispatcher.getSignalDataBuffer());
	commDispatcher.cleanSignalDataBuffer();
	return result;
}

RouteContainer ICommHandler::getRouteContainer(void)
{
	const RouteContainer result(commDispatcher.getSignalDataBuffer());
	commDispatcher.cleanSignalDataBuffer();
	return result;
}

DebugData ICommHandler::getDebugData(void) const
{
	return DebugData(commDispatcher.getDataBuffer());
}

ControlData ICommHandler::getControlData(void) const
{
	return ControlData(commDispatcher.getDataBuffer());
}

SensorsData ICommHandler::getSensorsData(void) const
{
	return SensorsData(commDispatcher.getDataBuffer());
}

AutopilotData ICommHandler::getAutopilotData(void) const
{
	return AutopilotData(commDispatcher.getDataBuffer());
}

unsigned ICommHandler::getSentMessages(void) const
{
	return sentMessages;
}

unsigned ICommHandler::getReceivedMessage(void) const
{
	return commDispatcher.getSucessfullReceptionCounter();
}

unsigned ICommHandler::getReceptionFailes(void) const
{
	return commDispatcher.getFailedReceptionCounter();
}

void ICommHandler::clearCounters(void)
{
	sentMessages = 0;
	commDispatcher.clearCounters();
}

bool ICommHandler::sendCommand(const SignalData& command)
{
    return send(command);
}

bool ICommHandler::waitForAnyCommand(SignalData& command, const unsigned timeout)
{
    resetTimer();
    while (true) // infinite loop
    {
		if (proceedReceiving() == IMessage::SIGNAL)
		{
			SignalData::Command commandValue = getCommand();
			if (!SignalData::hasPayload(commandValue))
			{
				command = getSignalData();
				return true;
			}
		}
		if (getTimerValue() > timeout)
		{
			return false;
		}
		holdThread(10);
    }
}

bool ICommHandler::waitForCommand(const SignalData& command, const unsigned timeout)
{
	SignalData receivedCommand;
    return waitForAnyCommand(receivedCommand, timeout)
		&& receivedCommand == command;
}

bool ICommHandler::sendCommandGetAnyResponse(const SignalData& command, SignalData& response)
{
    if (!send(command))
    {
        return false;
    }
	return waitForAnyCommand(response);
}

bool ICommHandler::sendCommandGetResponse(const SignalData& command, const SignalData& response)
{
	SignalData receivedCommand;
	return sendCommandGetAnyResponse(command, receivedCommand)
		&& receivedCommand == response;
}

bool ICommHandler::readCommandAndRespond(const SignalData& command, const SignalData& response)
{
	if (waitForCommand(command))
	{
		return sendCommand(response);
	}
	else
	{
		return false;
	}
}

bool ICommHandler::sendDataProcedure(const ISignalPayloadMessage& data)
{
	for (unsigned ret = 0; ret < MAX_RETRANSMISSION + 1; ret++)
	{
		if (!send(data))
		{
			// sending data failed
			return false;
		}
		SignalData receivedCommand;
		if (waitForAnyCommand(receivedCommand, DEFAULT_TIMEOUT + 500)
			&& receivedCommand.getCommand() == data.getSignalDataCommand())
		{
			switch (receivedCommand.getParameter())
			{
			case SignalData::ACK:
				// sending data successful
				return true;

			case SignalData::BAD_CRC:
#ifdef TRACER_H_
				Tracer::Trace("sendDataProcedure::Bad CRC!", true);
#endif
			case SignalData::TIMEOUT:
				// retransmit data
#ifdef TRACER_H_
				Tracer::Trace("sendDataProcedure::Timeout, retransmitting", true);
#endif
				break;

			default:
				// bad parameter received
#ifdef TRACER_H_
				Tracer::Trace("sendDataProcedure::Bad parameter received!", true);
#endif
				return false;
			}
		}
		else
		{
			// bad command received
#ifdef TRACER_H_
			Tracer::Trace("sendDataProcedure::Bad command received!", true);
#endif
			return false;
		}
	}
	// failed after max retransmissions
	return false;
}

bool ICommHandler::receiveDataProcedure(ISignalPayloadMessage& data)
{
	resetTimer();
	unsigned retransmissionCounter = 0;
	while (true) // infinite loop
	{
		if (proceedReceiving() == IMessage::SIGNAL
			&& getCommand() == data.getSignalDataType())
		{
			getSignalDataObject(data);
			if (data.isValid())
			{
				send(SignalData(data.getSignalDataCommand(), SignalData::ACK));
				return true;
			}
			else
			{
#ifdef TRACER_H_
				Tracer::Trace("receiveDataProcedure::Bad CRC!", true);
#endif
				retransmissionCounter++;
				if (retransmissionCounter >= MAX_RETRANSMISSION + 1)
				{
					// failed after max retransmissions
					break;
				}
				send(SignalData(data.getSignalDataCommand(), SignalData::BAD_CRC));
				resetTimer();
			}
		}
		if (getTimerValue() > DEFAULT_TIMEOUT)
		{
#ifdef TRACER_H_
			Tracer::Trace("receiveDataProcedure::Timeout!", true);
#endif
			retransmissionCounter++;
			if (retransmissionCounter >= MAX_RETRANSMISSION + 1)
			{
				// failed after max retransmissions
				break;
			}
			send(SignalData(data.getSignalDataCommand(), SignalData::TIMEOUT));
			resetTimer();
		}
		holdThread(10);
	}
#ifdef TRACER_H_
	Tracer::Trace("receiveDataProcedure::Max retransmissions reached!", true);
#endif
	return false;
}

bool ICommHandler::sendSignalData(const unsigned char* data, const unsigned dataSize, const SignalData::Command type)
{
	const unsigned messageSize = IMessage::PREAMBLE_SIZE + IMessage::SIGNAL_CONSTRAINT_SIZE
		+ IMessage::SIGNAL_DATA_PAYLOAD_SIZE
		+ IMessage::CRC_SIZE;

	const unsigned short messagesCount = 
		(unsigned short)((double)dataSize / IMessage::SIGNAL_DATA_PAYLOAD_SIZE + 0.99999);

	unsigned char message[messageSize] = {};

	// preamble
	const unsigned char preambleChar = IMessage::getPreambleCharByType(IMessage::SIGNAL);
	for (unsigned i = 0; i < IMessage::PREAMBLE_SIZE - 1; i++)
	{
		message[i] = preambleChar;
	}
	message[IMessage::PREAMBLE_SIZE - 1] = 0;

	// command
	const int command = (int)type;
	memcpy(message + IMessage::PREAMBLE_SIZE, &command, 4);

	// max packets
	memcpy(message + IMessage::PREAMBLE_SIZE + 4, &messagesCount, 2);

	for (unsigned short i = 0; i < messagesCount; i++)
	{
		// actual packet
		memcpy(message + IMessage::PREAMBLE_SIZE + 4 + 2, &i, 2);

		// payload
		if (i == messagesCount - 1)
		{
			// last packet, copy only bytes that left in data buffer
			const unsigned sent = (messagesCount - 1) * IMessage::SIGNAL_DATA_PAYLOAD_SIZE;
			const unsigned left = dataSize - sent;
			memcpy(message + IMessage::PREAMBLE_SIZE + IMessage::SIGNAL_CONSTRAINT_SIZE,
				data + i * IMessage::SIGNAL_DATA_PAYLOAD_SIZE,
				left);
		}
		else
		{
			memcpy(message + IMessage::PREAMBLE_SIZE + IMessage::SIGNAL_CONSTRAINT_SIZE,
				data + i * IMessage::SIGNAL_DATA_PAYLOAD_SIZE,
				IMessage::SIGNAL_DATA_PAYLOAD_SIZE);
		}

		// crc
		const unsigned short crcValue = IMessage::computeCrc16(message + IMessage::PREAMBLE_SIZE,
			SignalData::SIGNAL_CONSTRAINT_SIZE + SignalData::SIGNAL_DATA_PAYLOAD_SIZE);
		message[messageSize - 2] = (unsigned char)(crcValue & 0xff);
		message[messageSize - 1] = (unsigned char)((crcValue >> 8) & 0xff);

		// sending message
		if (!send(message, messageSize))
		{
			return false;
		}
	}
	return true;
}
