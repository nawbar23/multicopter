#ifndef __UAV_MANAGER__
#define __UAV_MANAGER__

#include "IUavMonitor.hpp"

#include "UavEvent.hpp"
#include "UserUavEvent.hpp"

#include "IAppCommHandler.hpp"

#include <mutex>

class UavManager
{
public:
    // any UAV action type has its equivalent in signal data command
    typedef SignalData::Command UavAction;
    typedef UserUavEvent::Type ExternalEvent;

	static std::string toString(const SignalData& command);
    static std::string toString(const SignalData::Command& command);
    static std::string toString(const SignalData::CommandParameter& parameter);

    UavManager(IAppCommHandler* const _commHandler, IUavMonitor* const _uavMonitor, const float _pingFreq);

    virtual ~UavManager(void);

    void preformAction(const UavAction& uavAction);
    void preformActionUpload(const ControlSettings* const controlSettings);
    void preformActionUpload(const RouteContainer* const routeContainer);

    void setControlData(const ControlData& controlData);

    void notifyUserUavEvent(const UserUavEvent* userUavEvent);

    // these 'connect' methods has to be called from their own thread that will handle
    // connection procedure and handling and will emit events through IUavMonitor
    void runAppLoop(ICommInterface* commInterface);
    void runExternalSensorsLogger(ICommInterface* commInterface);

    // get stored signal payload messages received from boards
    const CalibrationSettings* getCalibrationSettings(void) const;
    const ControlSettings* getControlSettings(void) const;
    const RouteContainer* getRouteContainer(void) const;

protected:
    static const long MAX_NO_CONNECTION_TIME = 1500; // [ms]
    static const unsigned SENDING_FREQ = 25; // [Hz]

    // mutex for in-connection data handling
    // applied for app. loop control: externalEvents and actions
    // setting flight loop control data to send
    std::mutex mutex;

    void setOngoingAction(const UavAction& _uavAction);
    UavAction getOngoingAction(void);

    void setExternalEvent(const ExternalEvent& _externalEvent);
    ExternalEvent getExternalEvent(void);

    ControlData getControlData(void);

    // handler for communication issues
    IAppCommHandler* const commHandler;

    // listener for events emitted from connection loops
    IUavMonitor* const uavMonitor;

    // data structures that reflects board state
    CalibrationSettings* calibrationSettings;
    ControlSettings* controlSettings;
    RouteContainer* routeContainer;

    // data structures that are to be sent to UAV
    ControlData controlDataToSend;
    ControlSettings* controlSettingsToSend;
    RouteContainer* routeContainerToSend;

    UavAction ongoingAction;
    ExternalEvent externalEvent;

    const unsigned pingInterval;

    int sentPingValue;
    clock_t sentPingTime;

    bool inApplicationLoop;
    bool inFlightLoop;
    bool inExternalSensorsLogger;

    void connectionProcedure(void);

    std::string applicationLoop(void);

    void handleSignalData(const SignalData& signalData);
    bool executeAction(void);

    // main uav interaction methods
    bool flightLoop(void);
    bool calibrateAccelerometer(void);
    bool calibrateMagnetometer(void);
    bool uploadSettings(void);
    bool downloadSettings(void);
    bool calibrateEsc(void);
    bool calibrateRadioReceiver(void);
    bool checkRadioReceiver(void);
    bool softwareUpgrade(void);
    bool reset(void);
    bool uploadRoute(void);
    bool downloadRoute(void);
    bool sensorsLogger(void);

    std::string externalSensorsLogger(void);

    void exceptAlreadyPreforming(void) const;
};

#endif // __UAV_MANAGER__
