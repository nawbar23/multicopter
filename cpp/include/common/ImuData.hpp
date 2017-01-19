// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __IMU_DATA__
#define __IMU_DATA__

#include "MathCore.hpp"

class ImuData
{
public:
	Vect3Ds gyroRaw;
	Vect3Ds accelRaw;
	Vect3Ds magnetRaw;
	float pressRaw;

	ImuData(void);
	ImuData(const unsigned char* tab);

	void serialize(unsigned char* tab) const;
};

#endif // __IMU_DATA__
