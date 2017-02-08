package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.UavEvent;
import com.multicopter.java.data.ControlSettings;
import com.multicopter.java.data.DebugData;
import com.multicopter.java.data.SignalData;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.MessageEvent;
import com.multicopter.java.events.SignalPayloadEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class UploadControlSettingsAction extends CommHandlerAction {

    public enum UploadState {
        IDLE,
        INITIAL_COMAND,
        WAITING_FOR_UPLOAD_DATA,
        WAITING_FOR_ACK

    }

    private UploadState state;

    private boolean uploadProcedureDone;

    private ControlSettings controlSettingsToUpload;

    public UploadControlSettingsAction(CommHandler commHandler, ControlSettings controlSettingsToUpload) {
        super(commHandler);
        this.state = UploadState.IDLE;
        this.controlSettingsToUpload = controlSettingsToUpload;
        uploadProcedureDone = false;
    }

    @Override
    public void start() {
        state = UploadState.INITIAL_COMAND;
        commHandler.stopCommTask(commHandler.getPingTask());
        commHandler.send(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.START).getMessage());

    }

    @Override
    public boolean isActionDone() {
        return uploadProcedureDone;
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        UploadState actualState = state;
        switch (state) {
            case INITIAL_COMAND:
                if (event.getType() == CommEvent.EventType.MESSAGE_RECEIVED) {
                    switch (((MessageEvent) event).getMessageType()) {
                        case CONTROL:
                            System.out.println("DebugData received when waiting for ACK on initial download control settings command");
                            commHandler.getUavManager().setDebugData(new DebugData(((MessageEvent) event).getMessage()));
                            break;

                        case SIGNAL:
                            if (event.matchSignalData(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.ACK))) {
                                System.out.println("Uploading control settings starts");
                                state = UploadState.WAITING_FOR_UPLOAD_DATA;
                            } else {
                                System.out.println("Unexpected event received at state " + state.toString());
                            }
                            break;
                    }
                }
                break;

            case WAITING_FOR_UPLOAD_DATA:
                if (event.getType() == CommEvent.EventType.SIGNAL_PAYLOAD_RECEIVED
                        && ((SignalPayloadEvent) event).getDataType() == SignalData.Command.CONTROL_SETTINGS_DATA) {

                    if (controlSettingsToUpload.isValid()) {
                        commHandler.send(controlSettingsToUpload);
                        state = UploadState.WAITING_FOR_ACK;
                    } else {
                        commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Uploading control settings failed!"));
                        System.out.println("Control settings can not be sent, because data is invalid");
                        commHandler.send(new SignalData(SignalData.Command.CONTROL_SETTINGS, SignalData.Parameter.DATA_INVALID).getMessage()); //??
                    }
                } else {
                    System.out.println("Unexpected event received at state " + state.toString());
                }
                break;

            case WAITING_FOR_ACK:
                if (event.matchSignalData(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.ACK))) {
                    System.out.println("Control settings uploaded");
                    commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Control settings uploaded successfully!"));
                    uploadProcedureDone = true;
                    commHandler.notifyActionDone();
                } else if (event.matchSignalData(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.DATA_INVALID))) {
                    commHandler.send(controlSettingsToUpload);
                } else {
                    System.out.println("Unexpected event received at state " + state.toString());
                }
                break;

            default:
                throw new Exception("Event: " + event.toString() + " received at unknown state");
        }
        if (actualState != state) {
            System.out.println("HandleEvent done, transition: " + actualState.toString() + " -> " + state.toString());
        } else {
            System.out.println("HandleEvent done, no state change");
        }

    }

    @Override
    public ActionType getActionType() {
        return ActionType.UPLOAD_CONTROL_SETTINGS;
    }
}
