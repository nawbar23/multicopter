//#include "UavManager.hpp"

//#include "Exception.hpp"

//#include <stdlib.h>

//UavManager::UavManager(ICommHandler* const _commHandler, IUavMonitor* const _uavMonitor,
//                       const float _pingFreq, const float _controlFreq, const float _connectionTimeout):
//    commHandler(_commHandler),
//    uavMonitor(_uavMonitor),
//    pingInterval((clock_t)(1000 / _pingFreq)),
//    controlInterval((clock_t)(1000 / _controlFreq)),
//    connectionTimeout((clock_t)(_connectionTimeout * 1000))
//{
//    containerToSend = NULL;

//    externalEvent = ExternalEvent::ALLOW;
//    ongoingAction = SignalData::DUMMY;

//    inApplicationLoop = false;
//    inFlightLoop = false;
//    inExternalSensorsLogger = false;
//}

//UavManager::~UavManager(void)
//{
//    delete containerToSend;
//}

//void UavManager::runAppLoop(ICommInterface* commInterface)
//{
//    commHandler->initialize(commInterface);
//    try
//    {
//        connectionProcedure();
//    }
//    catch (Exception e)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
//        // go back to connect method caller
//        return;
//    }

//    inApplicationLoop = true;
//    externalEvent = ExternalEvent::ALLOW;
//    ongoingAction = SignalData::DUMMY;
//    terminator = false;

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::APPLICATION_LOOP_STARTED));

//    try
//    {
//        applicationLoop();
//    }
//    catch (Exception e)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
//    }

//    inApplicationLoop = false;
//    externalEvent = ExternalEvent::ALLOW;
//    ongoingAction = SignalData::DUMMY;
//    // go back to connect method caller
//}

//void UavManager::runExternalSensorsLogger(ICommInterface* commInterface)
//{
//    commHandler->initialize(commInterface);
//    try
//    {
//        externalSensorsLogger();
//    }
//    catch (Exception e)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
//    }
//    // go back to connect method caller
//}

//void UavManager::runOfflineUpgrade(ICommInterface* commInterface)
//{
//    commHandler->initialize(commInterface);
//    try
//    {
//        commHandler->sendCommandGetResponseEx(
//                    SignalData(SignalData::START_CMD, SignalData::ENTER_DFU),
//                    SignalData(SignalData::START_CMD, SignalData::ACK));
//        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::UPGRADE_STARTED));
//    }
//    catch (Exception e)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::ERROR_MESSAGE, e.what()));
//    }
//    // go back to connect method caller
//}

//void UavManager::preformAction(const UavAction& uavAction)
//{
//    if (false == inApplicationLoop)
//    {
//        exceptNotConnected();
//    }
//    if (SignalData::DUMMY == ongoingAction)
//    {
//        ongoingAction = uavAction;
//    }
//    else
//    {
//        exceptAlreadyPreforming();
//    }
//}

//void UavManager::preformActionUpload(const ISignalPayloadMessage& container)
//{
//    if (false == inApplicationLoop)
//    {
//        exceptNotConnected();
//    }
//    if (SignalData::DUMMY == ongoingAction)
//    {
//        if (containerToSend != NULL)
//        {
//            delete containerToSend;
//        }
//        containerToSend = container.clone();
//        ongoingAction = container.getUploadAction();
//    }
//    else
//    {
//        exceptAlreadyPreforming();
//    }
//}

//void UavManager::notifyUserUavEvent(const UserUavEvent* const userUavEvent)
//{
//    externalEvent = userUavEvent->getType();
//    isExternalEventSet.notify_all();
//    delete userUavEvent;
//}

//void UavManager::terminate(void)
//{
//    terminator = true;
//}

//void UavManager::connectionProcedure(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::START_CMD, SignalData::START),
//        SignalData(SignalData::START_CMD, SignalData::ACK));

//    bool wasNonStaticConditions = false;
//    while (true)
//    {
//        const SignalData received = commHandler->waitForAnyCommandEx();
//        if (received.getCommand() == SignalData::CALIBRATION_SETTINGS)
//        {
//            if (received.getParameter() == SignalData::NON_STATIC)
//            {
//                if (!wasNonStaticConditions)
//                {
//                    uavMonitor->notifyUavEvent(
//                                new UavEvent(UavEvent::CALIBRATION_NON_STATIC));
//                    wasNonStaticConditions = true;
//                }
//            }
//            else if (received.getParameter() == SignalData::READY)
//            {
//                CalibrationSettings calibrationSettings =
//                        commHandler->receiveCalibarionSettingsEx();
//                uavMonitor->notifyDataReceived(&calibrationSettings);
//                break;
//            }
//            else
//            {
//                __RL_EXCEPTION__("Error command!");
//            }
//        }
//    }

