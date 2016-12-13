// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __I_SIGNAL_PAYLOAD_MESSAGE__
#define __I_SIGNAL_PAYLOAD_MESSAGE__

#include "IMessage.hpp"

#include "SignalData.hpp"

// interface for data caontainers sent in signal data message payload
class ISignalPayloadMessage : public IMessage
{
public:
	// IMessage override
	PreambleType getPreambleType(void) const;

	virtual unsigned getDataSize(void) const = 0;

	virtual SignalData::Command getSignalDataType(void) const = 0;

	virtual SignalData::Command getSignalDataCommand(void) const = 0;

	virtual bool isValid(void) const = 0;
};

#endif // __I_SIGNAL_PAYLOAD_MESSAGE__