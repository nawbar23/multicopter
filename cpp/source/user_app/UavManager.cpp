#include "UavManager.hpp"

#include "Exception.hpp"

#include <stdlib.h>

std::string UavManager::toString(const SignalData& command)
{
	return toString(command.getCommand())
		+ ":"
		+ toString(command.getParameter());
}

std::string UavManager::toString(const SignalData::Command& command)
{
	switch (command)
	{
	case SignalData::DUMMY:
		return std::string("DUMMY");
	case SignalData::START_CMD:
		return std::string("START COMMAND");
    case SignalData::APP_LOOP:
        return std::string("APPLICATION LOOP");
	case SignalData::FLIGHT_LOOP:
		return std::string("FLIGHT LOOP");
	case SignalData::CALIBRATE_ACCEL:
		return std::string("CALIBRATE ACCELEROMETER");
	case SignalData::CALIBRATE_MAGNET:
		return std::string("CALIBRATE MAGNETOMETER");
	case SignalData::CALIBRATE_ESC:
		return std::string("CALIBRATE ESC");
	case SignalData::UPLOAD_SETTINGS:
		return std::string("UPLOAD SETTINGS");
	case SignalData::DOWNLOAD_SETTINGS:
		return std::string("DOWNLOAD SETTINGS");
	case SignalData::CALIBRATE_RADIO:
		return std::string("CALIBRATE RADIO");
	case SignalData::CHECK_RADIO:
		return std::string("CHECK RADIO");
	case SignalData::SOFTWARE_UPGRADE:
		return std::string("SOFTWARE UPGRADE");
	case SignalData::SYSTEM_RESET:
		return std::string("SYSTEM RESET");
	case SignalData::UPLOAD_ROUTE:
		return std::string("UPLOAD_ROUTE");
	case SignalData::DOWNLOAD_ROUTE:
		return std::string("DOWNLOAD_ROUTE");
	case SignalData::SENSORS_LOGGER:
		return std::string("SENSORS_LOGGER");
	case SignalData::CALIBRATION_SETTINGS:
		return std::string("CALIBRATION SETTINGS");
	case SignalData::CONTROL_SETTINGS:
		return std::string("CONTROL SETTINGS");
	case SignalData::ROUTE_CONTAINER:
		return std::string("ROUTE CONTAINER");
	case SignalData::CALIBRATION_SETTINGS_DATA:
		return std::string("CALIBRATION SETTINGS DATA");
	case SignalData::CONTROL_SETTINGS_DATA:
		return std::string("CONTROL SETTINGS DATA");
	case SignalData::ROUTE_CONTAINER_DATA:
		return std::string("ROUTE CONTAINER DATA");
	default:
		return std::string("Bad command type");
	}
}

std::string UavManager::toString(const SignalData::CommandParameter& parameter)
{
	switch (parameter)
	{
	case SignalData::DUMMY_PARAMETER:
		return std::string("DUMMY PARAMETER");
	case SignalData::START:
		return std::string("START");
	case SignalData::ACK:
		return std::string("ACK");
	case SignalData::DATA_ACK:
		return std::string("DATA ACK");
	case SignalData::BREAK:
		return std::string("BREAK");
	case SignalData::BREAK_ACK:
		return std::string("BREAK ACK");
	case SignalData::BREAK_FAIL:
		return std::string("BREAK FAIL");
	case SignalData::DONE:
		return std::string("DONE");
	case SignalData::READY:
		return std::string("READY");
	case SignalData::FAIL:
		return std::string("FAIL");
	case SignalData::SKIP:
		return std::string("SKIP");
	case SignalData::NON_STATIC:
		return std::string("NON STATIC");
	case SignalData::NOT_ALLOWED:
		return std::string("NOT ALLOWED");
	case SignalData::BAD_CRC:
		return std::string("BAD_CRC");
	case SignalData::TIMEOUT:
		return std::string("TIMEOUT");
	default:
		return std::string("Bad parameter type");
	}
}

UavManager::UavManager(IAppCommHandler* const _commHandler, IUavMonitor* const _uavMonitor, const float _pingFreq):
    commHandler(_commHandler), uavMonitor(_uavMonitor), pingInterval((unsigned)(1000 / _pingFreq))
{
    calibrationSettings = NULL;
    controlSettings = NULL;
    routeContainer = NULL;

    controlSettingsToSend = NULL;
    routeContainerToSend = NULL;

    externalEvent = ExternalEvent::ALLOW;
    ongoingAction = SignalData::DUMMY;

    inApplicationLoop = false;
    inFlightLoop = false;
    inExternalSensorsLogger = false;
}

