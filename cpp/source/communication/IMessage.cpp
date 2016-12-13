#include "IMessage.hpp"

unsigned IMessage::getPayloadSize(void) const
{
	return getPayloadSizeByType(getPreambleType());
}

unsigned IMessage::getMessageSize(void) const
{
	return PREAMBLE_SIZE + getPayloadSize() + CRC_SIZE;
}

void IMessage::serializeMessage(unsigned char* data) const
{
	const unsigned size = getMessageSize();

	const unsigned char preambleChar = getPreambleCharByType(getPreambleType());
	for (unsigned i = 0; i < PREAMBLE_SIZE - 1; i++)
	{
		data[i] = preambleChar;
	}
	data[PREAMBLE_SIZE - 1] = 0;

	// payload
	serialize(data + PREAMBLE_SIZE);

	// crc
	const unsigned short crcValue = computeCrc16(data + PREAMBLE_SIZE, getPayloadSize());
	data[size - 2] = (unsigned char)(crcValue & 0xff);
	data[size - 1] = (unsigned char)((crcValue >> 8) & 0xff);
}

unsigned char* IMessage::createMessage(void) const
{
	const unsigned size = getMessageSize();
	unsigned char* message = new unsigned char[size]();

	// preamble
	const unsigned char preambleChar = getPreambleCharByType(getPreambleType());
	for (unsigned i = 0; i < PREAMBLE_SIZE - 1; i++)
	{
		message[i] = preambleChar;
	}
	message[PREAMBLE_SIZE - 1] = 0;

	// payload
	serialize(message + PREAMBLE_SIZE);

	// crc
	const unsigned short crcValue = computeCrc16(message + PREAMBLE_SIZE, getPayloadSize());
	message[size - 2] = (unsigned char)(crcValue & 0xff);
	message[size - 1] = (unsigned char)((crcValue >> 8) & 0xff);

	return message;
}

IMessage::~IMessage(void)
{
	// nothing to do here
}

unsigned char IMessage::getPreambleCharByType(const IMessage::PreambleType type)
{
	switch (type)
	{
	case CONTROL:
		return '$';

	case SIGNAL:
		return '%';

	case AUTOPILOT:
		return '^';

	default:
		// TODO handle this error in some way that uC can do
		return 'a';
	}
}

IMessage::PreambleType IMessage::getPreabmleTypeByChar(const unsigned char data)
{
	switch (data)
	{
	case '$':
		return CONTROL;

	case '%':
		return SIGNAL;

	case '^':
		return AUTOPILOT;

	default:
		// TODO handle this error in some way that uC can do
		return EMPTY;
	}
}

unsigned IMessage::getPayloadSizeByType(const IMessage::PreambleType type)
{
	switch (type)
	{
	case CONTROL:
		return 32;

	case SIGNAL:
		return 8;

	case AUTOPILOT:
		return 24;

	default:
		// TODO handle this error in some way that uC can do
		return 0;
	}
}

unsigned short IMessage::computeCrc16(const unsigned char* data, const unsigned dataSize)
{
	int crcShort = 0;
	for (unsigned i = 0; i < dataSize; i++)
	{
		crcShort = ((crcShort >> 8) | (crcShort << 8)) & 0xffff;
		crcShort ^= (data[i] & 0xff);
		crcShort ^= ((crcShort & 0xff) >> 4);
		crcShort ^= (crcShort << 12) & 0xffff;
		crcShort ^= ((crcShort & 0xFF) << 5) & 0xffff;
	}
	crcShort &= 0xffff;
	return (unsigned short)crcShort;
}

unsigned IMessage::computeCrc32(const unsigned char* data, const unsigned dataSize)
{
	const unsigned CRC_POLY = 0x82f63b78;
	unsigned len = dataSize;
	int k;
	unsigned crc = 0;
	crc = ~crc;
	while (len--)
	{
		crc ^= *data++;
		for (k = 0; k < 8; k++)
		{
			crc = crc & 1 ? (crc >> 1) ^ CRC_POLY : crc >> 1;
		}
	}
	return ~crc;
}
