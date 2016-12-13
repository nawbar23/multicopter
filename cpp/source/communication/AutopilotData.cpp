#include "AutopilotData.hpp"

#include <string.h>

AutopilotData::AutopilotData(void)
{

}

AutopilotData::AutopilotData(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

IMessage::PreambleType AutopilotData::getPreambleType(void) const
{
	return IMessage::CONTROL;
}

void AutopilotData::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

unsigned AutopilotData::getDataSize(void) const
{
	return sizeof(AutopilotData) - 4;
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
