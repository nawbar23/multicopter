#include "SignalData.hpp"

#include "CalibrationSettings.hpp"
#include "ControlSettings.hpp"
#include "RouteContainer.hpp"

#include <string.h>

SignalData::SignalData(void)
{
}

SignalData::SignalData(const unsigned char* src)
{
	command = parseCommand(src);
	parameter = parseCommandParameter(src + IMessage::SIGNAL_COMMAND_SIZE);
}

SignalData::SignalData(const Command& _command, const CommandParameter& _parameter) :
command(_command), parameter(_parameter)
{
}

SignalData::SignalData(const Command& _command, const int _parameterValue) :
	command(_command), parameter(_parameterValue)
{
}

SignalData::Command SignalData::getCommand(void) const
{
	return (Command)command;
}

SignalData::CommandParameter SignalData::getParameter(void) const
{
	return (CommandParameter)parameter;
}

int SignalData::getParameterValue() const
{
	return parameter;
}

bool SignalData::operator ==(const SignalData& right) const
{
	return getCommand() == right.getCommand()
		&& getParameter() == right.getParameter();
}

unsigned SignalData::getPayloadSize(void) const
{
	return IMessage::getPayloadSizeByType(getPreambleType());
}

IMessage::PreambleType SignalData::getPreambleType(void) const
{
	return IMessage::SIGNAL;
}

void SignalData::serialize(unsigned char* dst) const
{
	memcpy(dst, &command, IMessage::SIGNAL_COMMAND_SIZE);
	memcpy(dst + IMessage::SIGNAL_COMMAND_SIZE, &parameter, IMessage::SIGNAL_COMMAND_SIZE);
}

SignalData::Command SignalData::parseCommand(const unsigned char* src)
{
	int commandValue;
	memcpy(&commandValue, src, IMessage::SIGNAL_COMMAND_SIZE);
	return (SignalData::Command)commandValue;
}

SignalData::CommandParameter SignalData::parseCommandParameter(const unsigned char* src)
{
 	int commandParameterValue;
	memcpy(&commandParameterValue, src, IMessage::SIGNAL_COMMAND_SIZE);
	return (SignalData::CommandParameter)commandParameterValue;
}

unsigned short SignalData::parseAllPacketsNumber(const unsigned char* src)
{
	unsigned short result = 0;
	memcpy(&result, src + IMessage::SIGNAL_COMMAND_SIZE, 2);
	return result;
}

unsigned short SignalData::parseActualPacketNumber(const unsigned char* src)
{
	unsigned short result = 0;
	memcpy(&result, src + IMessage::SIGNAL_COMMAND_SIZE + 2, 2);
	return result;
}

bool SignalData::hasPayload(const SignalData::Command command)
{
	switch (command)
	{
	case CALIBRATION_SETTINGS_DATA:
	case CONTROL_SETTINGS_DATA:
	case ROUTE_CONTAINER_DATA:
		return true;

	default:
		return false;
	}
}

#endif // __MULTICOPTER_USER_APP__
