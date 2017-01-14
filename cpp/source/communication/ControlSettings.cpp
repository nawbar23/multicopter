#include "ControlSettings.hpp"

#include <string.h>

#include "IMessage.hpp"
#include "SignalData.hpp"
#include "DebugData.hpp"

ControlSettings::ControlSettings(void)
{
}

ControlSettings::ControlSettings(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

void ControlSettings::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

unsigned ControlSettings::getDataSize(void) const
{
	return sizeof(ControlSettings) - 4;
}

SignalData::Command ControlSettings::getSignalDataType(void) const
{
	return SignalData::CONTROL_SETTINGS_DATA;
}

SignalData::Command ControlSettings::getSignalDataCommand(void) const
{
	return SignalData::CONTROL_SETTINGS;
}

IMessage::MessageType ControlSettings::getMessageType(void) const
{
    return CONTROL_SETTINGS;
}

bool ControlSettings::isValid(void) const
{
	switch (uavType)
	{
	case TRICOPTER:
	case QUADROCOPTER_X:
	case QUADROCOPTER_PLUS:
	case HEXACOPTER_X:
	case HEXACOPTER_PLUS:
	case OCTOCOPTER_X:
	case OCTOCOPTER_PLUS:
		break;
	default:
		return false;
	}

	switch (initialSolverMode)
	{
	case ControlData::STABLILIZATION:
	case ControlData::ANGLE_NO_YAW:
	case ControlData::ANGLE:
		break;
	default:
		return false;
	}

	switch (manualThrottleMode)
	{
	case STATIC:
	case DYNAMIC:
		break;
	default:
		return false;
	}

	switch (stickMovementMode)
	{
	case COPTER:
	case GEOGRAPHIC:
	case BASE_POINT:
		break;
	default:
		return false;
	}

	switch (batteryType)
	{
	case UNDEFINED:
	case BATTERY_2S:
	case BATTERY_3S:
	case BATTERY_4S:
	case BATTERY_5S:
	case BATTERY_6S:
		break;
	default:
		return false;
	}

	switch (errorHandlingAction)
	{
	case DebugData::AUTOLANDING:
	case DebugData::AUTOLANDING_AP:
	case DebugData::BACK_TO_BASE:
		break;
	default:
		return false;
	}

	if (autoLandingDescedRate < 0.0f)
	{
		return false;
	}

	unsigned char dataTab[sizeof(ControlSettings)];
	serialize(dataTab);
	return IMessage::computeCrc32(dataTab, getDataSize() - 4) == crcValue;
}

void ControlSettings::setCrc(void)
{
	unsigned char dataTab[sizeof(ControlSettings)];
	serialize(dataTab);
	crcValue = IMessage::computeCrc32(dataTab, getDataSize() - 4);
}

float ControlSettings::getBatteryErrorLevel(void) const
{
	return batteryType * 3.5f;
}

float ControlSettings::getBatteryWarningLevel(void) const
{
	return batteryType * 3.7f;
}

float ControlSettings::getBatteryMaxLevel(void) const
{
	return batteryType * 4.2f;
}

bool ControlSettings::isBatteryError(const float voltage) const
{
	if (batteryType != UNDEFINED && voltage > 5.0f)
	{
		return voltage < getBatteryErrorLevel();
	}
	else
	{
		return false;
	}
}

bool ControlSettings::isBatteryWarning(const float voltage) const
{
	if (batteryType != UNDEFINED && voltage > 5.0f)
	{
		return voltage < getBatteryWarningLevel();
	}
	else
	{
		return false;
	}
}

float ControlSettings::getBatteryChargeLevel(const float voltage) const
{
	if (batteryType != UNDEFINED && voltage > getBatteryErrorLevel())
	{
		if (voltage > getBatteryMaxLevel())
		{
			return 100.0f;
		}
		else
		{
			return ((voltage - getBatteryErrorLevel()) * 100.0f) / getBatteryMaxLevel();
		}
	}
	else
	{
		return 0.0f;
	}
}

ControlData::SolverMode ControlSettings::getInitialSolverMode(void) const
{
	return (ControlData::SolverMode)initialSolverMode;
}

ControlData ControlSettings::formatEulers(const ControlData& controlData) const
{
	ControlData result(controlData);
	Vect3Df euler;
	euler.x *= maxRollPitchControlValue;
	euler.y *= maxRollPitchControlValue;
	euler.z *= maxYawControlValue;
	result.setEuler(euler);
	return result;
}

ControlSettings ControlSettings::createDefault(void)
{
	ControlSettings defaultControlSettings;
	defaultControlSettings.rollProp = 0.0f;
	defaultControlSettings.pitchProp = 0.0f;
	defaultControlSettings.yawProp = 0.0f;
	defaultControlSettings.autoLandingDescedRate = 0.0f;
	defaultControlSettings.maxAutoLandingTime = 0.0f;
	defaultControlSettings.maxRollPitchControlValue = 0.0f;
	defaultControlSettings.maxYawControlValue = 0.0f;

	// throttle controller
	defaultControlSettings.altPositionProp = 0.0f;
	defaultControlSettings.altVelocityProp = 0.0f;
	defaultControlSettings.throttleAltRateProp = 0.0f;

	// autopilot
	defaultControlSettings.autoPositionProp = 0.0f;
	defaultControlSettings.autoVelocityProp = 0.0f;
	defaultControlSettings.maxAutoAngle = 0.0f;
	defaultControlSettings.maxAutoVelocity = 0.0f;
	defaultControlSettings.stickPositionRateProp = 0.0f;
	defaultControlSettings.stickMovementMode = COPTER;

	defaultControlSettings.batteryType = UNDEFINED;

	defaultControlSettings.errorHandlingAction = DebugData::AUTOLANDING;

	defaultControlSettings.escPwmFreq = MEDIUM;

	defaultControlSettings.gpsSensorPosition = Vect3Df();

	defaultControlSettings.flags = 0;

	return defaultControlSettings;
}

bool ControlSettings::isUavBisymetricalFromData(void) const
{
	return (rollProp == pitchProp && pidRollRate == pidPitchRate);
}

ControlData ControlSettings::getControlDataForLogs(const ControlData& controlData) const
{
	ControlData result(controlData);
	Vect3Df resultEuler;
	resultEuler.x *= maxRollPitchControlValue;
	resultEuler.y *= maxRollPitchControlValue;
	resultEuler.z *= maxYawControlValue;
	result.setEuler(resultEuler);
	return result;
}
