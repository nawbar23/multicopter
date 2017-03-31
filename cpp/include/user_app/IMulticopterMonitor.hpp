// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef IMULTICOPTERMONITOR_HPP
#define IMULTICOPTERMONITOR_HPP

#include "IAppTimer.hpp"

#include "IMessage.hpp"

#include "UavEvent.hpp"
#include "ControlData.hpp"

class IMulticopterMonitor
{
public:  
    virtual ~IMulticopterMonitor();

    virtual void notifyUavEvent(const UavEvent* const event) = 0;

    virtual ControlData getControlDataToSend(void) = 0;

    virtual IAppTimer* createTimer(std::function<void (void)> exec) = 0;

    virtual void trace(const std::string& trace) = 0;
};

#endif // IMULTICOPTERMONITOR_HPP
