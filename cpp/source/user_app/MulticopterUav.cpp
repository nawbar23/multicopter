#include "MulticopterUav.hpp"

#include "Exception.hpp"

#include "UserUavEvent.hpp"

#include "IdleAction.hpp"

#include <stdlib.h>
#include <functional>

MulticopterUav::MulticopterUav(IMulticopterMonitor* const _monitor,
                               const double _pingFreq,
                               const double _controlFreq,
                               const double _connectionTimeout) :
    monitor(_monitor),
    pingFreq(_pingFreq),
    controlFreq(_controlFreq),
    connectionTimeoutFreq(1 / _connectionTimeout)
{
    action = new IdleAction(this);
    garbageAction = nullptr;

    pingTimer = monitor->createTimer(std::bind(&MulticopterUav::pingTimerHandler, this));
    connetionTimer = monitor->createTimer(std::bind(&MulticopterUav::connectionTimerHandler, this));

    std::srand(std::time(0));
}

MulticopterUav::~MulticopterUav(void)
{
    delete action;
    delete garbageAction;
    delete pingTimer;
    delete connetionTimer;
}

void MulticopterUav::notifyUserUavEvent(const UserUavEvent* const userUavEvent)
{
    std::unique_ptr<const UserUavEvent> guard(userUavEvent);
    if (nullptr != action)
    {
        monitor->trace("Handling user event: " + userUavEvent->toString() + " at: " + action->getName());
        action->handleUserEvent(*userUavEvent);
    }
    else
    {
        __RL_EXCEPTION__("MulticopterUav::notifyUserUavEvent:action null, unknown error");
    }
}

ICommAction::Type MulticopterUav::getState(void) const
{
    if (nullptr == action)
    {
        return ICommAction::IDLE_ACTION;
    }
    else
    {
        return action->getType();
    }
}

void MulticopterUav::notifyReception(const IMessage* const message)
{
    receptionFeed = true;
    if (connectionLost)
    {
        connectionLost = false;
        monitor->notifyUavEvent(new UavEvent(UavEvent::CONNECTION_RECOVERED));
    }

    // TODO memmory leak with SIGNAL messages

    try
    {
//        monitor->trace("HandleReception with " + action->getName() +
//                       " msg: " + message->getMessageName());
        action->baseHandleReception(*message);
    }
    catch (Exception e)
    {
        handleError(e.message());
    }
}

void MulticopterUav::handleError(const std::string& message)
{
    monitor->trace("MulticopterUav::handleError: " + message);
    enablePingTask(false);
    enableConnectionTimeoutTask(false);
    interface->disconnect();
    delete action;
    action = new IdleAction(this);
    monitor->notifyUavEvent(new UavEventMessage(UavEventMessage::ERROR, message));
}

void MulticopterUav::pingTimerHandler(void)
{
    sentPingValue = std::rand();
    sentPingTime = clock();
    send(SignalData(SignalData::PING_VALUE, sentPingValue));
}

void MulticopterUav::handlePong(const SignalData& signalData) const
{
    if (signalData.getParameterValue() == sentPingValue)
    {
        unsigned ping = (unsigned)(((clock() - sentPingTime) / 2.0f) + 0.5f);
        monitor->notifyUavEvent(new UavEventConnectionStatus(
                                    ping,
                                    dispatcher.getSucessfullReceptions(),
                                    dispatcher.getFailedReceptions()));
    }
}

void MulticopterUav::connectionTimerHandler(void)
{
    if (false == receptionFeed)
    {
        // connection timeout ocured
        if (false == connectionLost)
        {
            // first time
            connectionLost = true;
            monitor->notifyUavEvent(new UavEvent(UavEvent::CONNECTION_LOST));
        }
    }
    receptionFeed = false;
}

void MulticopterUav::onConnected()
{
    monitor->trace("MulticopterUav::onConnected");
    action->start();
}

void MulticopterUav::onDisconnected()
{
    monitor->trace("MulticopterUav::onDisconnected");
}

void MulticopterUav::onError(const std::string& message)
{
    handleError(message);
}

void MulticopterUav::onDataReceived(const unsigned char* data, const unsigned dataSize)
{
    IMessage::PreambleType receivedPreamble;
    for (unsigned i = 0; i < dataSize; i++)
    {
        receivedPreamble = dispatcher.putChar(data[i]);
        if (IMessage::EMPTY != receivedPreamble)
        {
            notifyReception(dispatcher.retriveMessage(
                                receivedPreamble, action->getExpectedControlMessageType()));
        }
    }
    // this is absolutely safe place to delete previous action
    if (nullptr != garbageAction)
    {
        delete garbageAction;
        garbageAction = nullptr;
    }
}

IMulticopterMonitor* MulticopterUav::getMonitor(void)
{
    return monitor;
}

void MulticopterUav::startAction(ICommAction* newAction, bool immediateStart)
{
    monitor->trace("Starting new action: " + newAction->getName());

    if (false == action->isActionDone())
    {
        std::string message = "Previous action (" + action->getName() +
                ") not done, when performing: " + ICommAction::toString(newAction->getType()) + ".";
        __RL_EXCEPTION__(message.c_str());
    }

    action->end();

    std::swap(action, newAction);
    garbageAction = newAction;

    // here is some possibility that message will be received before action start
    // because of this issue, action has to be ready for reception form construction moment

    if (immediateStart)
    {
        action->start();
    }
}

void MulticopterUav::onPongReception(const SignalData& pong)
{
    handlePong(pong);
}

void MulticopterUav::send(const IMessage& message)
{
    message.serializeMessage(messageBuildingBuffer);
    interface->sendData(messageBuildingBuffer, message.getMessageSize());
}

void MulticopterUav::send(const ISignalPayloadMessage& message)
{
    ISignalPayloadMessage::MessagesBuilder builder(&message);
    while (builder.hasNext())
    {
        builder.getNext(messageBuildingBuffer);
        interface->sendData(messageBuildingBuffer, IMessage::SIGNAL_DATA_MESSAGE_SIZE);
    }
}

void MulticopterUav::enablePingTask(bool enable)
{
    if (enable)
    {
        monitor->trace("Starting ping task");
        pingTimer->start(pingFreq);
    }
    else
    {
        monitor->trace("Ending ping task");
        pingTimer->stop();
    }
}

void MulticopterUav::enableConnectionTimeoutTask(bool enable)
{
    if (enable)
    {
        monitor->trace("Starting connection timeout task");
        connetionTimer->start(connectionTimeoutFreq);
    }
    else
    {
        monitor->trace("Ending connection timeout task");
        connetionTimer->stop();
    }
}

void MulticopterUav::connectInterface(IAppCommInterface* _interface)
{
    interface = _interface;
    interface->setListener(this);
    interface->connect();
}

void MulticopterUav::disconnectInterface(void)
{
    enablePingTask(false);
    enableConnectionTimeoutTask(false);
    interface->disconnect();
}
