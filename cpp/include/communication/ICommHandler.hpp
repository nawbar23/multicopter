// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __I_COMM_HANDLER__
#define __I_COMM_HANDLER__

#include "ICommInterface.hpp"
#include "CommDispatcher.hpp"

#include "CalibrationSettings.hpp"
#include "ControlSettings.hpp"
#include "RouteContainer.hpp"
#include "SignalData.hpp"
#include "AutopilotData.hpp"
#include "DebugData.hpp"
#include "ControlData.hpp"
#include "SensorsData.hpp"

// main interface to support communications tasks for any peer in Multicopter system
// all algorithms according to communication protocol and flow are implemented here
// cause of using this interface is to allow multi platform usage
// time issues have to be implemented under this interface
class ICommHandler
{
public:
	static const unsigned DEFAULT_TIMEOUT = 1000;
	static const unsigned MAX_RETRANSMISSION = 3;

	virtual ~ICommHandler(void);

	// initializer - sets communication interface as operative and resets all helper variables
	void initialize(ICommInterface* _commHandle);

	const ICommInterface* getCommHandle(void) const;

	// receiving handler, iteratate through data from commHandle,
	// if data is received returns its preamble type and stops processing received data
	// else return EMPTY type
	IMessage::PreambleType proceedReceiving(void);

	// sending methodes
	bool send(const unsigned char* data, const unsigned dataSize);
	bool send(const IMessage& message);
	bool send(const ISignalPayloadMessage& message);
	bool send(const SignalData& message);
	// signal data messages
	bool send(const CalibrationSettings& calibrationSettings);
	bool send(const ControlSettings& controlSettings);
	bool send(const RouteContainer& routeContainer);

	// methods for receiving specyfic data from processor buffer
	// when these methods will be called without check for data reception result can be invalid
	SignalData getSignalData(void) const;
	SignalData::Command getCommand(void) const;
	SignalData::CommandParameter getParameter(void) const;

	void getSignalDataObject(ISignalPayloadMessage& data);

	CalibrationSettings getCalibrationSettings(void);
	ControlSettings getControlSettings(void);
	RouteContainer getRouteContainer(void);

	DebugData getDebugData(void) const;
	ControlData getControlData(void) const;
	SensorsData getSensorsData(void) const;
	AutopilotData getAutopilotData(void) const;

	unsigned getSentMessages(void) const;
	unsigned getReceivedMessage(void) const;
	unsigned getReceptionFailes(void) const;

	void clearCounters(void);

	// commands interface
    bool sendCommand(const SignalData& command);
    bool waitForCommand(const SignalData& command, const unsigned timeout = DEFAULT_TIMEOUT);
    bool waitForAnyCommand(SignalData& command, const unsigned timeout = DEFAULT_TIMEOUT);
    
    bool sendCommandGetAnyResponse(const SignalData& command, SignalData& response);
    bool sendCommandGetResponse(const SignalData& command, const SignalData& response);
    
    bool readCommandAndRespond(const SignalData& command, const SignalData& response);

    // signal payload data interfaces
	bool sendDataProcedure(const ISignalPayloadMessage& data);
	bool receiveDataProcedure(ISignalPayloadMessage& data);

protected:
    virtual void resetTimer(void) = 0;
    virtual unsigned getTimerValue(void) = 0;
    virtual void sleep(const unsigned miliseconds) = 0;

private:
	bool sendSignalData(const unsigned char* data, const unsigned dataSize, const SignalData::Command type);

	ICommInterface* commInterface;
	CommDispatcher commDispatcher;

	unsigned sentMessages;
};

#endif // __I_COMM_HANDLER__