UavManager::~UavManager(void)
{
    delete calibrationSettings;
    delete controlSettings;
    delete routeContainer;
}

void UavManager::preformAction(const UavAction& uavAction)
{
    if (getOngoingAction() == SignalData::DUMMY)
    {
        setOngoingAction(uavAction);
    }
    else
    {
        exceptAlreadyPreforming();
    }
}

void UavManager::preformActionUpload(const ControlSettings* const controlSettings)
{
    if (ongoingAction == SignalData::DUMMY)
    {
        *controlSettingsToSend = *controlSettings;
        setOngoingAction(SignalData::UPLOAD_SETTINGS);
    }
    else
    {
        exceptAlreadyPreforming();
    }
}

void UavManager::preformActionUpload(const RouteContainer* const routeContainer)
{
    if (ongoingAction == SignalData::DUMMY)
    {
        *routeContainerToSend = *routeContainer;
        setOngoingAction(SignalData::UPLOAD_ROUTE);
    }
    else
    {
        exceptAlreadyPreforming();
    }
}

void UavManager::setControlData(const ControlData& _controlData)
{
    controlDataToSend = _controlData;
}

void UavManager::notifyUserUavEvent(const UserUavEvent* userUavEvent)
{
    externalEvent = userUavEvent->getType();
    delete userUavEvent;
}

void UavManager::runAppLoop(ICommInterface* commInterface)
{
    commHandler->initialize(commInterface);
    try
    {
        connectionProcedure();
    }
    catch (Exception e)
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
        // go back to connect method caller
        return;
    }

    inApplicationLoop = true;
    externalEvent = ExternalEvent::ALLOW;
    ongoingAction = SignalData::DUMMY;

    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::APPLICATION_LOOP_STARTED));

    std::string exitMessage;
    try
    {
         exitMessage = applicationLoop();
    }
    catch (Exception e)
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
    }

    uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::APPLICATION_LOOP_ENDED, exitMessage));

    inApplicationLoop = false;
    externalEvent = ExternalEvent::ALLOW;
    ongoingAction = SignalData::DUMMY;
    // go back to connect method caller
}

void UavManager::runExternalSensorsLogger(ICommInterface* commInterface)
{
    commHandler->initialize(commInterface);
    try
    {
        externalSensorsLogger();
    }
    catch (Exception e)
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
    }
    // go back to connect method caller
}

void UavManager::setOngoingAction(const UavAction& _uavAction)
{
    mutex.lock();
    ongoingAction = _uavAction;
    mutex.unlock();
}

UavManager::UavAction UavManager::getOngoingAction(void)
{
    mutex.lock();
    UavAction result = ongoingAction;
    mutex.unlock();
    return result;
}

void UavManager::setExternalEvent(const ExternalEvent& _externalEvent)
{
    mutex.lock();
    externalEvent = _externalEvent;
    mutex.unlock();
}

UavManager::ExternalEvent UavManager::getExternalEvent(void)
{
    mutex.lock();
    ExternalEvent result = externalEvent;
    mutex.unlock();
    return result;
}

ControlData UavManager::getControlData(void)
{
    mutex.lock();
    ControlData result = controlDataToSend;
    mutex.unlock();
    return result;
}

void UavManager::connectionProcedure(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::START_CMD, SignalData::START),
        SignalData(SignalData::START_CMD, SignalData::ACK));

    bool wasNonStaticConditions = false;
    while (true)
    {
        const SignalData received = commHandler->waitForAnyCommandEx();
        if (received.getCommand() == SignalData::CALIBRATION_SETTINGS)
        {
            if (received.getParameter() == SignalData::NON_STATIC)
            {
                if (!wasNonStaticConditions)
                {
                    uavMonitor->notifyUavEvent(
                                new UavEvent(UavEvent::CALIBRATION_NON_STATIC));
                    wasNonStaticConditions = true;
                }
            }
            else if (received.getParameter() == SignalData::READY)
            {
                if (calibrationSettings == NULL)
                {
                    calibrationSettings = new CalibrationSettings(
                                commHandler->receiveCalibarionSettingsEx());
                }
                else
                {
                    *calibrationSettings = commHandler->receiveCalibarionSettingsEx();
                }
                uavMonitor->notifyDataReceived(calibrationSettings);
                break;
            }
            else
            {
                __RL_EXCEPTION__("Error command!");
            }
        }
    }

    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::APP_LOOP, SignalData::START),
        SignalData(SignalData::APP_LOOP, SignalData::ACK));
}


