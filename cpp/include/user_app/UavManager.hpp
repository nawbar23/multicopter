//#ifndef __UAV_MANAGER__
//#define __UAV_MANAGER__

//#include "IUavMonitor.hpp"

//#include "UavEvent.hpp"
//#include "UserUavEvent.hpp"

//#include "ICommHandler.hpp"

//#include <mutex>
//#include <atomic>
//#include <condition_variable>

///**
// * =============================================================================================
// * Purpose of this class is to reflect data state of controller board and provides high level API
// * for controlling the connected UAV. By this class user can start application loops and any action
// * over application loop, including flight loop. Also this class posts events from UAV as UavEvents
// * to all listeners.
// * =============================================================================================
// */
//class UavManager
//{
//public:
//    // any UAV action type has its equivalent in signal data command
//    typedef SignalData::Command UavAction;
//    typedef UserUavEvent::Type ExternalEvent;

//    /**
//     * UavManager
//     */
//    UavManager(ICommHandler* const _commHandler, IUavMonitor* const _uavMonitor,
//               const float _pingFreq, const float _controlFreq, const float _connectionTimeout);

//    virtual ~UavManager(void);

//    /**
//     * runAppLoop method has to be called from their own thread that will handle
//     * connection procedure and handling and will emit events through IUavMonitor
//     */
//    void runAppLoop(ICommInterface* commInterface);

//    /**
//     * runExternalSensorsLogger
//     */
//    void runExternalSensorsLogger(ICommInterface* commInterface);

//    /**
//     * runOfflineUpgrade
//     */
//    void runOfflineUpgrade(ICommInterface* commInterface);

//    /**
//     * Starts specified action. Throws exception when UAV thread can not
//     * start this action.
//     */
//    void preformAction(const UavAction& uavAction);

//    /**
//     * Starts upload of SignalPayloadMessage to board with propper signal fow.
//     * Throws exception when UAV thread can not start this action.
//     */
//    void preformActionUpload(const ISignalPayloadMessage& container);

//    /**
//     * Sends event tu UAV thread and deletes its pointer. This is the main
//     * way of communication with UAV thread.
//     */
//    void notifyUserUavEvent(const UserUavEvent* const userUavEvent);

//    /**
//     * Unconditionaly kills UAV thread, APPLICATION_LOOP_TERMINATED will be emitted.
//     * Should be used with deep understanding of consequences of killing connection.
//     */
//    void terminate(void);

//protected:
//    // handler for communication issues
//    ICommHandler* const commHandler;

//    // listener for events emitted from connection loops
//    IUavMonitor* const uavMonitor;

//    // intervals for frequent tasks over loops [ms]
//    const clock_t pingInterval, controlInterval;

//    // time after connection lost will be set to debug data when no data received
//    const clock_t connectionTimeout;

//    // synchronization features for comm control
//    std::mutex mutex;
//    std::condition_variable isExternalEventSet;

//    std::atomic<bool> inApplicationLoop;
//    std::atomic<bool> inFlightLoop;
//    std::atomic<bool> inExternalSensorsLogger;

//    std::atomic<UavAction> ongoingAction;
//    std::atomic<ExternalEvent> externalEvent;
//    std::atomic<bool> terminator;

//    // data structures that are to be sent to UAV
//    ISignalPayloadMessage* containerToSend;

//    // ping feature variables
//    int sentPingValue;
//    clock_t sentPingTime;

//    void connectionProcedure(void);

//    void applicationLoop(void);

//    void handleSignalData(const SignalData& signalData);
//    bool executeAction(void);

//    // main uav interaction methods
//    bool flightLoop(void);
//    bool calibrateAccelerometer(void);
//    bool calibrateMagnetometer(void);
//    bool uploadSettings(void);
//    bool downloadSettings(void);
//    bool calibrateEsc(void);
//    bool calibrateRadioReceiver(void);
//    bool checkRadioReceiver(void);
//    bool softwareUpgrade(void);
//    bool reset(void);
//    bool uploadRoute(void);
//    bool downloadRoute(void);
//    bool sensorsLogger(void);
//    bool configureWifi(void);

//    std::string externalSensorsLogger(void);

//    void triggerPing(void);
//    void handlePong(const SignalData& signalData) const;

//    void exceptNotConnected(void) const;
//    void exceptAlreadyPreforming(void) const;
//};

//#endif // __UAV_MANAGER__
