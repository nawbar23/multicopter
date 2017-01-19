#ifndef __I_APP_COMM_HANDLER__
#define __I_APP_COMM_HANDLER__

#include "ICommHandler.hpp"

#include <string>
#include <ctime>

class IAppCommHandler : public ICommHandler
{
public:
	void sendCommandEx(const SignalData& command);
	void waitForCommandEx(const SignalData& command, const unsigned timeout = DEFAULT_TIMEOUT);
	SignalData waitForAnyCommandEx(const unsigned timeout = DEFAULT_TIMEOUT);

	SignalData sendCommandGetAnyResponseEx(const SignalData& command);
	void sendCommandGetResponseEx(const SignalData& command, const SignalData& response);

	void readCommandAndRespondEx(const SignalData& command, const SignalData& response);

	// signal payload data interfaces
	void sendDataProcedureEx(const CalibrationSettings& calibrationSettings);
	void sendDataProcedureEx(const ControlSettings& controlSettings);
	void sendDataProcedureEx(const RouteContainer& routeContainer);

	CalibrationSettings receiveCalibarionSettingsEx();
	ControlSettings receiveControlSettingsEx();
	RouteContainer receiveRouteContainerEx();
};

#endif // __I_APP_COMM_HANDLER__
