#include "IMulticopterMonitor.hpp"

IMulticopterMonitor::~IMulticopterMonitor()
{
}

void IMulticopterMonitor::notifyUavEvent(const UavEvent* const event)
{
    notifyUavEvent(std::unique_ptr<const UavEvent>(event));
}