//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::APP_LOOP, SignalData::START),
//        SignalData(SignalData::APP_LOOP, SignalData::ACK));
//}


//void UavManager::applicationLoop(void)
//{
//    clock_t loopTime = clock();
//    clock_t connectionTime = clock();
//    clock_t commandTime = clock();
//    clock_t pingTime = clock();

//    unsigned loopCounter = 0;
//    double timeInLoop = 0.0;

//    DebugData debugData;
//    while (externalEvent != UserUavEvent::STOP_APPLICATION_LOOP)
//    {
//        const IMessage::PreambleType type = commHandler->proceedReceiving();
//        if (type != IMessage::EMPTY)
//        {
//            switch (type)
//            {
//            case IMessage::CONTROL:
//                debugData = commHandler->getDebugData();
//                uavMonitor->notifyDataReceived(&debugData);
//                connectionTime = clock();
//                break;

//            case IMessage::SIGNAL:
//                handleSignalData(commHandler->getSignalData());
//                break;

//            default:
//                // unexpected data received in app loop
//                break;
//            }
//        }

//        if ((clock() - connectionTime) > connectionTimeout)
//        {
//            debugData.setNoConnection();
//        }

//        if ((clock() - pingTime) > pingInterval)
//        {
//            pingTime = clock();
//            triggerPing();
//        }

//        // allow to send only 1 command per second
//        const bool allowCommand = ((clock() - commandTime) > 1000);
//        if (ongoingAction != SignalData::DUMMY && allowCommand)
//        {
//            timeInLoop += (clock() - loopTime) / 1000.0;
//            if (false == executeAction())
//            {
//                // connection terminated by command, return silently
//                return;
//            }
//            ongoingAction = UavAction::DUMMY;
//            externalEvent = UserUavEvent::ALLOW;
//            commandTime = clock();
//            loopTime = clock();
//        }

//        if (terminator)
//        {
//            // connection terminated by command, return silently
//            return;
//        }

//        commHandler->holdThread(1);

//        loopCounter++;
//    }

//    timeInLoop += (clock() - loopTime) / 1000.0;

//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::APP_LOOP, SignalData::BREAK),
//        SignalData(SignalData::APP_LOOP, SignalData::BREAK_ACK));


//    uavMonitor->notifyUavEvent(new UavEventMessage(
//        UavEvent::APPLICATION_LOOP_ENDED,
//        "Application loop exited successfully."));
//}

//void UavManager::handleSignalData(const SignalData& signalData)
//{
//    switch (signalData.getCommand())
//    {
//    case SignalData::APP_LOOP:
//        __RL_EXCEPTION__("App. loop unexpectly shutdown by board, error!");

//    case SignalData::PING_VALUE:
//        handlePong(signalData);
//        break;

//    default:
//        // unexpected signal data received in app loop
//        break;
//    }
//}

//bool UavManager::executeAction(void)
//{
//    switch (ongoingAction)
//    {
//    case SignalData::FLIGHT_LOOP:
//        return flightLoop();
//    case SignalData::CALIBRATE_ACCEL:
//        return calibrateAccelerometer();
//    case SignalData::CALIBRATE_MAGNET:
//        return calibrateMagnetometer();
//    case SignalData::UPLOAD_SETTINGS:
//        return uploadSettings();
//    case SignalData::DOWNLOAD_SETTINGS:
//        return downloadSettings();
////    case SignalData::CALIBRATE_ESC:
////        return calibrateEsc();
////    case SignalData::CALIBRATE_RADIO:
////        return calibrateRadioReceiver();
//    case SignalData::CHECK_RADIO:
//        return checkRadioReceiver();
//    case SignalData::SOFTWARE_UPGRADE:
//        return softwareUpgrade();
//    case SignalData::SYSTEM_RESET:
//        return reset();
//    case SignalData::UPLOAD_ROUTE:
//        return uploadRoute();
//    case SignalData::DOWNLOAD_ROUTE:
//        return downloadRoute();
//    case SignalData::SENSORS_LOGGER:
//        return sensorsLogger();
//    case SignalData::CONFIGURE_WIFI:
//        return configureWifi();
//    default:
//        __RL_EXCEPTION__("Error! Unknown action type preformed.");
//    }
//}

