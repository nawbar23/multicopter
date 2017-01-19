#include "GpsData.hpp"

#include <string.h>

GpsData::GpsData(void)
{
}

GpsData::GpsData(const unsigned char* tab)
{
	memcpy(this, tab, sizeof(*this));
}

void GpsData::serialize(unsigned char* tab) const
{
	memcpy(tab, this, sizeof(*this));
}
