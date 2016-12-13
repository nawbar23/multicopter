// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __CONTROL_DATA__
#define __CONTROL_DATA__

#include "roboLibCore.hpp"

#include "IMessage.hpp"

class ControlData : public IMessage
{
public:
	enum ControllerCommand // unsigned short
	{
		MANUAL = 1000,
		AUTOLANDING = 1100,
		AUTOLANDING_AP = 1200,
		HOLD_ALTITUDE = 1300,
		HOLD_POSITION = 1400,
		BACK_TO_BASE = 1500,
		VIA_ROUTE = 1600,
		STOP = 2000,
		// error conditions
		ERROR_CONNECTION = 6100,
		ERROR_JOYSTICK = 6200,
		ERROR_EXTERNAL = 6300,
	};

	enum SolverMode // unsigned char
	{
		STABLILIZATION = 0,
		ANGLE_NO_YAW = 1,
		ANGLE = 2,
		HEADLESS = 3,
	};

	ControlData(void);
	ControlData(const unsigned char* src);

	PreambleType getPreambleType(void) const;

	void serialize(unsigned char* data) const;

	unsigned getDataSize(void) const;

	void setEuler(const Vect3Df& euler);
	void setThrottle(const float throttle);
	void setControllerCommand(const ControllerCommand& controllerCommand);
	void setSolverMode(const SolverMode& solverMode);

	const Vect3Df& getEuler(void) const;
	float getThrottle(void) const;
	ControllerCommand getControllerCommand(void) const;
	SolverMode getSolverMode(void) const;

	void setRoll(const float roll);
	void setPitch(const float pitch);
	void setYaw(const float yaw);

private:
	Vect3Df euler; // rotation control
	float throttle; // throttle
	unsigned short controllerCommand; // ControllerCommand
	unsigned char solverMode; // SolverMode
};

#endif // __CONTROL_DATA__