//bool UavManager::flightLoop(void)
//{
//    // send start command
//    const SignalData::Parameter initialResponse = commHandler->sendCommandGetAnyParameterEx(
//        SignalData(SignalData::FLIGHT_LOOP, SignalData::START), SignalData::FLIGHT_LOOP);
//    if (initialResponse == SignalData::ACK)
//    {
//        const ControlSettings controlSettings =
//                commHandler->receiveControlSettingsEx();
//        uavMonitor->notifyDataReceived(&controlSettings);
//    }
//    else if (initialResponse == SignalData::NOT_ALLOWED)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::FLIGHT_LOOP_NOT_ALLOWED,
//            "Flight loop can not be preformed, check drone settings on board!"));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Bad response for fligth loop initial command response!");
//    }

//    const SignalData::Parameter routeResponse = commHandler->waitForAnyParameterEx(SignalData::FLIGHT_LOOP);
//    if (routeResponse == SignalData::VIA_ROUTE_ALLOWED)
//    {
//        const RouteContainer routeContainer =
//                commHandler->receiveRouteContainerEx();
//        uavMonitor->notifyDataReceived(&routeContainer);
//    }
//    else if (routeResponse == SignalData::VIA_ROUTE_NOT_ALLOWED)
//    {
//        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::VIA_ROUTE_NOT_ALLOWED));
//    }
//    else
//    {
//        __RL_EXCEPTION__("Bad parameter for fligth loop route command response!");
//    }

//    bool brokenByBoard = false;
//    unsigned loopCounter = 0;

//    clock_t controlTime = clock();
//    clock_t connectionTime = clock();
//    clock_t pingTime = clock();

//    DebugData debugData;
//    ControlData controlData;

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::FLIGHT_LOOP_STARTED));

//    commHandler->sendCommandEx(SignalData(SignalData::FLIGHT_LOOP, SignalData::READY));

//    while (true) // infinite loop
//    {
//        // processing received data
//        const IMessage::PreambleType type = commHandler->proceedReceiving();
//        if (IMessage::CONTROL == type)
//        {
//            debugData = commHandler->getDebugData();
//            uavMonitor->notifyDataReceived(&debugData);
//            connectionTime = clock();
//        }
//        else if (IMessage::SIGNAL == type)
//        {
//            const SignalData signalData = commHandler->getSignalData();
//            if (SignalData::FLIGHT_LOOP == signalData.getCommand())
//            {
//                brokenByBoard = (signalData.getParameter() != SignalData::BREAK_ACK);
//                break;
//            }
//            else if (SignalData::PING_VALUE == signalData.getCommand())
//            {
//                handlePong(signalData);
//            }
//        }

//        // controls acquisition and sending
//        if ((clock() - controlTime) > controlInterval)
//        {
//            controlTime = clock();
//            controlData = uavMonitor->getControlDataToSend();
//            if (!commHandler->send(controlData))
//            {
//                // TODO handle sending error
//            }
//        }

//        // check for connection lost
//        if ((clock() - connectionTime) > connectionTimeout)
//        {
//            debugData.setNoConnection();
//            if (ControlData::STOP == controlData.getControllerCommand())
//            {
//                // if user set stop command and connection is lost - break the loop
//                break;
//            }
//        }

//        // pinging
//        if ((clock() - pingTime) > pingInterval)
//        {
//            pingTime = clock();
//            triggerPing();
//        }

//        if (terminator)
//        {
//            // process was terminated externaly
//            return false;
//        }

//        commHandler->holdThread(1);

//        loopCounter++;
//    }

//    std::string exitMessage = "Flight loop ended.";
//    if (brokenByBoard)
//    {
//        exitMessage += "Coused by board.\n";
//    }

//    uavMonitor->notifyUavEvent(new UavEventMessage(
//                                   UavEvent::FLIGHT_LOOP_ENDED,
//                                   exitMessage));

//    return true;
//}

//bool UavManager::calibrateAccelerometer(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::CALIBRATE_ACCEL, SignalData::START),
//        SignalData(SignalData::CALIBRATE_ACCEL, SignalData::ACK));

//    const SignalData response = commHandler->waitForAnyCommandEx();

