#ifndef __UAV_EVENT__
#define __UAV_EVENT__

#include <string>

class UavEvent
{
public:
    // events that can happen on UAV side
    enum Type
    {
        ERROR_MESSAGE,
        INFO_MESSAGE,
        CALIBRATION_NON_STATIC,
        APPLICATION_LOOP_STARTED,
        APPLICATION_LOOP_ENDED,
        FLIGHT_LOOP_STARTED,
        FLIGHT_LOOP_ENDED,
        CONTROL_SETTINGS_UPLOADED,
        CALIBRATE_ESC,
        CALIBRATE_RADIO,
        CHECK_RADIO_STARTED,
        CHECK_RADIO_ENDED,
        UPGRADE_STARTED,
        ROUTE_UPLOADED,
        SENSORS_LOGGER_STARTED,
        SENSORS_LOGGER_ENDED,
    };

    const Type type;

    UavEvent(const Type& _type):
        type(_type)
    {
    }

    virtual ~UavEvent(void)
    {
    }

    Type getType(void) const
    {
        return type;
    }

    virtual std::string getMessage(void) const
    {
        return "Error, no message specified, should never happened!";
    }

    virtual std::string toString(void) const
    {
        switch (type)
        {
        case ERROR_MESSAGE: return "ERROR_MESSAGE";
        case INFO_MESSAGE: return "INFO_MESSAGE";
        case CALIBRATION_NON_STATIC: return "CALIBRATION_NON_STATIC";
        case APPLICATION_LOOP_STARTED: return "APPLICATION_LOOP_STARTED";
        case APPLICATION_LOOP_ENDED: return "APPLICATION_LOOP_ENDED";
        case FLIGHT_LOOP_STARTED: return "FLIGHT_LOOP_STARTED";
        case FLIGHT_LOOP_ENDED: return "FLIGHT_LOOP_ENDED";
        case CONTROL_SETTINGS_UPLOADED: return "CONTROL_SETTINGS_UPLOADED";
        case CALIBRATE_ESC: return "CALIBRATE_ESC";
        case CALIBRATE_RADIO: return "CALIBRATE_RADIO";
        case CHECK_RADIO_STARTED: return "CHECK_RADIO_STARTED";
        case CHECK_RADIO_ENDED: return "CHECK_RADIO_ENDED";
        case UPGRADE_STARTED: return "UPGRADE_STARTED";
        case ROUTE_UPLOADED: return "ROUTE_UPLOADED";
        case SENSORS_LOGGER_STARTED: return "SENSORS_LOGGER_STARTED";
        case SENSORS_LOGGER_ENDED: return "SENSORS_LOGGER_ENDED";
        default: return "ERROR_EVENT_TYPE";
        }
    }
};

class UavEventMessage : public UavEvent
{
public:
    const std::string message;

    UavEventMessage(const Type& _type, const std::string& _message):
        UavEvent(_type), message(_message)
    {
    }

    std::string getMessage(void) const override
    {
        return message;
    }

    virtual std::string toString(void) const
    {
        return UavEvent::toString() + " with:\n\"" + message + "\"";
    }
};

#endif // __UAV_EVENT__
