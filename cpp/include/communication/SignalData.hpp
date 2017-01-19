// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __SIGNAL_DATA__
#define __SIGNAL_DATA__

#include "IMessage.hpp"

class SignalData : public IMessage
{
public:
	enum Command
	{
		DUMMY,

		START_CMD = 100007,

		APP_LOOP,
		FLIGHT_LOOP,
		CALIBRATE_ACCEL,
		CALIBRATE_MAGNET,
		CALIBRATE_ESC,
		UPLOAD_SETTINGS,
		DOWNLOAD_SETTINGS,
		CALIBRATE_RADIO,
		CHECK_RADIO,
		SOFTWARE_UPGRADE,
		SYSTEM_RESET,
		UPLOAD_ROUTE,
		DOWNLOAD_ROUTE,
		SENSORS_LOGGER,

		CALIBRATION_SETTINGS,
		CONTROL_SETTINGS,
		ROUTE_CONTAINER,

		CALIBRATION_SETTINGS_DATA,
		CONTROL_SETTINGS_DATA,
		ROUTE_CONTAINER_DATA,

		PING_VALUE
	};

	enum CommandParameter
	{
		DUMMY_PARAMETER,

		START = 1000011,
		ACK,
		DATA_ACK,
		ENTER_DFU,

		BREAK,
		BREAK_ACK,
		BREAK_FAIL,

		DONE,
		READY,
		FAIL,
		SKIP,

		NON_STATIC,
		NOT_ALLOWED,

		BAD_CRC,
		TIMEOUT
	};

	SignalData(void);
	SignalData(const unsigned char* src);
	SignalData(const Command& command, const CommandParameter& parameter);
	SignalData(const Command& command, const int parameterValue);

	Command getCommand(void) const;
	CommandParameter getParameter(void) const;

	int getParameterValue() const;

	bool operator ==(const SignalData& right) const;

    unsigned getPayloadSize(void) const;

    PreambleType getPreambleType(void) const;

    void serialize(unsigned char* dst) const;

    MessageType getMessageType(void) const;

	static Command parseCommand(const unsigned char* src);
	static CommandParameter parseCommandParameter(const unsigned char* src);

	static unsigned short parseAllPacketsNumber(const unsigned char* src);
	static unsigned short parseActualPacketNumber(const unsigned char* src);

	static bool hasPayload(const Command command);

private:
	int command;
	int parameter;
};


#endif // __SIGNAL_DATA__
