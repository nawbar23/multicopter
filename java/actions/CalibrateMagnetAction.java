package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.events.CommEvent;
import com.multicopter.java.events.UserEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class CalibrateMagnetAction extends CommHandlerAction {

    public enum CalibrationState {
        IDLE,

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
        // TODO send initial command - SignalData(???ACTION???, START)
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        // TODO handle communication events and proceed state machine
    }

    @Override
    public void notifyUserEvent(UserEvent userEvent) {
        // TODO handle DONE or CANCEL user event
    }

    @Override
    public ActionType getActionType() {
        return ActionType.MAGNETOMETER_CALIBRATION;
    }
}
