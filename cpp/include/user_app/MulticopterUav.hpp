// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef MULTICOPTERUAV_HPP
#define MULTICOPTERUAV_HPP

#include "UavEvent.hpp"
#include "UserUavEvent.hpp"

#include "IAppCommInterface.hpp"
#include "IMulticopterMonitor.hpp"

#include "ICommAction.hpp"

#include "CommDispatcher.hpp"

#include <ctime>
#include <memory>
#include <atomic>

class MulticopterUav :
        public IAppCommInterface::Listener,
        public ICommAction::Listener
{
public:
    // any UAV action type has its equivalent in signal data command
    typedef ICommAction::Type UavAction;
    typedef UserUavEvent::Type ExternalEvent;

    MulticopterUav(IMulticopterMonitor* const _monitor,
                  const double _pingFreq,
                  const double _controlFreq,
                  const double _connectionTimeout);

    ~MulticopterUav(void);

    /**
     * Sends event to ongoing UAV action. This is the main
     * communication pipeline, every command allowed and needed
     * will be handled by calling this method. If unexpected event
     * will be pushed, communication will be broken and UavEvent::ERROR_MESSAGE
     * will be emmited by IMulticopterUav interface.
     * Event should be dynamicaly allocated and will be deleted
     * after processing.
     * Calling this method is thread safe.
     */
    void pushUserUavEvent(const UserUavEvent* const userUavEvent);

    /**
     * Returns state of communication with UAV. Also this is state
     * of UAV processing loop. Evaluation of allowed UserUavEvent
     * should be done by checking ongoing state.
     * Calling this method is thread safe.
     */
    ICommAction::Type getState(void) const;

private:
    // listener for events emitted from UAV
    IMulticopterMonitor* const monitor;

    // used communication buffer
    IAppCommInterface* interface;

    // preformed UAV action
    ICommAction* action;
    ICommAction* garbageAction;

    // dispatcher for Multicopter Comm Protocol
    CommDispatcher dispatcher;

    const double pingFreq, controlFreq;
    const double connectionTimeoutFreq;

    IAppTimer* pingTimer;
    IAppTimer* connetionTimer;

    // ping feature variables
    int sentPingValue;
    clock_t sentPingTime;

    // connection timeout variables
    std::atomic<bool> receptionFeed;
    std::atomic<bool> connectionLost;

    unsigned char messageBuildingBuffer[IMessage::MAX_DATA_SIZE];

    void notifyReception(const IMessage* const message);

    void handleError(const std::string& message);

    void pingTimerHandler(void);
    void handlePong(const SignalData& signalData) const;

    void connectionTimerHandler(void);

    // IAppCommInterface::Listener overrides
    void onConnected(void) override;
    void onDisconnected(void) override;
    void onError(const std::string& message) override;
    void onDataReceived(const unsigned char* data, const unsigned dataSize) override;

    // ICommAction::Listener overrides
    IMulticopterMonitor* getMonitor(void) override;
    void startAction(ICommAction* action, bool immediateStart = true) override;
    void onPongReception(const SignalData& pong) override;
    void send(const IMessage& message) override;
    void send(const ISignalPayloadMessage& message) override;
    void enablePingTask(bool enabled) override;
    void enableConnectionTimeoutTask(bool enabled) override;
    void connectInterface(IAppCommInterface* _interface) override;
    void disconnectInterface(void) override;
};

#endif // MULTICOPTERUAV_HPP