std::string UavManager::applicationLoop(void)
{
    unsigned debugCounter = 0;
    unsigned loopCounter = 0;

    double timeInAppLoop = 0;

    clock_t connectionTime = clock();
    clock_t commandTime = clock();
    clock_t pingTime = clock();

    DebugData debugData;
    while (1) // infinite loop
    {
        const IMessage::PreambleType type = commHandler->proceedReceiving();
        if (type != IMessage::EMPTY)
        {
            switch (type)
            {
            case IMessage::CONTROL:
                debugData = commHandler->getDebugData();
                uavMonitor->notifyDataReceived(&debugData);
                connectionTime = clock();
                debugCounter++;
                break;

            case IMessage::SIGNAL:
                handleSignalData(commHandler->getSignalData());
                break;

            default:
                // unexpected data received in app loop
                break;
            }
        }

        if ((clock() - connectionTime) > MAX_NO_CONNECTION_TIME)
        {
            debugData.setNoConnection();
        }

        if ((clock() - pingTime) > (int)pingInterval)
        {
            pingTime = clock();
            sentPingValue = std::rand();
            sentPingTime = clock();
            commHandler->send(SignalData(SignalData::PING_VALUE, sentPingValue));
        }

        if (getExternalEvent() == UserUavEvent::STOP_APPLICATION_LOOP)
        {
            break;
        }

        // allow to send only 1 command per second
        const bool allowCommand = ((clock() - commandTime) > 1000);
        if (getOngoingAction() != SignalData::DUMMY && allowCommand)
        {
            timeInAppLoop += (double)(clock() - commandTime) / CLOCKS_PER_SEC;
            executeAction();
            setOngoingAction(ongoingAction);
            setExternalEvent(UserUavEvent::ALLOW);
            commandTime = clock();
        }

        commHandler->holdThread(2);

        loopCounter++;
    }
    timeInAppLoop += (double)(clock() - commandTime) / CLOCKS_PER_SEC;

    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::APP_LOOP, SignalData::BREAK),
        SignalData(SignalData::APP_LOOP, SignalData::BREAK_ACK));

    const double failRate = commHandler->getReceptionFailes() != 0 ?
        commHandler->getReceptionFailes() / commHandler->getReceivedMessage() :
        0.0;

    const double debugFreq = debugCounter / timeInAppLoop;
    const double loopFreq = loopCounter / timeInAppLoop;

    std::string exitMessage("Application loop stopped succesfuly.\n");
    exitMessage += "Loop frequency: " + std::to_string(loopFreq) + " Hz.\n";
    exitMessage += "Reception frequency: " + std::to_string(debugFreq) + " Hz.\n";
    exitMessage += "Reception failure rate: " + std::to_string(failRate * 100) + "%.";

    return exitMessage;
}

void UavManager::handleSignalData(const SignalData& signalData)
{
    switch (signalData.getCommand())
    {
    case SignalData::APP_LOOP:
        __RL_EXCEPTION__("App. loop unexpectly shutdown by board, error!");

    case SignalData::PING_VALUE:
        if (signalData.getParameterValue() == sentPingValue)
        {
            uavMonitor->notifyPingUpdated(clock() - sentPingTime);
        }
        break;

    default:
        // unexpected signal data received in app loop
        break;
    }
}

bool UavManager::executeAction(void)
{
    switch (ongoingAction)
    {
//    case SignalData::FLIGHT_LOOP:
//        return flightLoop();
    case SignalData::CALIBRATE_ACCEL:
        return calibrateAccelerometer();
    case SignalData::CALIBRATE_MAGNET:
        return calibrateMagnetometer();
    case SignalData::UPLOAD_SETTINGS:
        return uploadSettings();
    case SignalData::DOWNLOAD_SETTINGS:
        return downloadSettings();
//    case SignalData::CALIBRATE_ESC:
//        return calibrateEsc();
//    case SignalData::CALIBRATE_RADIO:
//        return calibrateRadioReceiver();
    case SignalData::CHECK_RADIO:
        return checkRadioReceiver();
    case SignalData::SOFTWARE_UPGRADE:
        return softwareUpgrade();
    case SignalData::SYSTEM_RESET:
        return reset();
    case SignalData::UPLOAD_ROUTE:
        return uploadRoute();
    case SignalData::DOWNLOAD_ROUTE:
        return downloadRoute();
    case SignalData::SENSORS_LOGGER:
        return sensorsLogger();
    default:
        __RL_EXCEPTION__("Error! Unknown action type preformed.");
    }
}

