package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.data.ControlSettings;
import com.multicopter.java.events.CommEvent;

/**
 * Created by nawbar on 12.01.2017.
 */

public class UploadControlSettingsAction extends CommHandlerAction {

    public enum UploadState {
        IDLE,

    }

    private UploadState state;

    private ControlSettings controlSettingsToUpload;

    public UploadControlSettingsAction(CommHandler commHandler, ControlSettings controlSettingsToUpload) {
        super(commHandler);
        this.state = UploadState.IDLE;
        this.controlSettingsToUpload = controlSettingsToUpload;
    }

    @Override
    public void start() {

    }

    @Override
    public boolean isActionDone() {
        return false;
    }

    @Override
    public void handleEvent(CommEvent event) throws Exception {

    }

    @Override
    public ActionType getActionType() {
        return ActionType.UPLOAD_CONTROL_SETTINGS;
    }
}
