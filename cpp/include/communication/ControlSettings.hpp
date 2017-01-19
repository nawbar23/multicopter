// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __CONTROL_SETTINGS__
#define __CONTROL_SETTINGS__

#include "MathCore.hpp"

#include "ISignalPayloadMessage.hpp"

#include "SignalData.hpp"
#include "ControlData.hpp"
#include "Flags.hpp"

class ControlSettings : public ISignalPayloadMessage
{
public:
	enum UavType
	{
		TRICOPTER = 1000,
		QUADROCOPTER_X = 2000,
		QUADROCOPTER_PLUS = 2500,
		HEXACOPTER_X = 3000,
		HEXACOPTER_PLUS = 3500,
		OCTOCOPTER_X = 4000,
		OCTOCOPTER_PLUS = 4500
	};

	enum ThrottleMode // unsigned char
	{
		STATIC = 10,
		DYNAMIC = 20,
		RATE = 30,
		ALTITUDE = 40
	};

	enum StickMovementMode
	{
		COPTER,
		GEOGRAPHIC,
		BASE_POINT
	};

	enum BatteryType
	{
		UNDEFINED = 0,
		BATTERY_2S = 2,
		BATTERY_3S,
		BATTERY_4S,
		BATTERY_5S,
		BATTERY_6S
	};

	enum EscPwmFreq
	{
		SLOW,
		MEDIUM,
		FAST,
		VERY_FAST,
		ONESHOT_125
	};

	enum FlagId
	{
		ENABLE_FLIGHT_LOGGER,
		ALLOW_DYNAMIC_AUTOPILOT,
		GPS_SENSORS_POSITION_DEFINED
	};

	// base control settings
	int uavType;
	int initialSolverMode;
	int manualThrottleMode;

	// auto landing settings
	float autoLandingDescedRate;
	float maxAutoLandingTime;

	// control values
	float maxRollPitchControlValue;
	float maxYawControlValue;

	// regulator control settings
	// stabilization
	Vect3Df pidRollRate, pidPitchRate, pidYawRate;
	// angle
	float rollProp, pitchProp, yawProp;

	// throttle controller settings
	float altPositionProp;
	float altVelocityProp;
	Vect3Df pidThrottleAccel;

	// throttle controller stick settings
	float throttleAltRateProp;

	// autopilot settings
	float maxAutoAngle;
	float maxAutoVelocity;
	float autoPositionProp;
	float autoVelocityProp;
	Vect3Df pidAutoAccel;

	// autopilot stick settings
	float stickPositionRateProp;
	int stickMovementMode;

	// battery type
	int batteryType;

	// error handling type
	int errorHandlingAction;

	// esc controller PWM frequency
	int escPwmFreq;

	// gps sensors position in rigid body coordinate system
	Vect3Df gpsSensorPosition;

	// flags for any boolean settings
	Flags<unsigned> flags;

	ControlSettings(void);
	ControlSettings(const unsigned char* src);

	void serialize(unsigned char* dst) const;

	unsigned getDataSize() const;

	SignalData::Command getSignalDataType(void) const;
	SignalData::Command getSignalDataCommand(void) const;

    MessageType getMessageType(void) const;

	bool isValid(void) const;

	void setCrc(void);

	float getBatteryErrorLevel(void) const;
	float getBatteryWarningLevel(void) const;
	float getBatteryMaxLevel(void) const;

	bool isBatteryError(const float voltage) const;
	bool isBatteryWarning(const float voltage) const;

	float getBatteryChargeLevel(const float voltage) const;

	ControlData::SolverMode getInitialSolverMode(void) const;

	ControlData formatEulers(const ControlData& controlData) const;

	static ControlSettings createDefault(void);

	bool isUavBisymetricalFromData(void) const;

	ControlData getControlDataForLogs(const ControlData& controlData) const;

private:
	unsigned crcValue;
};

#endif // __CONTROL_SETTINGS__