//    if (response == SignalData(SignalData::CALIBRATE_ACCEL, SignalData::DONE))
//    {
//        CalibrationSettings calibrationSettings =
//                commHandler->receiveCalibarionSettingsEx();
//        uavMonitor->notifyDataReceived(&calibrationSettings);
//        return true;
//    }
//    else if (response == SignalData(SignalData::CALIBRATE_ACCEL, SignalData::NON_STATIC))
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::INFO_MESSAGE,
//            "Accelerometer calibration aborted.\n"
//            "Was preformed in non-static conditions."));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Bad data received on accelerometer calibration verdict.");
//    }
//}

//bool UavManager::calibrateMagnetometer(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::CALIBRATE_MAGNET, SignalData::START),
//        SignalData(SignalData::CALIBRATE_MAGNET, SignalData::ACK));

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CALIBRATE_MAGNET_STARTED));

//    // wait for user to do calibration manualy
//    while (!(externalEvent == ExternalEvent::STOP_APPLICATION_LOOP
//        || externalEvent == ExternalEvent::STOP_MAGNET_CALIB_FAIL
//        || externalEvent == ExternalEvent::STOP_MAGNET_CALIB_OK))
//    {
//        std::mutex mtx;
//        std::unique_lock<std::mutex> lock(mtx);
//        isExternalEventSet.wait(lock);
//        lock.unlock();
//    }

//    if (externalEvent == ExternalEvent::STOP_APPLICATION_LOOP
//            || externalEvent == ExternalEvent::STOP_MAGNET_CALIB_FAIL)
//    {
//        // stopped by user - break magnet calib and do not change calibration settings
//        commHandler->sendCommandGetResponseEx(
//            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::SKIP),
//            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::ACK));
//        return true;
//    }
//    else
//    {
//        const SignalData response = commHandler->sendCommandGetAnyResponseEx(
//            SignalData(SignalData::CALIBRATE_MAGNET, SignalData::DONE));

//        if (response == SignalData(SignalData::CALIBRATE_MAGNET, SignalData::DONE))
//        {
//            CalibrationSettings calibrationSettings =
//                    commHandler->receiveCalibarionSettingsEx();
//            uavMonitor->notifyDataReceived(&calibrationSettings);
//            return true;
//        }
//        else if (response == SignalData(SignalData::CALIBRATE_MAGNET, SignalData::FAIL))
//        {
//            uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::WARNING_MESSAGE,
//                "Error during solving magnetometer calibration!\n"
//                "Bad data acquired."));
//            return true;
//        }
//        else
//        {
//            __RL_EXCEPTION__("Bad magnetometer calibration handler response.");
//        }
//    }
//}

//bool UavManager::uploadSettings(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::UPLOAD_SETTINGS, SignalData::START),
//        SignalData(SignalData::UPLOAD_SETTINGS, SignalData::ACK));

//    commHandler->sendDataProcedureEx(*containerToSend);
//    uavMonitor->notifyUavEvent(new UavEventUploaded(UavEvent::CONTROL_SETTINGS_UPLOADED,
//                                                    *containerToSend));
//    return true;
//}

//bool UavManager::downloadSettings(void)
//{
//    const SignalData::Parameter response = commHandler->sendCommandGetAnyParameterEx(
//        SignalData(SignalData::DOWNLOAD_SETTINGS, SignalData::START), SignalData::DOWNLOAD_SETTINGS);

//    if (SignalData::ACK == response)
//    {
//        ControlSettings controlSettings =
//                commHandler->receiveControlSettingsEx();
//        uavMonitor->notifyDataReceived(&controlSettings);
//        return true;
//    }
//    else if (SignalData::FAIL == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::INFO_MESSAGE,
//            "Error during downloading data from board, bad data safed in memory."));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during downloading data from board!\nBad board response.");
//    }
//}

////bool UavManager::calibrateEsc(void)
////{
////    commHandler.sendCommandGetResponseEx(
////        SignalData(SignalData::CALIBRATE_ESC, SignalData::START),
////        SignalData(SignalData::CALIBRATE_ESC, SignalData::ACK));

////    uavMonitor->notifyEvent(new IUavMonitor::Event(
////        IUavMonitor::CALIBRATE_ESC, IUavMonitor::STARTED, ""));

