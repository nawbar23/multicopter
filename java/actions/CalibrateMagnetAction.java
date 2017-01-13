package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.CommMessage;
import com.multicopter.java.UavEvent;
import com.multicopter.java.data.CalibrationSettings;
import com.multicopter.java.data.DebugData;
import com.multicopter.java.data.SignalData;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.MessageEvent;
import com.multicopter.java.events.SignalPayloadEvent;
import com.multicopter.java.events.UserEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class CalibrateMagnetAction extends CommHandlerAction {

    public enum CalibrationState {
        IDLE,
        INITIAL_COMMAND,
        FINISH,
        TERMINATE,
        WAITING_FOR_CALIBRATION,
        WAITING_FOR_CALIBRATION_DATA

    }

    private CalibrationState state;

    private boolean calibrationProcedureDone;

    public CalibrateMagnetAction(CommHandler commHandler){
        super(commHandler);
        state = CalibrationState.IDLE;
        calibrationProcedureDone = false;
    }

    @Override
    public boolean isActionDone() {
        return calibrationProcedureDone;
    }

    @Override
    public void start() {
        System.out.println("Starting magnetometer calibration procedure");
        calibrationProcedureDone = false;
        state = CalibrationState.INITIAL_COMMAND;
        commHandler.stopCommTask(commHandler.getPingTask());
        commHandler.send(new SignalData(SignalData.Command.CALIBRATE_MAGNET, SignalData.Parameter.START).getMessage());
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        CalibrationState actualState = state;
        switch (state) {
            case INITIAL_COMMAND:
                if (event.getType() == CommEvent.EventType.MESSAGE_RECEIVED) {
                    switch (((MessageEvent) event).getMessageType()) {
                        case CONTROL:
                            System.out.println("DebugData received when waiting for ACK on initial calibrate magnetometer command");
                            commHandler.getUavManager().setDebugData(new DebugData(((MessageEvent) event).getMessage()));
                            break;

                        case SIGNAL:
                            if (event.matchSignalData(new SignalData(SignalData.Command.CALIBRATE_MAGNET, SignalData.Parameter.ACK))) {
                                System.out.println("Magnetometer calibration starts");
                                commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MAGENTOMETER_CALIBRATION_STARTED));
                                state = CalibrationState.WAITING_FOR_CALIBRATION;
                            } else {
                                System.out.println("Unexpected event received at state " + state.toString());
                            }
                            break;
                    }
                }
            case WAITING_FOR_CALIBRATION:
                if (event.getType() == CommEvent.EventType.MESSAGE_RECEIVED) {
                    CommMessage msg = ((MessageEvent) event).getMessage();
                    if (msg.getType() == CommMessage.MessageType.SIGNAL) {
                        SignalData command = new SignalData(msg);
                        if (command.getCommand() == SignalData.Command.CALIBRATE_MAGNET) {
                            if (command.getParameter() == SignalData.Parameter.SKIP || state == CalibrationState.TERMINATE) {
                                notifyUserEvent(new UserEvent(UserEvent.Type.CANCEL_MAGNETOMETER_CALIBRATION));
                            } else if(command.getParameter() == SignalData.Parameter.DONE || state == CalibrationState.FINISH){
                                notifyUserEvent(new UserEvent(UserEvent.Type.DONE_MAGNETOMETER_CALIBRATION));
                            } else{
                                System.out.println("Calibration failed");
                                commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Calibration failed"));
                                calibrationProcedureDone = true;
                                commHandler.notifyActionDone();

                            }
                        }
                    }
                }
                break;
            case TERMINATE:
                commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Terminating calibration!"));
                commHandler.send(new SignalData(SignalData.Command.CALIBRATE_MAGNET, SignalData.Parameter.ACK).getMessage());
                calibrationProcedureDone = true;
                commHandler.notifyActionDone();
                break;
            case FINISH:
                commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Finishing calibration!"));
                commHandler.send(new SignalData(SignalData.Command.CALIBRATE_MAGNET, SignalData.Parameter.DONE).getMessage());
                state = CalibrationState.WAITING_FOR_CALIBRATION_DATA;
                break;
            case WAITING_FOR_CALIBRATION_DATA:
                if (event.getType() == CommEvent.EventType.SIGNAL_PAYLOAD_RECEIVED
                        && ((SignalPayloadEvent)event).getDataType() == SignalData.Command.CALIBRATION_SETTINGS_DATA) {
                    SignalPayloadEvent signalEvent = (SignalPayloadEvent)event;

                    CalibrationSettings calibrationSettings = (CalibrationSettings)signalEvent.getData();
                    if (calibrationSettings.isValid()) {
                        System.out.println("Calibration settings received after magnetometer calibration");
                        state = CalibrationState.IDLE;
                        commHandler.send(new SignalData(SignalData.Command.CALIBRATION_SETTINGS, SignalData.Parameter.ACK).getMessage());
                        commHandler.getUavManager().setCalibrationSettings(calibrationSettings);
                        commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Magnetometer calibration successful"));
                        calibrationProcedureDone = true;
                        commHandler.notifyActionDone();
                    } else {
                        System.out.println("Calibration settings received but the data is invalid, responding with BAD_CRC");
                        commHandler.send(new SignalData(SignalData.Command.CALIBRATION_SETTINGS, SignalData.Parameter.BAD_CRC).getMessage());
                    }
                } else {
                    System.out.println("Unexpected event received at state " + state.toString());
                }
                break;
        }
        if (actualState != state) {
            System.out.println("HandleEvent done, transition: " + actualState.toString() + " -> " + state.toString());
        } else {
            System.out.println("HandleEvent done, no state change");
        }
    }

    @Override
    public void notifyUserEvent(UserEvent userEvent) {
        if (userEvent.getType() == UserEvent.Type.CANCEL_MAGNETOMETER_CALIBRATION) {
            state = CalibrationState.TERMINATE;
        } else if (userEvent.getType() == UserEvent.Type.DONE_MAGNETOMETER_CALIBRATION) {
            state = CalibrationState.FINISH;
        }
    }

    @Override
    public ActionType getActionType() {
        return ActionType.MAGNETOMETER_CALIBRATION;
    }
}
