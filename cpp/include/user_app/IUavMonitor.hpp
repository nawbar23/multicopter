#ifndef __I_UAV_MONITOR__
#define __I_UAV_MONITOR__

#include "IMessage.hpp"

#include "UavEvent.hpp"

class IUavMonitor
{
public:
    virtual ~IUavMonitor()
    {
    }

    // these methods will be called by UavManager from connection thread
    // they should handle moving event to propper handler thread
    // connection thread can not be blocked due to communication handling
    virtual void notifyUavEvent(const UavEvent* const) = 0;
    virtual void notifyDataReceived(const IMessage* const) = 0;
    virtual void notifyPingUpdated(const float miliseconds) = 0;
};

#endif // __I_UAV_MONITOR__
