#include "ControlData.hpp"

#include <string.h>

ControlData::ControlData(void)
{
	euler = Vect3Dd();
	throttle = 0.0f;
}

ControlData::ControlData(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

IMessage::PreambleType ControlData::getPreambleType(void) const
{
	return IMessage::CONTROL;
}

void ControlData::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

IMessage::MessageType ControlData::getMessageType(void) const
{
    return CONTROL_DATA;
}

unsigned ControlData::getDataSize(void) const
{
	return sizeof(ControlData) - 4;
}

void ControlData::setEuler(const Vect3Df& _euler)
{
	euler = _euler;
}

void ControlData::setThrottle(const float _throttle)
{
	throttle = _throttle;
}

void ControlData::setControllerCommand(const ControllerCommand& _controllerCommand)
{
	controllerCommand = (unsigned short)_controllerCommand;
}

void ControlData::setSolverMode(const SolverMode& _solverMode)
{
	solverMode = (unsigned char)_solverMode;
}

const Vect3Df& ControlData::getEuler(void) const
{
	return euler;
}

float ControlData::getThrottle(void) const
{
	return throttle;
}

ControlData::ControllerCommand ControlData::getControllerCommand(void) const
{
	return (ControlData::ControllerCommand)controllerCommand;
}

ControlData::SolverMode ControlData::getSolverMode(void) const
{
	return (ControlData::SolverMode)solverMode;
}

void ControlData::setRoll(const float roll)
{
	euler.x = roll;
}

void ControlData::setPitch(const float pitch)
{
	euler.y = pitch;
}

void ControlData::setYaw(const float yaw)
{
	euler.z = yaw;
}
