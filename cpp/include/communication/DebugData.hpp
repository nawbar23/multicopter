// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __DEBUG_DATA__
#define __DEBUG_DATA__

#ifdef __MULTICOPTER_USE_STL__

#include <iostream>
#include <sstream>

#endif //__MULTICOPTER_USE_STL__

#include "MathCore.hpp"

#include "IMessage.hpp"

#include "ControlData.hpp"
#include "Flags.hpp"

/**
 * =============================================================================================
 * DebugData
 * =============================================================================================
 */
class DebugData : public IMessage
{
public:
	enum ControllerState // unsigned short
	{
		IDLE,
		MANUAL = ControlData::MANUAL,
		AUTOLANDING = ControlData::AUTOLANDING,
		AUTOLANDING_AP = ControlData::AUTOLANDING_AP,
		HOLD_ALTITUDE = ControlData::HOLD_ALTITUDE,
		HOLD_POSITION = ControlData::HOLD_POSITION,
		BACK_TO_BASE = ControlData::BACK_TO_BASE,
		VIA_ROUTE = ControlData::VIA_ROUTE,
		STOP = ControlData::STOP,
		APPLICATION_LOOP = 3000
	};

	enum GpsFix
	{
		NO_FIX,
		FIX,
		FIX_3D
	};

	enum FlagsId // unsigned char bit ID for flags
	{
		SOLVER_1,
		SOLVER_2,
		AUTOLANDING_ENABLED,
		AUTOPILOT_ENABLED,
		ERROR_HANDLING,
		LOW_BATTERY_VOLTAGE,
		GPS_FIX_3D,
		GPS_FIX
	};

	Vect3Df euler; // rotation [roll; pitch; yaw] [rad]
	Vect2Df position; // location [lat; lon;] [deg]
	float altitude; // altitude (at the moment relative)

	float velocity; // velocity relative to XY earth plane [m/s]

	unsigned short controllerState; // ControllerState

	// flags:
	// GPS fix | GPS 3D fix | low batt. V | errorHandling | autopilotUsed | autolanding | solver1 | solver2
	Flags<unsigned char> flagsObj;

	unsigned char battery; // percent of battery capacity

	DebugData(void);
	DebugData(const unsigned char* src);

	PreambleType getPreambleType(void) const;

	void serialize(unsigned char* dst) const;

    MessageType getMessageType(void) const;

	unsigned getDataSize(void) const;

	void setEuler(const Vect3Df& euler);
	void setPosition(const Vect2Df& position);
	void setAltitude(const float controllerCommand);
	void setVelocity(const float solverMode);
	void setControllerState(const ControllerState& controllerState);
	void setSolverMode(const ControlData::SolverMode& solverMode);
	void setBatteryVoltage(const float voltage);

	const Vect3Df& getEuler(void) const;
	const Vect2Df& getPosition(void) const;
	float getAltitude(void) const;
	float getVelocity(void) const;
	ControllerState getControllerState(void) const;
	ControlData::SolverMode getSolverMode(void) const;
	float getBatteryVoltage(void) const;

	const Flags<unsigned char>& flags(void) const;
	Flags<unsigned char>& flags(void);

	void setGpsFlags(const GpsFix& gpsFix);
	void setNoConnection();

	bool isGpsFixed(void) const;
	bool isGps3DFixed(void) const;
	bool isBatteryWaring(void) const;
	bool isAutopilotUsed(void) const;
	bool isAutolanding(void) const;
	bool isErrorHandling(void) const;
	bool isDrawable(void) const;
	bool isConnection(void) const;

	float getNormalYaw(void) const;

#ifdef __MULTICOPTER_USE_STL__

	static std::string getControllerStateString(const ControllerState& state);
	static DebugData parseFromString(const std::string& line);
	friend std::ostream& operator << (std::ostream& stream, const DebugData& debugData);

#endif //__MULTICOPTER_USE_STL__
};

#endif // __DEBUG_DATA__
