package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.data.SignalData;
import com.multicopter.java.events.CommEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class DownloadControlSettingsAction extends CommHandlerAction {

    private enum DownloadState {
        IDLE,
        INITIAL_COMAND,
        WAITING_FOR_SETTINGS,
        WAITING_FOR_SETTINGS_DATA
    }

    private DownloadState state;

    private boolean downloadProcedureDone;

    public DownloadControlSettingsAction(CommHandler commHandler) {
        super(commHandler);
        this.state = DownloadState.IDLE;

    }

    @Override
    public void start() {
        // TODO send initial command - SignalData(???ACTION???, START)
        System.out.println("Starting download control settings procedre");
        state=DownloadState.INITIAL_COMAND;
        commHandler.stopCommTask(commHandler.getPingTask());
        commHandler.send(new SignalData(SignalData.Command.DOWNLOAD_SETTINGS, SignalData.Parameter.START).getMessage());
    }

    @Override
    public boolean isActionDone() {
        return false;
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {
        // TODO handle communication events and proceed state machine
    }

    @Override
    public ActionType getActionType() {
        return ActionType.DOWNLOAD_CONTROL_SETTINGS;
    }
}