//bool UavManager::flightLoop(void)
//{
//    // fast check for flight loop enable condition
//    if (!controlSettings.getData().isValid())
//    {
//        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//            "Control settings invalid, define your UAV.\n"
//            "It can be done easly in UAV tab"));
//        return false;
//    }

//    UserControlSettings userControlSettings;
//    try
//    {
//        userControlSettings = uavMonitor->getUserPreferences().getValidUserControlSettings();
//    }
//    catch (Exception e)
//    {
//        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//            "Define control settings, let us know what you pressed!\n"
//            "It can be done easly in CONTROLS tab"));
//        return false;
//    }

//    Joystick joystick(userControlSettings.deviceId);

//    if (!joystick.isGood())
//    {
//        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//            "No Joystick connected"));
//        return false;
//    }
//    joystick.update();
//    if (joystick.throttle > 0.03f)
//    {
//        // prevent throttle from being set higher than 3% just after start
//        joystick.blockThrottle();
//    }

//    // if fast flight conditions checker passed, send start command
//    SignalData initialResponse = commHandler.sendCommandGetAnyResponseEx(
//        SignalData(SignalData::FLIGHT_LOOP, SignalData::START));
//    if (initialResponse == SignalData(SignalData::FLIGHT_LOOP, SignalData::ACK))
//    {
//        // OK start flight loop
//    }
//    else if (initialResponse == SignalData(SignalData::FLIGHT_LOOP, SignalData::NOT_ALLOWED))
//    {
//        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//            "Filgih loop can not be preformed, check drone settings!"));
//        return false;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Bad parameter for fligth loop initial command response!");
//    }

//    // initial configuration for user application flight loop
//    // manager object for interaction with user
//    UserControlManager userControlManager(joystick, controlSettings.getData().getInitialSolverMode(), userControlSettings);
//    controlData = userControlManager.createControlData();

//    // initiate results repository
//    ResultsRepository::Results& results = ResultsRepository::get().
//        initializeResults(calibrationSettings, controlSettings,
//        uavMonitor->getUserPreferences().controlSettingsName);
//    Logger debugDataLog(results.path + "debugData.txt");
//    Logger controlDataLog(results.path + "controlData.txt");

//    commHandler.sendCommandEx(SignalData(SignalData::FLIGHT_LOOP, SignalData::READY));

//    uavMonitor->notifyEvent(
//        new IUavMonitor::Event(IUavMonitor::FLIGHT_LOOP, IUavMonitor::STARTED, ""));

//    // variables used by flight loop
//    const unsigned sendingInterval = 1000 / SENDING_FREQ;

//    bool brokenByBoard = false;
//    unsigned debugCounter = 0;
//    unsigned controlCounter = 0;
//    unsigned loopCounter = 0;

//    const clock_t startTime = clock();
//    clock_t controlTime = clock();
//    clock_t connectionTime = clock();

//    bool wasStopCondition = false;

//    while (!(externalEvent == STOP_APPLICATION_LOOP || externalEvent == STOP_FLIGHT_LOOP))
//    {
//        // processing received data
//        const IMessage::PreambleType type = commHandler.proceedReceiving();
//        if (type == IMessage::CONTROL)
//        {
//            debugData = commHandler.getDebugData();
//            debugDataLog << (float)(clock() - startTime) / CLOCKS_PER_SEC << debugData << std::endl;
//            connectionTime = clock();
//            debugCounter++;
//        }
//        else if (type == IMessage::SIGNAL && commHandler.getCommand() == SignalData::FLIGHT_LOOP)
//        {
//            brokenByBoard = (commHandler.getParameter() != SignalData::BREAK_ACK);
//            break;
//        }

//        // controls acquisition and sending
//        if ((clock() - controlTime) > sendingInterval)
//        {
//            controlTime = clock();

//            // sending control data to controller
//            controlData = userControlManager.createControlData();
//            if (wasStopCondition)
//            {
//                controlData.setControllerCommand(ControlData::STOP);
//            }
//            if (!commHandler.send(controlData))
//            {
//                // TODO handle sending error
//            }

