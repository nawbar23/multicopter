#include "DebugData.hpp"

#include <string.h>

DebugData::DebugData(void)
{
}

DebugData::DebugData(const StateVector& stateVector)
{
	flagsObj.getFlagsVector() = 0;
	euler = stateVector.getEulerAngles();
	position = Vect2Df(stateVector.getPosition());
	altitude = stateVector.getRelativeAltitude();
	velocity = stateVector.getVLoc().getVect2D().getNorm();
	setGpsFlags(stateVector.gpsFix);
	setBatteryVoltage(stateVector.batteryVoltage);
}

DebugData::DebugData(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

IMessage::PreambleType DebugData::getPreambleType(void) const
{
	return IMessage::CONTROL;
}

void DebugData::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

unsigned DebugData::getDataSize(void) const
{
	return sizeof(DebugData) - 4;
}

void DebugData::setEuler(const Vect3Df& _euler)
{
	euler = _euler;
}

void DebugData::setPosition(const Vect2Df& _position)
{
	position = _position;
}

void DebugData::setAltitude(const float _altitude)
{
	altitude = _altitude;
}

void DebugData::setVelocity(const float _velocity)
{
	velocity = _velocity;
}

void DebugData::setControllerState(const ControllerState& _controllerState)
{
	controllerState = (unsigned short)_controllerState;
}

void DebugData::setSolverMode(const ControlData::SolverMode& solverMode)
{
	// clean previous state 
	flagsObj.getFlagsVector() &= 0xFC;
	// set new solver flags
	flagsObj.getFlagsVector() |= solverMode;
}

void DebugData::setBatteryVoltage(const float voltage)
{
	const float maxVoltage = 3.3f * 11.0f;
	battery = (unsigned char)(((255 * voltage) / maxVoltage) + 0.5f);
}

const Vect3Df& DebugData::getEuler(void) const
{
	return euler;
}

const Vect2Df& DebugData::getPosition(void) const
{
	return position;
}

float DebugData::getAltitude(void) const
{
	return altitude;
}

float DebugData::getVelocity(void) const
{
	return velocity;
}

DebugData::ControllerState DebugData::getControllerState(void) const
{
	return (DebugData::ControllerState)controllerState;
}

ControlData::SolverMode DebugData::getSolverMode(void) const
{
	return (ControlData::SolverMode)(flagsObj.getFlagsVector() & 0x03);
}

float DebugData::getBatteryVoltage(void) const
{
	const float maxVoltage = 3.3f * 11.0f;
	return (battery * maxVoltage) / 255.0f;
}

const Flags<unsigned char>& DebugData::flags(void) const
{
	return flagsObj;
}

Flags<unsigned char>& DebugData::flags(void)
{
	return flagsObj;
}

void DebugData::setGpsFlags(const StateVector::GpsFix& gpsFix)
{
	switch (gpsFix)
	{
	case StateVector::NO_FIX:
		flagsObj.setFlagState(GPS_FIX, false); // no GPS fix
		flagsObj.setFlagState(GPS_FIX_3D, false); // no 3D fix
		break;
	case StateVector::FIX:
		flagsObj.setFlagState(GPS_FIX, true); // GPS fixed
		flagsObj.setFlagState(GPS_FIX_3D, false); // no 3D fix
		break;
	case StateVector::FIX_3D:
		flagsObj.setFlagState(GPS_FIX, true); // GPS fixed
		flagsObj.setFlagState(GPS_FIX_3D, true); // 3D fixed
		break;
	}
}

void DebugData::setNoConnection()
{
	controllerState = ControlData::ERROR_CONNECTION;
}

bool DebugData::isGpsFixed(void) const
{
	return flagsObj.getFlagState(GPS_FIX);
}

bool DebugData::isGps3DFixed(void) const
{
	return flagsObj.getFlagState(GPS_FIX_3D);
}

bool DebugData::isBatteryWaring(void) const
{
	return flagsObj.getFlagState(LOW_BATTERY_VOLTAGE);
}

bool DebugData::isAutopilotUsed(void) const
{
	return flagsObj.getFlagState(AUTOPILOT_ENABLED);
}

bool DebugData::isAutolanding(void) const
{
	return flagsObj.getFlagState(AUTOLANDING_ENABLED);
}

bool DebugData::isErrorHandling(void) const
{
	return flagsObj.getFlagState(ERROR_HANDLING);
}

bool DebugData::isDrawable(void) const
{
	// euler angles
	if (euler.x < -roboLib::pi || euler.x > roboLib::pi)
	{
		return false;
	}
	if (euler.y < -roboLib::pi / 2 || euler.y > roboLib::pi / 2)
	{
		return false;
	}
	if (euler.z < -roboLib::pi || euler.z > roboLib::pi)
	{
		return false;
	}
	// position
	if (position.x < -90.0f || position.x > 90.0f)
	{
		return false;
	}
	if (position.y < -90.0f || position.y > 90.0f)
	{
		return false;
	}
	return true;
}

bool DebugData::isConnection(void) const
{
	return controllerState != ControlData::ERROR_CONNECTION;
}

float DebugData::getNormalYaw(void) const
{
	float yaw = getEuler().z;
	if (yaw > 2 * roboLib::pi) yaw -= float(2 * roboLib::pi);
	else if (yaw < 0.0f) yaw += float(2 * roboLib::pi);
	return yaw;
}

#endif //__MULTICOPTER_USER_APP__
