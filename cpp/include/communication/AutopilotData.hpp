// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __AUTOPILOT_DATA__
#define __AUTOPILOT_DATA__

#include "MathCore.hpp"

#include "IMessage.hpp"

#include "Flags.hpp"

class AutopilotData : public IMessage
{
public:
	AutopilotData(void);
	AutopilotData(const unsigned char* src);

	PreambleType getPreambleType(void) const;

	void serialize(unsigned char* dst) const;

    MessageType getMessageType(void) const;

	unsigned getDataSize() const;

	const Vect2Dd& getTargetPosition(void) const;
	float getTargetAltitude(void) const;

	void setTargetPosition(const Vect2Dd& _targetPosition);
	void setTargetAltitude(const float _targetAltitude);

	const Flags<int>& flags(void) const;
	Flags<int>& flags(void);

private:
	// target
	Vect2Dd targetPosition;
	float targetAltitude;

	// flags
	Flags<int> flagsObj;
};

#endif // __AUTOPILOT_DATA__