//            const float actualTime = (float)(clock() - startTime) / CLOCKS_PER_SEC;
//            controlDataLog << actualTime << controlSettings.getData().formatEulers(controlData) << std::endl;
//            controlCounter++;
//        }

//        // check for connection lost
//        if ((clock() - connectionTime) > MAX_NO_CONNECTION_TIME)
//        {
//            debugData.setNoConnection();
//        }

//        if (GetKeyState(VK_ESCAPE) & 0x8000 || userControlManager.isStopButtonPressed())
//        {
//            // user set stop command
//            wasStopCondition = true;
//        }

//        loopCounter++;
//        Sleep(10);
//    }

//    // safe overall results information
//    results.duration = (float)(clock() - startTime) / CLOCKS_PER_SEC;
//    results.receivedPackets = debugCounter;
//    results.sentPackets = controlCounter;
//    results.loopCounter = loopCounter;

//    // store results data in repository
//    ResultsRepository::get().finalizeResults(results);

//    const float debugFreq = debugCounter / results.duration;
//    const float loopFreq = loopCounter / results.duration;

//    std::string exitMessage = "Flight loop ended.";
//    if (brokenByBoard)
//    {
//        exitMessage += "Coused by board.\n";
//    }
//    exitMessage += "\nLoop frequency: " + System::toString(loopFreq, 2) + " Hz.\n";
//    exitMessage += "Reception frequency: " + System::toString(debugFreq, 2) + " Hz.\n";

//    uavMonitor->notifyEvent(
//        new IUavMonitor::Event(IUavMonitor::FLIGHT_LOOP, IUavMonitor::ENDED, exitMessage));

//    return true;
//}

bool UavManager::calibrateAccelerometer(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::CALIBRATE_ACCEL, SignalData::START),
        SignalData(SignalData::CALIBRATE_ACCEL, SignalData::ACK));

    const SignalData response = commHandler->waitForAnyCommandEx();

    if (response == SignalData(SignalData::CALIBRATE_ACCEL, SignalData::DONE))
    {
        *calibrationSettings = commHandler->receiveCalibarionSettingsEx();
        uavMonitor->notifyDataReceived(calibrationSettings);
        return true;
    }
    else if (response == SignalData(SignalData::CALIBRATE_ACCEL, SignalData::NON_STATIC))
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
            "Accelerometer calibration aborted.\n"
            "Was preformed in non-static conditions."));
        return false;
    }
    else
    {
        __RL_EXCEPTION__("Bad data received on accelerometer calibration verdict.");
    }
}

bool UavManager::calibrateMagnetometer(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::CALIBRATE_MAGNET, SignalData::START),
        SignalData(SignalData::CALIBRATE_MAGNET, SignalData::ACK));

    // wait for user to do calibration manualy
    ExternalEvent externalEventLatch = ExternalEvent::ALLOW;
    while (!(externalEventLatch == ExternalEvent::STOP_APPLICATION_LOOP
        || externalEventLatch == ExternalEvent::STOP_MAGNET_CALIB_FAIL
        || externalEventLatch == ExternalEvent::STOP_MAGNET_CALIB_OK))
    {
        commHandler->holdThread(50);
        externalEventLatch = getExternalEvent();
    }

    if (externalEventLatch == ExternalEvent::STOP_APPLICATION_LOOP
            || externalEventLatch == ExternalEvent::STOP_MAGNET_CALIB_FAIL)
    {
        // stopped by user - break magnet calib and do not change calibration settings
        commHandler->sendCommandGetResponseEx(
            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::SKIP),
            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::ACK));
        return false;
    }
    else
    {
        const SignalData response = commHandler->sendCommandGetAnyResponseEx(
            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::DONE));

        if (response == SignalData(SignalData::CALIBRATE_MAGNET, SignalData::DONE))
        {
            *calibrationSettings = commHandler->receiveCalibarionSettingsEx();
            uavMonitor->notifyDataReceived(calibrationSettings);
            return true;
        }
        else if (response == SignalData(SignalData::CALIBRATE_MAGNET, SignalData::FAIL))
        {
            uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
                "Error during solving magnetometer calibration!\n"
                "Bad data acquired."));
            return false;
        }
        else
        {
            __RL_EXCEPTION__("Bad magnetometer calibration handler response.");
        }
    }
}

