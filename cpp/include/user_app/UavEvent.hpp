#ifndef __UAV_EVENT__
#define __UAV_EVENT__

#include "IMessage.hpp"
#include "ISignalPayloadMessage.hpp"

#include "Exception.hpp"

#include <string>

/**
 * =============================================================================================
 * UavEvent
 * =============================================================================================
 */
class UavEvent
{
public:
    // events that can happen on UAV side
    enum Type
    {
        MESSAGE,
        DATA_RECEIVED,
        DATA_UPLOADED,
        CONNECTION_STATUS,
        CALIBRATION_NON_STATIC,
        APPLICATION_LOOP_STARTED,
        APPLICATION_LOOP_ENDED,
        APPLICATION_LOOP_TERMINATED,
        FLIGHT_LOOP_NOT_ALLOWED,
        VIA_ROUTE_NOT_ALLOWED,
        FLIGHT_LOOP_STARTED,
        FLIGHT_LOOP_ENDED,
        FLIGHT_LOOP_TERMINATED,
        CALIBRATE_MAGNET_STARTED,
        CALIBRATE_RADIO_STARTED,
        CALIBRATE_RADIO_NOT_ALLOWED,
        CALIBRATE_RADIO_ENDED,
        CALIBRATE_RADIO_ACK,
        CALIBRATE_RADIO_FAIL,
        CHECK_RADIO_STARTED,
        CHECK_RADIO_NOT_ALLOWED,
        CHECK_RADIO_ENDED,
        UPGRADE_STARTED,
        SENSORS_LOGGER_STARTED,
        SENSORS_LOGGER_ENDED,
        WIFI_CONFIG_ENDED,
        CONNECTION_LOST,
        CONNECTION_RECOVERED,
        CONTROLS_DOWNLOAD_FAIL,
        ROUTE_DOWNLOAD_FAIL,
        CALIBRATE_ESC_STARTED,
        CALIBRATE_ESC_NOT_ALLOWED,
        CALIBRATE_ESC_DONE,
        CALIBRATE_ESC_ENDED
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

    virtual std::string toString(void) const
    {
        switch (type)
        {
        case MESSAGE: return "MESSAGE";
        case DATA_RECEIVED: return "DATA_RECEIVED";
        case DATA_UPLOADED: return "DATA_UPLOADED";
        case CONNECTION_STATUS: return "CONNECTION_STATUS";
        case CALIBRATION_NON_STATIC: return "CALIBRATION_NON_STATIC";
        case APPLICATION_LOOP_STARTED: return "APPLICATION_LOOP_STARTED";
        case APPLICATION_LOOP_ENDED: return "APPLICATION_LOOP_ENDED";
        case APPLICATION_LOOP_TERMINATED: return "APPLICATION_LOOP_TERMINATED";
        case FLIGHT_LOOP_NOT_ALLOWED: return "FLIGHT_LOOP_NOT_ALLOWED";
        case VIA_ROUTE_NOT_ALLOWED: return "VIA_ROUTE_NOT_ALLOWED";
        case FLIGHT_LOOP_STARTED: return "FLIGHT_LOOP_STARTED";
        case FLIGHT_LOOP_ENDED: return "FLIGHT_LOOP_ENDED";
        case FLIGHT_LOOP_TERMINATED: return "FLIGHT_LOOP_TERMINATED";
        case CALIBRATE_MAGNET_STARTED: return "CALIBRATE_MAGNET_STARTED";
        case CALIBRATE_RADIO_STARTED: return "CALIBRATE_RADIO_STARTED";
        case CALIBRATE_RADIO_NOT_ALLOWED: return "CALIBRATE_RADIO_NOT_ALLOWED";
        case CALIBRATE_RADIO_ENDED: return "CALIBRATE_RADIO_ENDED";
        case CALIBRATE_RADIO_ACK: return "CALIBRATE_RADIO_ACK";
        case CALIBRATE_RADIO_FAIL: return "CALIBRATE_RADIO_FAIL";
        case CHECK_RADIO_STARTED: return "CHECK_RADIO_STARTED";
        case CHECK_RADIO_NOT_ALLOWED: return "CHECK_RADIO_NOT_ALLOWED";
        case CHECK_RADIO_ENDED: return "CHECK_RADIO_ENDED";
        case UPGRADE_STARTED: return "UPGRADE_STARTED";
        case SENSORS_LOGGER_STARTED: return "SENSORS_LOGGER_STARTED";
        case SENSORS_LOGGER_ENDED: return "SENSORS_LOGGER_ENDED";
        case WIFI_CONFIG_ENDED: return "WIFI_CONFIG_ENDED";
        case CONNECTION_LOST: return "CONNECTION_LOST";
        case CONNECTION_RECOVERED: return "CONNECTION_RECOVERED";
        case CONTROLS_DOWNLOAD_FAIL: return "CONTROLS_DOWNLOAD_FAIL";
        case ROUTE_DOWNLOAD_FAIL: return "ROUTE_DOWNLOAD_FAIL";
        case CALIBRATE_ESC_STARTED: return "CALIBRATE_ESC_STARTED";
        case CALIBRATE_ESC_NOT_ALLOWED: return "CALIBRATE_ESC_NOT_ALLOWED";
        case CALIBRATE_ESC_DONE: return "CALIBRATE_ESC_DONE";
        case CALIBRATE_ESC_ENDED: return "CALIBRATE_ESC_ENDED";
        default:
            __RL_EXCEPTION__("UavEvent::toString:Unexpected event type");
        }
    }
};

class UavEventMessage : public UavEvent
{
public:
    enum MessageType
    {
        INFO,
        WARNING,
        ERROR
    };

