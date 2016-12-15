#include "RouteContainer.hpp"

#include <string.h>

#include "IMessage.hpp"
#include "SignalData.hpp"

RouteContainer::RouteContainer(void)
{
	route = NULL;
	constraint.routeSize = 0;
}

RouteContainer::RouteContainer(const unsigned char* src)
{
	// cast constraint of container
	memcpy(&constraint, src, getConstraintBinarySize());

	// alocate memmory
	if (constraint.routeSize <= getMaxRouteSize())
	{
		route = new Waypoint[constraint.routeSize];
		// cast dynamic route vector
		for (unsigned i = 0; i < constraint.routeSize; i++)
		{
			memcpy(route + i, src + getConstraintBinarySize() + i * sizeof(Waypoint), sizeof(Waypoint));
		}
	}
	else
	{
		route = NULL;
		constraint.routeSize = 0;
	}
}

RouteContainer::RouteContainer(const RouteContainer& routeContainer)
{
	route = NULL;
	constraint.routeSize = 0;
	*this = routeContainer;
}

void RouteContainer::serialize(unsigned char* dst) const
{
	// cast constraint of container
	memcpy(dst, &constraint, sizeof(Constraint));
	// cast dynamic route vector
	for (unsigned i = 0; i < constraint.routeSize; i++)
	{
		memcpy(dst + getConstraintBinarySize() + i * sizeof(Waypoint), route + i, sizeof(Waypoint));
	}
}

unsigned RouteContainer::getDataSize() const
{
	return getBinarySize();
}

SignalData::Command RouteContainer::getSignalDataType(void) const
{
	return SignalData::ROUTE_CONTAINER_DATA;
}

SignalData::Command RouteContainer::getSignalDataCommand(void) const
{
	return SignalData::ROUTE_CONTAINER;
}

bool RouteContainer::isValid(void) const
{
	unsigned char *dataTab = new unsigned char[getBinarySize()];
	serialize(dataTab);
	bool result = (IMessage::computeCrc32(dataTab + 4, getBinarySize() - 4) == constraint.crcValue);
	delete[] dataTab;
	return result;
}

void RouteContainer::setCrc(void)
{
	unsigned char *dataTab = new unsigned char[getBinarySize()];
	serialize(dataTab);
	constraint.crcValue = IMessage::computeCrc32(dataTab + 4, getBinarySize() - 4);
	delete[] dataTab;
}

unsigned RouteContainer::getRouteBinarySize(void) const
{
	return sizeof(Waypoint)* constraint.routeSize;
}

unsigned RouteContainer::getBinarySize(void) const
{
	return getConstraintBinarySize() + getRouteBinarySize();
}

const Waypoint* RouteContainer::getRouteWaypoint(const unsigned waypointIndex) const
{
	if (waypointIndex < constraint.routeSize)
	{
		// if index is in boundry, return pointed Location
		return route + waypointIndex;
	}
	else
	{
		// if invalid index was sepcified, return last Location in route
		return route + (constraint.routeSize - 1);
	}
}

unsigned RouteContainer::getRouteSize(void) const
{
	return constraint.routeSize;
}

float RouteContainer::getWaypointTime(void) const
{
	return constraint.waypointTime;
}

float RouteContainer::getBaseTime(void) const
{
	return constraint.baseTime;
}

unsigned RouteContainer::getWaypointStablilizationMaxCounter(const float dt) const
{
	return (unsigned)(constraint.waypointTime / dt);
}

unsigned RouteContainer::getBaseStablilizationMaxCounter(const float dt) const
{
	return (unsigned)(constraint.baseTime / dt);
}

bool RouteContainer::isRouteEnded(const unsigned waypointIndex) const
{
	return (waypointIndex + 1) >= constraint.routeSize;
}

bool RouteContainer::isContainerFilled(void) const
{
	return constraint.routeSize != 0 && constraint.routeSize < getMaxRouteSize();
}

RouteContainer& RouteContainer::operator=(const RouteContainer& right)
{
	if (route != NULL)
	{
		delete[] route;
	}

	constraint.routeSize = right.constraint.routeSize;
	route = new Waypoint[constraint.routeSize];
	for (unsigned i = 0; i < constraint.routeSize; i++)
	{
		route[i] = right.route[i];
	}

	constraint.waypointTime = right.constraint.waypointTime;
	constraint.baseTime = right.constraint.baseTime;

	setCrc();

	return *this;
}

RouteContainer::~RouteContainer(void)
{
	if (route != NULL)
	{
		delete[] route;
	}
}

unsigned RouteContainer::getConstraintBinarySize(void)
{
	return sizeof(Constraint);
}

unsigned RouteContainer::getMaxRouteSize(void)
{
	return 16;
}

unsigned RouteContainer::getMaxRouteContainerBinarySize(void)
{
	return getConstraintBinarySize() + getMaxRouteSize() * sizeof(Waypoint);
}

#endif // __MULTICOPTER_USER_APP__