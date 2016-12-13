// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __ROUTE_CONTAINER__
#define __ROUTE_CONTAINER__

#include "IXmlParcable.hpp"
#include "ISignalPayloadMessage.hpp"
#include "SignalData.hpp"

#include "Waypoint.h"

class RouteContainer : public ISignalPayloadMessage, public IXmlParcable
{
public:
	RouteContainer(void);
	RouteContainer(const unsigned char* src);
	RouteContainer(const RouteContainer& routeContainer);

	virtual void serialize(unsigned char* dst) const;

	unsigned getDataSize() const;

	SignalData::Command getSignalDataType(void) const;
	SignalData::Command getSignalDataCommand(void) const;

	bool isValid(void) const;

	void setCrc(void);

	unsigned getRouteBinarySize(void) const;

    unsigned getBinarySize(void) const;

    const Waypoint* getRouteWaypoint(const unsigned waypointIndex) const;

	unsigned getRouteSize(void) const;

	float getWaypointTime(void) const;

	float getBaseTime(void) const;

	unsigned getWaypointStablilizationMaxCounter(const float dt) const;

	unsigned getBaseStablilizationMaxCounter(const float dt) const;

	bool isRouteEnded(const unsigned waypointIndex) const;

	bool isContainerFilled(void) const;

	RouteContainer& operator=(const RouteContainer& right);

	virtual ~RouteContainer(void);

	static unsigned getConstraintBinarySize(void);
	static unsigned getMaxRouteSize(void);
	static unsigned getMaxRouteContainerBinarySize(void);

private:
	struct Constraint
	{
		unsigned crcValue;

		unsigned routeSize;

		float waypointTime; // [s]
		float baseTime; // [s]
	};

	Constraint constraint;
	Waypoint* route;
};

#endif // __ROUTE_CONTAINER__
