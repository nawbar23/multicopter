#include "AutopilotData.hpp"

#include <string.h>

AutopilotData::AutopilotData(void)
{

}

AutopilotData::AutopilotData(const unsigned char* src)
{
	// pointer is offseted because of vptr and padding for double
	memcpy((unsigned char*)this + 8, src, getDataSize());
}

IMessage::PreambleType AutopilotData::getPreambleType(void) const
{
	return IMessage::AUTOPILOT;
}

void AutopilotData::serialize(unsigned char* dst) const
{
	// pointer is offseted because of vptr and padding for double
	memcpy(dst, (unsigned char*)this + 8, getDataSize());
}

IMessage::MessageType AutopilotData::getMessageType(void) const
{
    return AUTOPILOT_DATA;
}

unsigned AutopilotData::getDataSize(void) const
{
	// size is lower because od vptr and 8 byte padding (double in position)
	return sizeof(AutopilotData) - 8;
}

const Vect2Dd& AutopilotData::getTargetPosition(void) const
{
	return targetPosition;
}

float AutopilotData::getTargetAltitude(void) const
{
	return targetAltitude;
}

void AutopilotData::setTargetPosition(const Vect2Dd& _targetPosition)
{
	targetPosition = _targetPosition;
}

void AutopilotData::setTargetAltitude(const float _targetAltitude)
{
	targetAltitude = _targetAltitude;
}

const Flags<int>& AutopilotData::flags(void) const
{
	return flagsObj;
}

Flags<int>& AutopilotData::flags(void)
{
	return flagsObj;
}
