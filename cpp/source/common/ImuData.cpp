#include "ImuData.hpp"

#include <string.h>

ImuData::ImuData(void) 
{ 
}

ImuData::ImuData(const unsigned char* tab)
{
	memcpy(this, tab, sizeof(*this));
}

void ImuData::serialize(unsigned char* tab) const
{
	memcpy(tab, this, sizeof(*this));
}
