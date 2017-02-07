package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.CommMessage;
import com.multicopter.java.UavEvent;
import com.multicopter.java.data.DebugData;
import com.multicopter.java.data.RouteContainer;
import com.multicopter.java.data.SignalData;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.MessageEvent;
import com.multicopter.java.events.SignalPayloadEvent;

/**
 * Created by nawba on 12.01.2017.
 */

public class UploadRouteContainerAction extends CommHandlerAction {

    public enum UploadState {
        IDLE,
        INITIAL_COMMAND,
        UPLOADING_DATA

    }

    private UploadState state;

    private RouteContainer routeContainerToUpload;

    public UploadRouteContainerAction(CommHandler commHandler, RouteContainer routeContainerToUpload) {
        super(commHandler);
        this.state = UploadState.IDLE;
        this.routeContainerToUpload = routeContainerToUpload;
    }

    @Override
    public void start() {
        state = UploadState.INITIAL_COMMAND;
        commHandler.stopCommTask(commHandler.getPingTask());
        commHandler.send(new SignalData(SignalData.Command.UPLOAD_ROUTE, SignalData.Parameter.START).getMessage());

    }

    @Override
    public boolean isActionDone() {
        return false;
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        UploadState actualState = state;
        switch (state) {
            case INITIAL_COMMAND:
                if (event.getType() == CommEvent.EventType.MESSAGE_RECEIVED) {
                    switch (((MessageEvent) event).getMessageType()) {
                        case CONTROL:
                            System.out.println("DebugData received when waiting for ACK on RouteContainer upload procedure");
                            commHandler.getUavManager().setDebugData(new DebugData(((MessageEvent) event).getMessage()));
                            break;
                        case SIGNAL:
                            if (event.matchSignalData(new SignalData(SignalData.Command.UPLOAD_ROUTE, SignalData.Parameter.ACK))) {
                                System.out.println("Starting Route Container upload procedure");
                                state = UploadState.UPLOADING_DATA;
                            } else {
                                System.out.println("Unexpected event received at state " + state.toString());
                            }
                            break;
                    }
                }
                break;
            case UPLOADING_DATA:
                if (event.getType() == CommEvent.EventType.SIGNAL_PAYLOAD_RECEIVED
                        && ((SignalPayloadEvent)event).getDataType() == SignalData.Command.ROUTE_CONTAINER_DATA) {
                    if (routeContainerToUpload.isValid()) {
                        System.out.println("Route Container settings uploaded");
                        commHandler.send(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.ACK).getMessage());
                        commHandler.send(routeContainerToUpload);
                        commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Route Container settings uploaded successfully!"));
                        commHandler.notifyActionDone();
                    } else {
                        commHandler.getUavManager().notifyUavEvent(new UavEvent(UavEvent.Type.MESSAGE, "Uploading Route Container settings failed!"));
                        System.out.println("Route Container settings sent but the data is invalid, responding with DATA_INVALID");
                        commHandler.send(new SignalData(SignalData.Command.UPLOAD_SETTINGS, SignalData.Parameter.DATA_INVALID).getMessage());
                        commHandler.notifyActionDone();
                    }
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
        return ActionType.UPLOAD_ROUTE_CONTAINER;
    }
}
