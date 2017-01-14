// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __LOCATION__
#define __LOCATION__

#include "MathCore.hpp"

class Location
{
public:
	Vect2Dd p;
	float absoluteAltitude;
	float relativeAltitude;

	Location(void);
	Location(const Vect2Dd& _p, const float _absoluteAltitude, const float _relativeAltitude);

	bool isValidPosition(void) const;
	bool isValidAbsoluteAltitude(void) const;
	bool isValidRelativeAltitude(void) const;

	static Vect2Df getInvalidPosition(void);
	static float getInvalidAltitude(void);
	static Location getInvalidLocation(void);
};


#endif // __LOCATION__
