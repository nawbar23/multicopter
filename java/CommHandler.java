package com.multicopter.java;

import com.multicopter.java.actions.*;
import com.multicopter.java.data.ControlData;
import com.multicopter.java.data.SignalData;
import com.multicopter.java.data.SignalPayloadData;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.MessageEvent;
import com.multicopter.java.events.UserEvent;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by NawBar on 2016-10-12.
 */
public class CommHandler implements CommInterface.CommInterfaceListener,
        CommDispatcher.CommDispatcherListener {

    private CommHandlerAction commHandlerAction;
    private CommInterface commInterface;
    private CommDispatcher dispatcher;

    private UavManager uavManager;

    private List<CommTask> runningTasks;

    private CommTask controlTask;
    private CommTask pingTask;

    public CommHandler(UavManager uavManager, CommInterface commInterface, double controlFreq, double pingFreq) {
        this.commHandlerAction = new IdleAction(this);
        this.commInterface = commInterface;
        this.commInterface.setListener(this);
        this.dispatcher = new CommDispatcher(this);

        this.uavManager = uavManager;

        this.runningTasks = new ArrayList<>();

        controlTask = new CommTask(controlFreq) {
            @Override
            protected String getTaskName() {
                return "control_task";
            }

            @Override
            protected void task() {
                ControlData controlData = getUavManager().getCurrentControlData();
                if (((FlightLoopAction)commHandlerAction).isBreaking()) {
                    controlData.setStopCommand();
                }
                System.out.println("Controlling: " + controlData.toString());
                send(controlData.getMessage());
            }
        };

        pingTask = new CommTask(pingFreq) {
            @Override
            protected String getTaskName() {
                return "ping_task";
            }

            @Override
            protected void task() {
                System.out.println("CommHandler: Pinging...");
                switch (state) {
                    case CONFIRMED:
                        sentPing = new SignalData(SignalData.Command.PING_VALUE, (int) (Math.random() * 1000000000));
                        send(sentPing.getMessage());
                        timestamp = System.currentTimeMillis();
                        break;

                    case WAITING:
                        System.out.println("CommHandler: Ping receiving timeout");
                        state = PingTaskState.CONFIRMED;
                        break;
                }
            }
        };
    }

    public void connectSocket(String ipAddress, int port) {
        System.out.println("CommHandler: connectSocket");
        commInterface.connect(ipAddress, port);
    }

    void disconnectSocket() {
        System.out.println("CommHandler: disconnectSocket");
        stopAllTasks();
        commInterface.disconnect();
    }

    void preformAction(CommHandlerAction.ActionType actionType) throws Exception {
        if (commHandlerAction.isActionDone()) {
            commHandlerAction = actionFactory(actionType, null);
            commHandlerAction.start();
        } else {
            throw new Exception("CommHandler: Previous action not ready at state: " + commHandlerAction.getActionName() + ", aborting...");
        }
    }

    void preformActionUpload(CommHandlerAction.ActionType actionType, SignalPayloadData dataToUpload) throws Exception {
        if (commHandlerAction.isActionDone()) {
            commHandlerAction = actionFactory(actionType, dataToUpload);
            commHandlerAction.start();
        } else {
            throw new Exception("CommHandler: Previous action not ready at state: " + commHandlerAction.getActionName() + ", aborting...");
        }
    }

    void notifyUserEvent(UserEvent userEvent) throws Exception {
        if (commHandlerAction.getActionType() == userEvent.getOwnerAction()) {
            commHandlerAction.notifyUserEvent(userEvent);
        } else {
            throw new Exception("Owner of user event is different than ongoing action, could not handel it");
        }
    }

    @Override
    public void handleCommEvent(CommEvent event) {
        System.out.println("CommHandler: Event " + event.toString() + " received at action " + commHandlerAction.toString());
        switch (event.getType()) {
            case MESSAGE_RECEIVED:
                if (((MessageEvent) event).getMessageType() == CommMessage.MessageType.SIGNAL) {
                    SignalData signalData = new SignalData(((MessageEvent) event).getMessage());
                    if (signalData.getCommand() == SignalData.Command.PING_VALUE) {
                        uavManager.setCommDelay(handlePongReception(signalData));
                        return;
                    }
                }
                break;
        }

        try {
            commHandlerAction.handleEvent(event);
        } catch (Exception e) {
            System.out.println(e.toString());
        }
    }

    public UavManager getUavManager() {
        return uavManager;
    }

    public CommHandlerAction.ActionType getCommActionType() {
        return commHandlerAction.getActionType();
    }

    public void send(CommMessage message) {
        System.out.println("CommHandler: Sending message: " + message.toString());
        commInterface.send(message.getByteArray());
    }

    public void notifyActionDone() {
        System.out.println("CommHandler: notifyActionDone");
        try {
            preformAction(CommHandlerAction.ActionType.APPLICATION_LOOP);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private CommHandlerAction actionFactory(CommHandlerAction.ActionType actionType, SignalPayloadData data) throws Exception {
        switch (actionType) {
            case IDLE:
                return new IdleAction(this);
            case CONNECT:
                return new ConnectAction(this);
            case DISCONNECT:
                return new DisconnectAction(this);
            case APPLICATION_LOOP:
                return new AppLoopAction(this);
            case FLIGHT_LOOP:
                return new FlightLoopAction(this);
            case ACCELEROMETER_CALIBRATION:
                return new CalibrateAccelAction(this);
            case CALIBRATE_MAGNETOMETER:
                return new CalibrateMagnetAction(this);

            default:
                throw new Exception("CommHandler: Unsupported action type");
        }
    }

    @Override
    public void onDataReceived(final byte[] data, final int dataSize) {
        dispatcher.proceedReceiving(data, dataSize);
    }

    @Override
    public void onError(IOException e) {
        System.out.println("CommHandler: onError: " + e.getMessage());
        uavManager.notifyUavEvent(new UavEvent(UavEvent.Type.ERROR, e.getMessage()));
        uavManager.notifyUavEvent(new UavEvent(UavEvent.Type.DISCONNECTED));
    }

    @Override
    public void onDisconnected() {
        System.out.println("CommHandler: onDisconnected");
        commHandlerAction = new IdleAction(this);
    }

    @Override
    public void onConnected() {
        System.out.println("CommHandler: onConnected");
        try {
            preformAction(CommHandlerAction.ActionType.CONNECT);
        } catch (Exception e) {
            e.printStackTrace();
            uavManager.notifyUavEvent(new UavEvent(UavEvent.Type.ERROR, e.getMessage()));
        }
    }

    private SignalData sentPing;
    private long timestamp;

    private PingTaskState state = PingTaskState.CONFIRMED;

    private long handlePongReception(final SignalData pingPongMessage) {
        if (pingPongMessage.getParameterValue() == sentPing.getParameterValue()) {
            // valid ping measurement, compute ping time
            state = PingTaskState.CONFIRMED;
            return (System.currentTimeMillis() - timestamp) / 2;
        } else {
            System.out.println("CommHandler: Pong key does not match to the ping key!");
            return 0;
        }
    }

    private enum PingTaskState {
        WAITING,
        CONFIRMED
    }

    public CommTask getPingTask() {
        return pingTask;
    }

    public void startCommTask(CommTask task) {
        task.start();
        runningTasks.add(task);
    }

    public void stopCommTask(CommTask task) {
        task.stop();
        runningTasks.remove(task);
    }

    private void stopAllTasks() {
        System.out.println("CommHandler: stopAllTasks");
        for (CommTask task : runningTasks) {
            stopCommTask(task);
        }
    }

    public CommTask getControlTask() {
        return controlTask;
    }
}