////    // in the ESC calibration there wil be triple user question, handle them
////    for (unsigned i = 0; i < 3; i++)
////    {
////        while (externalEvent == ALLOW)
////        {
////            // wait for command
////            System::sleep(50);
////        }
////        switch (externalEvent)
////        {
////        case DONE:
////            commHandler.sendCommandGetResponseEx(
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::DONE),
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::ACK));
////            break;
////        case BREAK:
////            commHandler.sendCommandGetResponseEx(
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_FAIL),
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_ACK));
////            if (i == 1)
////            {
////                uavMonitor->notifyEvent(new IUavMonitor::Event(
////                    IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER, "Calibration broken while computing!"));
////            }
////            else
////            {
////                uavMonitor->notifyEvent(new IUavMonitor::Event(
////                    IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER, ""));
////            }
////            return true;
////        default:
////            commHandler.sendCommandGetResponseEx(
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_FAIL),
////                SignalData(SignalData::CALIBRATE_ESC, SignalData::BREAK_ACK));
////            uavMonitor->notifyEvent(new IUavMonitor::Event(
////                IUavMonitor::CALIBRATE_ESC, IUavMonitor::OTHER,
////                "Calibration aborted, bad user input!"));
////            return false;
////        }
////        externalEvent = ALLOW;
////    }
////    uavMonitor->notifyEvent(new IUavMonitor::Event(
////        IUavMonitor::CALIBRATE_ESC, IUavMonitor::ENDED, ""));
////    return true;
////}

////bool UavManager::calibrateRadioReceiver(void)
////{
////    const SignalData response = commHandler.sendCommandGetAnyResponseEx(
////        SignalData(SignalData::CALIBRATE_RADIO, SignalData::START));

////    if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
////    {
////        uavMonitor->notifyEvent(
////            new IUavMonitor::Event(IUavMonitor::CALIBRATE_RADIO, IUavMonitor::STARTED, ""));
////    }
////    else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::NOT_ALLOWED))
////    {
////        uavMonitor->notifyEvent(new IUavMonitor::Event(IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
////            "Radio receiver invaild\nCan not preform calibration."));
////        return false;
////    }
////    else
////    {
////        __RL_EXCEPTION__("Error during starting calibration!\nBad board response.");
////    }

////    while (true) // infinite loop
////    {
////        if (externalEvent != ALLOW)
////        {
////            // handle event
////            switch (externalEvent)
////            {
////            case DONE:
////            case SKIP:
////            {
////                const SignalData response = commHandler.sendCommandGetAnyResponseEx(
////                    SignalData(SignalData::CALIBRATE_RADIO, getCommandParameterByExternalEvent(externalEvent)));

////                if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
////                {
////                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
////                        externalEvent, SignalData::ACK));
////                }
////                else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL))
////                {
////                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
////                        externalEvent, SignalData::BREAK_FAIL));
////                }
////                else
////                {
////                    __RL_EXCEPTION__("Error during accept command!\nBad board response.");
////                }
////                break;
////            }

////            case BREAK:
////            {
////                commHandler.sendCommandGetResponseEx(
////                    SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK),
////                    SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_ACK));

////                uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                    IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
////                    externalEvent, SignalData::BREAK_ACK));
////                externalEvent = ALLOW;
////                return false;
////            }

////            case CHECK:
////            {
////                // final radio check
////                externalEvent = ALLOW;
////                ControlData controlData;
////                while (externalEvent == ALLOW)
////                {
////                    const IMessage::PreambleType type = commHandler.proceedReceiving();
////                    if (type == IMessage::CONTROL)
////                    {
////                        controlData = commHandler.getControlData();
////                        // notify main window about reception
////                        uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                            IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER,
////                            "", controlData, CHECK, CHECK));
////                    }
////                    Sleep(10);
////                }

////                SignalData response;
////                switch (externalEvent)
////                {
////                case DONE:
////                    // user accepted calibration results
////                    response = commHandler.sendCommandGetAnyResponseEx(
////                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK));

////                    if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::ACK))
////                    {
////                        calibrationSettings = commHandler.receiveCalibarionSettingsEx();
////                        uavMonitor->notifyEvent(new IUavMonitor::Event(
////                            IUavMonitor::CALIBRATION_SETTINGS_CHANGED, IUavMonitor::DATA_CHANGED, ""));
////                    }
////                    else if (response == SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL))
////                    {
////                        uavMonitor->notifyEvent(new IUavMonitor::Event(
////                            IUavMonitor::ERROR_MESSAGE, IUavMonitor::DUMMY,
////                            "Error during ending radio calibration!\n"
////                            "Failed to safe data in memmory."));
////                    }
////                    else
////                    {
////                        __RL_EXCEPTION__("Error during ending radio calibration!\nBad board response.");
////                    }

