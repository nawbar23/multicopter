// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __SENSORS_DATA__
#define __SENSORS_DATA__

#include "MathCore.hpp"

#include "IMessage.hpp"

#include "ImuData.hpp"
#include "GpsData.hpp"

/**
 * =============================================================================================
 * SensorsData
 * =============================================================================================
 */
class SensorsData : public IMessage
{
public:
	static const double GEO_ZERO_POINT_LAT;
	static const double GEO_ZERO_POINT_LON;
	static const double POSITION_GRID_SIZE;

	float pressRaw;

	Vect3Ds gyroRaw;
	Vect3Ds accelRaw;
	Vect3Ds magnetRaw;

	unsigned short lat, lon;
	unsigned short speedGps;
	unsigned short courseGps;
	unsigned short altitudeGps;

	SensorsData(void);
	SensorsData(const unsigned char* tab);

	PreambleType getPreambleType(void) const;

	void serialize(unsigned char* tab) const;

    MessageType getMessageType(void) const;

	unsigned getDataSize(void) const;

	GpsData getGpsData(void) const;
	ImuData getImuData(void) const;
};

#endif // __SENSORS_DATA_PACKET__
