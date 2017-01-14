#include "IAppCommHandler.hpp"

#include "UavManager.hpp"
#include "Exception.hpp"

void IAppCommHandler::sendCommandEx(const SignalData& command)
{
	if (!sendCommand(command))
	{
		std::string message = "Sending command: ("
			+ UavManager::toString(command)
			+ ") failed!";
		__RL_EXCEPTION__(message.c_str());
	}
}

void IAppCommHandler::waitForCommandEx(const SignalData& command, const unsigned timeout)
{
	if (!waitForCommand(command, timeout))
	{
		std::string message = "Waiting for command: ("
			+ UavManager::toString(command)
			+ ") failed!";
		__RL_EXCEPTION__(message.c_str());
	}
}

SignalData IAppCommHandler::waitForAnyCommandEx(const unsigned timeout)
{
	SignalData result;
	if (!waitForAnyCommand(result, timeout))
	{
		__RL_EXCEPTION__("Waiting for any command failed!");
	}
	return result;
}

SignalData IAppCommHandler::sendCommandGetAnyResponseEx(const SignalData& command)
{
	SignalData result;
	if (!sendCommandGetAnyResponse(command, result))
	{
		std::string message = "Send command: ("
			+ UavManager::toString(command)
			+ ") with any response failed!";
		__RL_EXCEPTION__(message.c_str());
	}
	return result;
}

void IAppCommHandler::sendCommandGetResponseEx(const SignalData& command, const SignalData& response)
{
	if (!sendCommandGetResponse(command, response))
	{
		std::string message = "Send command: ("
			+ UavManager::toString(command)
			+ ") with response: ("
			+ UavManager::toString(response)
			+ ") failed!";
		__RL_EXCEPTION__(message.c_str());
	}
}

void IAppCommHandler::readCommandAndRespondEx(const SignalData& command, const SignalData& response)
{
	if (!sendCommandGetResponse(command, response))
	{
		std::string message = "Read command: ("
			+ UavManager::toString(command)
			+ ") and respond: ("
			+ UavManager::toString(response)
			+ ") failed!";
		__RL_EXCEPTION__(message.c_str());
	}
}

void IAppCommHandler::sendDataProcedureEx(const CalibrationSettings& calibrationSettings)
{
	if (!sendDataProcedure(calibrationSettings))
	{
		__RL_EXCEPTION__("Sending calibration settings failed!");
	}
}

void IAppCommHandler::sendDataProcedureEx(const ControlSettings& controlSettings)
{
	if (!sendDataProcedure(controlSettings))
	{
		__RL_EXCEPTION__("Sending control settings failed!");
	}
}

void IAppCommHandler::sendDataProcedureEx(const RouteContainer& routeContainer)
{
	if (!sendDataProcedure(routeContainer))
	{
		__RL_EXCEPTION__("Sending route container failed!");
	}
}

CalibrationSettings IAppCommHandler::receiveCalibarionSettingsEx()
{
	CalibrationSettings result;
	if (!receiveDataProcedure(result))
	{
		__RL_EXCEPTION__("Receiving calibration settings failed!");
	}
	return result;
}

ControlSettings IAppCommHandler::receiveControlSettingsEx()
{
	ControlSettings result;
	if (!receiveDataProcedure(result))
	{
		__RL_EXCEPTION__("Receiving control settings failed!");
	}
	return result;
}

RouteContainer IAppCommHandler::receiveRouteContainerEx()
{
	RouteContainer result;
	if (!receiveDataProcedure(result))
	{
		__RL_EXCEPTION__("Receiving route container failed!");
	}
	return result;
}