    const MessageType messageType;
    const std::string message;

    UavEventMessage(const MessageType& _messageType, const std::string& _message):
        UavEvent(MESSAGE),
        messageType(_messageType),
        message(_message)
    {
    }

    bool hasMessage(void) const
    {
        return true;
    }

    MessageType getMessageType(void) const
    {
        return messageType;
    }

    std::string getMessage(void) const
    {
        return message;
    }

    virtual std::string toString(void) const
    {
        return UavEvent::toString() + " with:\n\"" + message + "\"";
    }
};

class UavEventReceived : public UavEvent
{
private:
    const IMessage& received;

public:
    UavEventReceived(const IMessage& _received) :
        UavEvent(DATA_RECEIVED),
        received(_received)
    {
    }

    ~UavEventReceived(void)
    {
        delete &received;
    }

    const IMessage& getReceived(void) const
    {
        return received;
    }

    std::string toString(void) const
    {
        return UavEvent::toString() + " with: " + received.getMessageName();
    }
};

class UavEventUploaded : public UavEvent
{
private:
    const ISignalPayloadMessage& uploaded;

public:
    UavEventUploaded(const ISignalPayloadMessage& _uploaded) :
        UavEvent(DATA_UPLOADED),
        uploaded(_uploaded)
    {
    }

    ~UavEventUploaded(void)
    {
        delete &uploaded;
    }

    const ISignalPayloadMessage& getUploaded(void) const
    {
        return uploaded;
    }

    std::string toString(void) const
    {
        return UavEvent::toString() + " with: " + uploaded.getMessageName();
    }
};

class UavEventConnectionStatus : public UavEvent
{
private:
    const unsigned ping;
    const unsigned received;
    const unsigned fails;

public:
    UavEventConnectionStatus(const unsigned _ping,
                             const unsigned _received,
                             const unsigned _fails) :
        UavEvent(CONNECTION_STATUS),
        ping(_ping),
        received(_received),
        fails(_fails)
    {
    }

    unsigned getPing(void) const
    {
        return ping;
    }

    unsigned getReceived(void) const
    {
        return received;
    }

    unsigned getFails(void) const
    {
        return fails;
    }

    std::string toString(void) const
    {
        return UavEvent::toString() + ": " + std::to_string(ping) + " ms";
    }
};

#endif // __UAV_EVENT__