////                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
////                        BREAK, SignalData::BREAK_ACK));

////                    externalEvent = ALLOW;
////                    return true;

////                case BREAK:
////                    // user rejected calibration results
////                    commHandler.sendCommandGetResponseEx(
////                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_FAIL),
////                        SignalData(SignalData::CALIBRATE_RADIO, SignalData::BREAK_ACK));

////                    uavMonitor->notifyEvent(new IUavMonitor::EventCalibrateRadio(
////                        IUavMonitor::CALIBRATE_RADIO, IUavMonitor::OTHER, "", ControlData(),
////                        BREAK, SignalData::BREAK_ACK));

////                    externalEvent = ALLOW;
////                    return true;

////                default:
////                    __RL_EXCEPTION__("Error during radio check ending!\nBad user command.");
////                }
////            }

////            default:
////                __RL_EXCEPTION__("Error during handling event!\nBad event received at radio calibration.");
////            }
////            externalEvent = ALLOW;
////        }
////        Sleep(50);
////    }
////}

//bool UavManager::checkRadioReceiver(void)
//{
//    const SignalData::Parameter response = commHandler->sendCommandGetAnyParameterEx(
//        SignalData(SignalData::CHECK_RADIO, SignalData::START), SignalData::CHECK_RADIO);

//    if (SignalData::ACK == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CHECK_RADIO_STARTED));
//    }
//    else if (SignalData::NOT_ALLOWED == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::INFO_MESSAGE,
//            "Error during radio check startup!\n"
//            "No radio receiver connected or bad control settings stored in memmory"));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during radio check startup!\nBad board response.");
//    }

//    const ControlSettings boardControlSettings = commHandler->receiveControlSettingsEx();
//    uavMonitor->notifyDataReceived(&boardControlSettings);

//    ControlData radioControlData;
//    while (!(externalEvent == ExternalEvent::STOP_APPLICATION_LOOP
//        || externalEvent == ExternalEvent::STOP_RADIO_CHECK))
//    {
//        const IMessage::PreambleType type = commHandler->proceedReceiving();
//        if (type == IMessage::CONTROL)
//        {
//            radioControlData = commHandler->getControlData();
//            uavMonitor->notifyDataReceived(&radioControlData);
//        }
//        commHandler->holdThread(1);
//    }

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::CHECK_RADIO_ENDED));

//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::CHECK_RADIO, SignalData::BREAK),
//        SignalData(SignalData::CHECK_RADIO, SignalData::BREAK_ACK));

//    return true;
//}

//bool UavManager::softwareUpgrade(void)
//{
//    const SignalData::Parameter response = commHandler->sendCommandGetAnyParameterEx(
//        SignalData(SignalData::SOFTWARE_UPGRADE, SignalData::START), SignalData::SOFTWARE_UPGRADE);

//    if (SignalData::ACK == response)
//    {
//        // connection is broken now
//        inApplicationLoop = false;

//        // start programming over DFU sequence
//        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::UPGRADE_STARTED));

//        uavMonitor->notifyUavEvent(new UavEvent(UavEvent::APPLICATION_LOOP_TERMINATED));

//        // application thread is ended now
//        return false;
//    }
//    else if (SignalData::NOT_ALLOWED == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::INFO_MESSAGE,
//            "Upgrade via radio connection is forbiden, aborting!"));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during software update startup!\nBad board response.");
//    }
//}

//bool UavManager::reset(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::SYSTEM_RESET, SignalData::START),
//        SignalData(SignalData::SYSTEM_RESET, SignalData::ACK));

//    // connection is broken now
//    inApplicationLoop = false;

//    uavMonitor->notifyUavEvent(new UavEventMessage(
//        UavEvent::INFO_MESSAGE,
//        "Connection closed after reset."));

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::APPLICATION_LOOP_TERMINATED));

//    // application thread is ended now
//    return false;
//}

//bool UavManager::uploadRoute(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::UPLOAD_ROUTE, SignalData::START),
//        SignalData(SignalData::UPLOAD_ROUTE, SignalData::ACK));