bool UavManager::uploadSettings(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::UPLOAD_SETTINGS, SignalData::START),
        SignalData(SignalData::UPLOAD_SETTINGS, SignalData::ACK));

    commHandler->sendDataProcedureEx(*controlSettingsToSend);
    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CONTROL_SETTINGS_UPLOADED));

    *controlSettings = *controlSettingsToSend;
    uavMonitor->notifyDataReceived(controlSettings);
    return true;
}

bool UavManager::downloadSettings(void)
{
    const SignalData response = commHandler->sendCommandGetAnyResponseEx(
        SignalData(SignalData::DOWNLOAD_SETTINGS, SignalData::START));

    if (response == SignalData(SignalData::DOWNLOAD_SETTINGS, SignalData::ACK))
    {
        if (controlSettings == NULL)
        {
            controlSettings = new ControlSettings(
                        commHandler->receiveControlSettingsEx());
        }
        else
        {
            *controlSettings = commHandler->receiveControlSettingsEx();
        }
        uavMonitor->notifyDataReceived(controlSettings);
        return true;
    }
    else if (response == SignalData(SignalData::DOWNLOAD_SETTINGS, SignalData::FAIL))
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
            "Error during downloading data from board, bad data safed in memory."));
        return false;
    }
    else
    {
        __RL_EXCEPTION__("Error during downloading data from board!\nBad board response.");
    }
}

//bool UavManager::calibrateEsc(void)
//{
//    commHandler.sendCommandGetResponseEx(
//        SignalData(SignalData::CALIBRATE_ESC, SignalData::START),
//        SignalData(SignalData::CALIBRATE_ESC, SignalData::ACK));

//    uavMonitor->notifyEvent(new IUavMonitor::Event(
//        IUavMonitor::CALIBRATE_ESC, IUavMonitor::STARTED, ""));

//    // in the ESC calibration there wil be triple user question, handle them
//    for (unsigned i = 0; i < 3; i++)
//    {
//        while (externalEvent == ALLOW)
//        {
//            // wait for command
//            System::sleep(50);
//        }
//        switch (externalEvent)
//        {
//        case DONE:
//            commHandler.sendCommandGetResponseEx(
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::DONE),
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::ACK));
//            break;
//        case BREAK:
//            commHandler.sendCommandGetResponseEx(
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_FAIL),
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_ACK));
//            if (i == 1)
//            {
//                uavMonitor->notifyEvent(new IUavMonitor::Event(
//                    IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER, "Calibration broken while computing!"));
//            }
//            else
//            {
//                uavMonitor->notifyEvent(new IUavMonitor::Event(
//                    IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER, ""));
//            }
//            return true;
//        default:
//            commHandler.sendCommandGetResponseEx(
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_FAIL),
//                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_ACK));
//            uavMonitor->notifyEvent(new IUavMonitor::Event(
//                IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER,
//                "Calibration aborted, bad user input!"));
//            return false;
//        }
//        externalEvent = ALLOW;
//    }
//    uavMonitor->notifyEvent(new IUavMonitor::Event(
//        IUavMonitor::CALIBRATE_ESC, IUavMonitor::ENDED, ""));
//    return true;
//}

//bool UavManager::calibrateRadioReceiver(void)
//{
//    const SignalData response = commHandler.sendCommandGetAnyResponseEx(
//        SignalData(SignalData::CALIBRATE_RADIO, SignalData::START));

//    if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
//    {
//        uavMonitor->notifyEvent(
//            new IUavMonitor::Event(IUavMonitor::CALIBRATE_RADIO, IUavMonitor::STARTED, ""));
//    }
//    else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::NOT_ALLOWED))
//    {
//        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//            "Radio receiver invaild\nCan not preform calibration."));
//        return false;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during starting calibration!\nBad board response.");
//    }

//    while (true) // infinite loop
//    {
//        if (externalEvent != ALLOW)
//        {
//            // handle event
//            switch (externalEvent)
//            {
//            case DONE:
//            case SKIP:
//            {
//                const SignalData response = commHandler.sendCommandGetAnyResponseEx(
//                    SignalData(SignalData::CALIBRATE_RADIO, getCommandParameterByExternalEvent(externalEvent)));

//                if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
//                {
//                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
//                        externalEvent, SignalData::ACK));
//                }
//                else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL))
//                {
//                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
//                        externalEvent, SignalData::BREAK_FAIL));
//                }
//                else
//                {
//                    __RL_EXCEPTION__("Error during accept command!\nBad board response.");
//                }
//                break;
//            }

