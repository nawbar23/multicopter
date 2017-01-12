package com.multicopter.java;

import com.multicopter.java.actions.CommHandlerAction;
import com.multicopter.java.data.*;
import com.multicopter.java.events.UserEvent;

import java.util.ArrayList;

/**
 * Created by ebarnaw on 2016-10-14.
 * Purpose of this class is to reflect data state of controller board and provides high level API
 * for controlling the connected UAV. By this class user can start application loops and any action
 * over application loop, including flight loop. Also this class posts events from UAV as UavEvents
 * to all listeners.
 */
public class UavManager {

    private ArrayList<UavManagerListener> listeners;

    // data received form board
    private DebugData debugData;
    private AutopilotData autopilotData;

    // settings received form board at startup connection and after calibrations
    private CalibrationSettings calibrationSettings;

    // actual communication delay
    private long commDelay;

    // main communication handler
    private CommHandler commHandler;

    private ControlDataSource controlDataSource;

    public UavManager(CommInterface commInterface) {
        this.listeners = new ArrayList<>();
        this.commDelay = 0;
        this.commHandler = new CommHandler(this, commInterface);
    }

    public CommHandler getCommHandler() {
        return commHandler;
    }

    public void setControlDataSource(ControlDataSource controlDataSource) {
        this.controlDataSource = controlDataSource;
    }

    public ControlData getCurrentControlData() {
        return controlDataSource.getControlData();
    }

    /**
     * =============================================================================================
     * Here starts communication actions control method
     * User can use them to start/end/process actions according to Multicopter Comm Protocol schema
     * Effects of running actions will be posted as UavEvents to registered listeners
     * =============================================================================================
     */

    /**
     * Sends command to leave application loop and finally end connection
     */
    public void disconnectApplicationLoop() {
        preformAction(CommHandlerAction.ActionType.DISCONNECT);
    }

    /**
     * startFlightLoop
     */
    public void startFlightLoop() {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformAction(CommHandlerAction.ActionType.FLIGHT_LOOP);
        }
    }

    /**
     * endFlightLoop
     */
    public void endFlightLoop() throws Exception {
        commHandler.notifyUserEvent(new UserEvent(UserEvent.Type.END_FLIGHT_LOOP));
    }

    /**
     * startAccelerometerCalibration
     */
    public void startAccelerometerCalibration() {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformAction(CommHandlerAction.ActionType.ACCELEROMETER_CALIBRATION);
        }
    }

    /**
     * startMagnetometerCalibration
     */
    public void startMagnetometerCalibration() {
        notifyUavEvent(new UavEvent(UavEvent.Type.MAGENTOMETER_CALIBRATION_STARTED));
        // TODO uncomment this after dialog tests, begin of magnetometer comm procedure
//        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
//            preformAction(CommHandlerAction.ActionType.CALIBRATE_MAGNETOMETER);
//        }
    }

    /**
     * doneMagnetometerCalibration
     */
    public void doneMagnetometerCalibration() throws Exception {
        commHandler.notifyUserEvent(new UserEvent(UserEvent.Type.DONE_MAGNETOMETER_CALIBRATION));
    }

    /**
     * cancelMagnetometerCalibration
     */
    public void cancelMagnetometerCalibration() throws Exception {
        commHandler.notifyUserEvent(new UserEvent(UserEvent.Type.CANCEL_MAGNETOMETER_CALIBRATION));
    }

    /**
     * uploadControlSettings
     */
    public void uploadControlSettings(ControlSettings controlSettings) {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformActionUpload(CommHandlerAction.ActionType.UPLOAD_CONTROL_SETTINGS, controlSettings);
        }
    }

    /**
     * downloadControlSettings
     */
    public void downloadControlSettings() {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformAction(CommHandlerAction.ActionType.DOWNLOAD_CONTROL_SETTINGS);
        }
    }

    /**
     * uploadRouteContainer
     */
    public void uploadRouteContainer(RouteContainer routeContainer) {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformActionUpload(CommHandlerAction.ActionType.UPLOAD_ROUTE_CONTAINER, routeContainer);
        }
    }

    /**
     * downloadRouteContainer
     */
    public void downloadRouteContainer() {
        if (commHandler.getCommActionType() == CommHandlerAction.ActionType.APPLICATION_LOOP) {
            preformAction(CommHandlerAction.ActionType.DOWNLOAD_ROUTE_CONTAINER);
        }
    }

    /**
     * notifyAutopilotEvent
     */
    public void notifyAutopilotEvent(AutopilotData autopilotData) {
        // TODO implement this feature
    }

    private void preformAction(CommHandlerAction.ActionType actionType) {
        try {
            commHandler.preformAction(actionType);
        } catch (Exception e) {
            System.out.println("UavManager exception: " + e.toString());
            notifyUavEvent(new UavEvent(UavEvent.Type.ERROR, e.getMessage()));
        }
    }

    private void preformActionUpload(CommHandlerAction.ActionType actionType, SignalPayloadData dataToUpload) {
        try {
            commHandler.preformActionUpload(actionType, dataToUpload);
        } catch (Exception e) {
            System.out.println("UavManager exception: " + e.toString());
            notifyUavEvent(new UavEvent(UavEvent.Type.ERROR, e.getMessage()));
        }
    }

    public void notifyUavEvent(UavEvent event) {
        try {
            updateCommState(event);
        }
        catch (Exception e) {
            System.out.println("UavManager update comm state error: " + e.getMessage());
        }
        for (UavManagerListener listener : listeners) {
            listener.handleUavEvent(event, this);
        }
    }

    private void updateCommState(UavEvent event) throws Exception {
        switch (event.getType()) {
            case CONNECTED:
                commHandler.preformAction(CommHandlerAction.ActionType.APPLICATION_LOOP);
                break;

            case DISCONNECTED:
                commHandler.disconnectSocket();
                break;
        }
    }

    public void registerListener(UavManagerListener listener) {
        listeners.add(listener);
    }

    public void unregisterListener(UavManagerListener listener) {
        listeners.remove(listener);
    }

    public interface UavManagerListener {
        void handleUavEvent(UavEvent event, UavManager uavManager);
    }

    public DebugData getDebugData() {
        return debugData;
    }

    public void setDebugData(DebugData debugData) {
        this.debugData = debugData;
        notifyUavEvent(new UavEvent(UavEvent.Type.DEBUG_UPDATED));
    }

    public CalibrationSettings getCalibrationSettings() {
        return calibrationSettings;
    }

    public void setCalibrationSettings(CalibrationSettings calibrationSettings) {
        this.calibrationSettings = calibrationSettings;
        notifyUavEvent(new UavEvent(UavEvent.Type.CALIBRATION_UPDATED));
    }

    public long getCommDelay() {
        return commDelay;
    }

    public void setCommDelay(long commDelay) {
        System.out.println("Ping delay updated: " + String.valueOf(commDelay) + " ms");
        this.commDelay = commDelay;
        notifyUavEvent(new UavEvent(UavEvent.Type.PING_UPDATED));
    }

    public AutopilotData getAutopilotData() {
        return autopilotData;
    }

    public interface ControlDataSource {
        ControlData getControlData();
    }
}
