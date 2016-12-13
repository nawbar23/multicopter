package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.CommMessage;
import com.multicopter.java.data.DebugData;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.MessageEvent;

/**
 * Created by ebarnaw on 2016-10-14.
 */
public class AppLoopAction extends CommHandlerAction {

    public AppLoopAction(CommHandler commHandler){
        super(commHandler);
    }

    @Override
    public ActionType getActionType() {
        return ActionType.APPLICATION_LOOP;
    }

    @Override
    public boolean isActionDone() {
        return true;
    }

    @Override
    public void start() {
        System.out.println("AppLoopAction: Starting app loop handling mode");
        commHandler.startCommTask(commHandler.getPingTask());
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        if (event.getType() == CommEvent.EventType.MESSAGE_RECEIVED) {
            final MessageEvent messageEvent = ((MessageEvent)event);

            if (messageEvent.getMessageType() == CommMessage.MessageType.CONTROL) {
                // debug data received
                commHandler.getUavManager().setDebugData(new DebugData(messageEvent.getMessage()));
            } else {
                System.out.println("AppLoopAction: Unexpected massage received: " + messageEvent.toString());
            }
        }
    }
}