//            case BREAK:
//            {
//                commHandler.sendCommandGetResponseEx(
//                    SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK),
//                    SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_ACK));

//                uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                    IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
//                    externalEvent, SignalData::BREAK_ACK));
//                externalEvent = ALLOW;
//                return false;
//            }

//            case CHECK:
//            {
//                // final radio check
//                externalEvent = ALLOW;
//                ControlData controlData;
//                while (externalEvent == ALLOW)
//                {
//                    const IMessage::PreambleType type = commHandler.proceedReceiving();
//                    if (type == IMessage::CONTROL)
//                    {
//                        controlData = commHandler.getControlData();
//                        // notify main window about reception
//                        uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                            IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER,
//                            "", controlData, CHECK, CHECK));
//                    }
//                    Sleep(10);
//                }

//                SignalData response;
//                switch (externalEvent)
//                {
//                case DONE:
//                    // user accepted calibration results
//                    response = commHandler.sendCommandGetAnyResponseEx(
//                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK));

//                    if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
//                    {
//                        calibrationSettings = commHandler.receiveCalibarionSettingsEx();
//                        uavMonitor->notifyEvent(new IUavMonitor::Event(
//                            IUavMonitor::CALIBRATION_SETTINGS_CHANGED, IUavMonitor::DATA_CHANGED, ""));
//                    }
//                    else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL))
//                    {
//                        uavMonitor->notifyEvent(new IUavMonitor::Event(
//                            IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
//                            "Error during ending radio calibration!\n"
//                            "Failed to safe data in memmory."));
//                    }
//                    else
//                    {
//                        __RL_EXCEPTION__("Error during ending radio calibration!\nBad board response.");
//                    }

//                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
//                        BREAK, SignalData::BREAK_ACK));

//                    externalEvent = ALLOW;
//                    return true;

//                case BREAK:
//                    // user rejected calibration results
//                    commHandler.sendCommandGetResponseEx(
//                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL),
//                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_ACK));

//                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
//                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
//                        BREAK, SignalData::BREAK_ACK));

//                    externalEvent = ALLOW;
//                    return true;

//                default:
//                    __RL_EXCEPTION__("Error during radio check ending!\nBad user command.");
//                }
//            }

//            default:
//                __RL_EXCEPTION__("Error during handling event!\nBad event received at radio calibration.");
//            }
//            externalEvent = ALLOW;
//        }
//        Sleep(50);
//    }
//}

bool UavManager::checkRadioReceiver(void)
{
    const SignalData response = commHandler->sendCommandGetAnyResponseEx(
        SignalData(SignalData::CHECK_RADIO, SignalData::START));

    if (response == SignalData(SignalData::CHECK_RADIO, SignalData::ACK))
    {
        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CHECK_RADIO_STARTED));
    }
    else if (response == SignalData(SignalData::CHECK_RADIO, SignalData::NOT_ALLOWED))
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
            "Error during radio check startup!\n"
            "No radio receiver connected or bad control settings stored in memmory"));
        return false;
    }
    else
    {
        __RL_EXCEPTION__("Error during radio check startup!\nBad board response.");
    }

    const ControlSettings boardControlSettings = commHandler->receiveControlSettingsEx();
    uavMonitor->notifyDataReceived(&boardControlSettings);

    ControlData radioControlData;
    ExternalEvent externalEventLatch = ExternalEvent::ALLOW;
    while (!(externalEventLatch == ExternalEvent::STOP_APPLICATION_LOOP
        || externalEventLatch == ExternalEvent::STOP_RADIO_CHECK))
    {
        const IMessage::PreambleType type = commHandler->proceedReceiving();
        if (type == IMessage::CONTROL)
        {
            radioControlData = commHandler->getControlData();
            uavMonitor->notifyDataReceived(&radioControlData);
        }
        commHandler->holdThread(2);
        externalEventLatch = getExternalEvent();
    }

    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CHECK_RADIO_ENDED));

    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::CHECK_RADIO, SignalData::BREAK),
        SignalData(SignalData::CHECK_RADIO, SignalData::BREAK_ACK));

    return true;
}

