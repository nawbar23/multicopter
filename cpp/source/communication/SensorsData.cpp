#include "SensorsData.hpp"

#include <string.h>

const double SensorsData::GEO_ZERO_POINT_LAT = 50.25;
const double SensorsData::GEO_ZERO_POINT_LON = 20.00;
const double SensorsData::POSITION_GRID_SIZE = 0.25;

SensorsData::SensorsData(void)
{
}

SensorsData::SensorsData(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

IMessage::PreambleType SensorsData::getPreambleType(void) const
{
	return IMessage::CONTROL;
}

void SensorsData::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

unsigned SensorsData::getDataSize(void) const
{
	return sizeof(SensorsData) - 4;
}

GpsData SensorsData::getGpsData(void) const
{
	GpsData gpsData;
	gpsData.lat = roboLib::shortToVal(
		GEO_ZERO_POINT_LAT - POSITION_GRID_SIZE, GEO_ZERO_POINT_LAT + POSITION_GRID_SIZE, lat);
	gpsData.lon = roboLib::shortToVal(
		GEO_ZERO_POINT_LON - POSITION_GRID_SIZE, GEO_ZERO_POINT_LON + POSITION_GRID_SIZE, lon);
	gpsData.speed = (float)roboLib::shortToVal(0.0, 50.0, speedGps);
	gpsData.course = (float)roboLib::shortToVal(0.0, 360.0, courseGps);
	gpsData.alt = (float)roboLib::shortToVal(0.0, 1000.0, altitudeGps);
	gpsData.fix = (lat != 0 && lon != 0);
	gpsData.HDOP = -1.0f;
	return gpsData;
}

ImuData SensorsData::getImuData(void) const
{
	ImuData imuData;
	imuData.gyroRaw = gyroRaw;
	imuData.accelRaw = accelRaw;
	imuData.magnetRaw = magnetRaw;
	imuData.pressRaw = pressRaw;
	return imuData;
}
