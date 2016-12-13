package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.events.CommEvent;

import static com.multicopter.java.actions.CommHandlerAction.ActionType.IDLE;

/**
 * Created by NawBar on 2016-10-12.
 */
public class IdleAction extends CommHandlerAction {

    public IdleAction(CommHandler commHandler){
        super(commHandler);
    }

    @Override
    public boolean isActionDone() {
        return true;
    }

    @Override
    public void start() {
        System.out.println("Starting IDLE action - no action");
    }

    @Override
    public void handleEvent(CommEvent event) {
        System.out.println("Idle action drops event");
    }

    @Override
    public ActionType getActionType() {
        return IDLE;
    }
}