bool UavManager::softwareUpgrade(void)
{
    const SignalData response = commHandler->sendCommandGetAnyResponseEx(
        SignalData(SignalData::SOFTWARE_UPGRADE, SignalData::START));

    if (response == SignalData(SignalData::SOFTWARE_UPGRADE, SignalData::ACK))
    {
        // connection is broken now
        inApplicationLoop = false;

        // start programming over DFU sequence
        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::UPGRADE_STARTED));

        // TODO application loop thread should end here and now !!!
        // application thread is ended now
        _endthread();

        // just for compiler perpouse, this method should never ends here
        __RL_EXCEPTION__("Error during software update startup!\n"
                         "Unknown resason, method riched end.");
    }
    else if (response == SignalData(SignalData::SOFTWARE_UPGRADE, SignalData::NOT_ALLOWED))
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
            "Upgrade via radio connection is forbiden, aborting!"));
        return false;
    }
    else
    {
        __RL_EXCEPTION__("Error during software update startup!\nBad board response.");
    }
}

bool UavManager::reset(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::SYSTEM_RESET, SignalData::START),
        SignalData(SignalData::SYSTEM_RESET, SignalData::ACK));

    // connection is broken now
    inApplicationLoop = false;

    uavMonitor->notifyUavEvent(new UavEventMessage(
        UavEvent::APPLICATION_LOOP_ENDED,
        "Connection closed after reset."));

    // TODO application loop thread should end here and now !!!
    // application thread is ended now
    _endthread();

    // just for compiler perpouse, this method should never ends here
    __RL_EXCEPTION__("Error during system reset!\n"
                     "Unknown resason, method riched end.");
}

bool UavManager::uploadRoute(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::UPLOAD_ROUTE, SignalData::START),
        SignalData(SignalData::UPLOAD_ROUTE, SignalData::ACK));

    commHandler->sendDataProcedureEx(*routeContainerToSend);
    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::ROUTE_UPLOADED));

    *routeContainer = *routeContainerToSend;
    uavMonitor->notifyDataReceived(routeContainer);
    return true;
}

bool UavManager::downloadRoute(void)
{
    const SignalData response = commHandler->sendCommandGetAnyResponseEx(
        SignalData(SignalData::DOWNLOAD_ROUTE, SignalData::START));

    if (response == SignalData(SignalData::DOWNLOAD_ROUTE, SignalData::ACK))
    {
        if (routeContainer == NULL)
        {
            routeContainer = new RouteContainer(
                        commHandler->receiveRouteContainerEx());
        }
        else
        {
            *routeContainer = commHandler->receiveRouteContainerEx();
        }
        uavMonitor->notifyDataReceived(routeContainer);
        return true;
    }
    else if (response == SignalData(SignalData::DOWNLOAD_ROUTE, SignalData::FAIL))
    {
        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE,
            "Error during downloading data from board, bad data safed in memory."));
        return false;
    }
    else
    {
        __RL_EXCEPTION__("Error during downloading data from board!\nBad board response.");
    }
}

bool UavManager::sensorsLogger(void)
{
    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::SENSORS_LOGGER, SignalData::START),
        SignalData(SignalData::SENSORS_LOGGER, SignalData::ACK));

    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::SENSORS_LOGGER_STARTED));

    SensorsData sensorsData;

    ExternalEvent externalEventLatch = ExternalEvent::ALLOW;
    while (!(externalEventLatch == ExternalEvent::STOP_APPLICATION_LOOP
        || externalEventLatch == ExternalEvent::STOP_SENSORS_LOGGER))
    {
        const IMessage::PreambleType type = commHandler->proceedReceiving();
        if (type == IMessage::CONTROL)
        {
            sensorsData = commHandler->getSensorsData();
            uavMonitor->notifyDataReceived(&sensorsData);
        }
        commHandler->holdThread(1);
        externalEventLatch = getExternalEvent();
    }

    commHandler->sendCommandGetResponseEx(
        SignalData(SignalData::SENSORS_LOGGER, SignalData::BREAK),
        SignalData(SignalData::SENSORS_LOGGER, SignalData::BREAK_ACK));

    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::SENSORS_LOGGER_ENDED));

    return true;
}

std::string UavManager::externalSensorsLogger(void)
{
    //sensorsLogger(EXTERNAL_TRUE);
    return "dummy msg";
}

void UavManager::exceptAlreadyPreforming(void) const
{
    const std::string msg = "Error, already preforming: "
            + toString(SignalData::Command(ongoingAction));
    __RL_EXCEPTION__(msg.c_str());
}