//    commHandler->sendDataProcedureEx(*containerToSend);
//    uavMonitor->notifyUavEvent(new UavEventUploaded(UavEvent::ROUTE_UPLOADED,
//                                                    *containerToSend));
//    return true;
//}

//bool UavManager::downloadRoute(void)
//{
//    const SignalData::Parameter response = commHandler->sendCommandGetAnyParameterEx(
//        SignalData(SignalData::DOWNLOAD_ROUTE, SignalData::START), SignalData::DOWNLOAD_ROUTE);

//    if (SignalData::ACK == response)
//    {
//        RouteContainer routeContainer =
//                commHandler->receiveRouteContainerEx();
//        uavMonitor->notifyDataReceived(&routeContainer);
//        return true;
//    }
//    else if (SignalData::FAIL == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::INFO_MESSAGE,
//            "Error during downloading data from board, bad data safed in memory."));
//        return true;
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during downloading data from board!\nBad board response.");
//    }
//}

//bool UavManager::sensorsLogger(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::SENSORS_LOGGER, SignalData::START),
//        SignalData(SignalData::SENSORS_LOGGER, SignalData::ACK));

//    uavMonitor->notifyUavEvent(new UavEvent(UavEvent::SENSORS_LOGGER_STARTED));

//    unsigned loopCounter = 0;

//    SensorsData sensorsData;

//    while (!(externalEvent == ExternalEvent::STOP_APPLICATION_LOOP
//        || externalEvent == ExternalEvent::STOP_SENSORS_LOGGER))
//    {
//        const IMessage::PreambleType type = commHandler->proceedReceiving();
//        if (type == IMessage::CONTROL)
//        {
//            sensorsData = commHandler->getSensorsData();
//            uavMonitor->notifyDataReceived(&sensorsData);
//        }

//        loopCounter++;

//        commHandler->holdThread(1);
//    }

//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::SENSORS_LOGGER, SignalData::BREAK),
//        SignalData(SignalData::SENSORS_LOGGER, SignalData::BREAK_ACK));

//    uavMonitor->notifyUavEvent(new UavEventMessage(
//                                   UavEvent::SENSORS_LOGGER_ENDED,
//                                   "Sensors logger ended."));

//    return true;
//}

//bool UavManager::configureWifi(void)
//{
//    commHandler->sendCommandGetResponseEx(
//        SignalData(SignalData::CONFIGURE_WIFI, SignalData::START),
//        SignalData(SignalData::CONFIGURE_WIFI, SignalData::ACK));

//    commHandler->sendDataProcedureEx(*containerToSend);

//    // wifi configuration can take seriously long time on board, 15s assumed
//    const SignalData response = commHandler->waitForAnyCommandEx(15000);
//    if (SignalData(SignalData::CONFIGURE_WIFI, SignalData::DONE) == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::WIFI_CONFIG_ENDED,
//                                                    "Wifi configuration done."));
//    }
//    else if (SignalData(SignalData::CONFIGURE_WIFI, SignalData::FAIL) == response)
//    {
//        uavMonitor->notifyUavEvent(new UavEventMessage(UavEvent::WIFI_CONFIG_ENDED,
//                                                    "Wifi not configured, module error."));
//    }
//    else
//    {
//        __RL_EXCEPTION__("Error during wifi configuration!\nBad board response.");
//    }

//    // delete right now due to password safty
//    delete containerToSend;

//    return true;
//}

//std::string UavManager::externalSensorsLogger(void)
//{
//    //sensorsLogger(EXTERNAL_TRUE);
//    return "dummy msg";
//}

//void UavManager::triggerPing(void)
//{
//    sentPingValue = std::rand();
//    sentPingTime = clock();
//    commHandler->send(SignalData(SignalData::PING_VALUE, sentPingValue));
//}

//void UavManager::handlePong(const SignalData& signalData) const
//{
//    if (signalData.getParameterValue() == sentPingValue)
//    {
//        uavMonitor->notifyPingUpdated(
//                    (long)(((clock() - sentPingTime) / 2.0f) + 0.5f));
//    }
//}

//void UavManager::exceptNotConnected(void) const
//{
//    __RL_EXCEPTION__("Not connected to board!");
//}

//void UavManager::exceptAlreadyPreforming(void) const
//{
//    const std::string msg = "Error, already preforming: "
//            + SignalData::toString(SignalData::Command(ongoingAction));
//    __RL_EXCEPTION__(msg.c_str());
//}
