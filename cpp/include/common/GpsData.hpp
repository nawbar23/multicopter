// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __GPS_DATA__
#define __GPS_DATA__

#include "MathCore.hpp"

class GpsData
{
public:
	double lat, lon; // polozenie
	float alt; // wysokosc
	float speed, course; // predkosc
	float HDOP; // dokladnosc
	bool fix;

	GpsData(void);
	GpsData(const unsigned char* tab);

	void serialize(unsigned char* tab) const;
};

#endif // __GPS_DATA__
