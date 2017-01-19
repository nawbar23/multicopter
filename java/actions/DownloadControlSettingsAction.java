package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.events.CommEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class DownloadControlSettingsAction extends CommHandlerAction {

    private enum DownloadState {
        IDLE,
    }

    private DownloadState state;

    public DownloadControlSettingsAction(CommHandler commHandler) {
        super(commHandler);
        this.state = DownloadState.IDLE;
    }

    @Override
    public void start() {
        // TODO send initial command - SignalData(???ACTION???, START)
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
