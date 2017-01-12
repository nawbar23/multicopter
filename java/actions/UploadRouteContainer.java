package com.multicopter.java.actions;

import com.multicopter.java.CommHandler;
import com.multicopter.java.data.RouteContainer;
import com.multicopter.java.events.CommEvent;

/**
 * Created by nawba on 12.01.2017.
 */

public class UploadRouteContainer extends CommHandlerAction {

    public enum UploadState {
        IDLE,

    }

    private UploadState state;

    private RouteContainer routeContainerToUpload;

    public UploadRouteContainer(CommHandler commHandler, RouteContainer routeContainerToUpload) {
        super(commHandler);
        this.state = UploadState.IDLE;
        this.routeContainerToUpload = routeContainerToUpload;
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
        return ActionType.UPLOAD_ROUTE_CONTAINER;
    }
}
