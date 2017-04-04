#include "FlightAction.hpp"

#include "AppAction.hpp"

#include "UserUavEvent.hpp"

#include "Exception.hpp"

#include <functional>

FlightAction::FlightAction(Listener* const _listener, const double _controlFreq):
    ICommAction(_listener),
    controlFreq(_controlFreq)
{
    state = IDLE;

    controlTimer = monitor->createTimer(std::bind(&FlightAction::controlTaskHandler, this));
}

FlightAction::~FlightAction(void)
{
    delete controlTimer;
}

void FlightAction::start(void)
{
    monitor->trace("Flight final start command");
    state = FLING;
    monitor->notifyUavEvent(new UavEvent(UavEvent::FLIGHT_LOOP_STARTED));
    listener->send(SignalData(SignalData::FLIGHT_LOOP, SignalData::READY));
    listener->enablePingTask(true);
    controlTimer->start(controlFreq);
}

bool FlightAction::isActionDone(void) const
{
    return IDLE == state;
}

ICommAction::Type FlightAction::getType(void) const
{
    return FLIGHT;
}

std::string FlightAction::getStateName(void) const
{
    switch (state)
    {
    case IDLE: return "IDLE";
    case FLING: return "FLING";
    case BREAKING: return "BREAKING";
    default:
        __RL_EXCEPTION__("FlightAction::getStateName::Unexpected state");
    }
}

void FlightAction::handleReception(const IMessage& message)
{
    switch (state)
    {
    case IDLE:
        handleIdleReception(message);
        break;

    case FLING:
    case BREAKING:
        handleRunningReception(message);
        break;

    default:
        except("Unexpected state");
    }
}

void FlightAction::handleUserEvent(const UserUavEvent& event)
{
    switch (state)
    {
    case FLING:
        if (UserUavEvent::BREAK_FLIGHT_LOOP == event.getType())
        {
            listener->enablePingTask(false);
            listener->enableConnectionTimeoutTask(false);
            state = BREAKING;
        }
        else
        {
            except("Unexpected user event received");
        }
        break;

    default:
        except("User event received in unexpected state");
    }
}

void FlightAction::handleRunningReception(const IMessage& message)
{
    if (IMessage::DEBUG_DATA == message.getMessageType()
            || IMessage::AUTOPILOT_DATA == message.getMessageType())
    {
        monitor->notifyUavEvent(new UavEventReceived(message));
    }
    else if (IMessage::SIGNAL_DATA == message.getMessageType())
    {
        const SignalData signal = reinterpret_cast<const SignalData&>(message);
        switch (signal.getCommand())
        {
        case SignalData::PING_VALUE:
            listener->onPongReception(signal);
            break;

        case SignalData::FLIGHT_LOOP:
            if (SignalData::BREAK_ACK == signal.getParameter() && BREAKING == state)
            {
                flightEnded(false);
            }
            else if (SignalData::BREAK == signal.getParameter())
            {
                flightEnded(true);
            }
            else
            {
                except("Unexpected message received");
            }
            break;

        default:
            except("Unexpected message received");
        }
    }
    else
    {
        except("Unexpected message received");
    }
}

void FlightAction::controlTaskHandler(void)
{
    ControlData controlDataToSend = monitor->getControlDataToSend();
    if (BREAKING == state)
    {
        monitor->trace("FlightAction::controlTaskHandler:setting break command");
        controlDataToSend.setControllerCommand(ControlData::STOP);
    }
    listener->send(controlDataToSend);
}

void FlightAction::flightEnded(const bool byBoard)
{
    if (byBoard)
    {
        monitor->notifyUavEvent(new UavEvent(UavEvent::FLIGHT_LOOP_TERMINATED));
    }
    else
    {
        monitor->notifyUavEvent(new UavEvent(UavEvent::FLIGHT_LOOP_ENDED));
    }
    listener->enablePingTask(false);
    controlTimer->stop();
    state = IDLE;
    listener->startAction(new AppAction(listener));
}
