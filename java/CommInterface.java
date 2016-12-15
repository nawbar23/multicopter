package com.multicopter.java;

import java.io.IOException;

/**
 * Created by ebarnaw on 2016-11-02.
 */

public abstract class CommInterface {

    protected CommInterfaceListener listener;

    void setListener(CommInterfaceListener listener) {
        this.listener = listener;
    }

    public abstract void connect(String ipAddress, int port);

    public abstract void disconnect();

    public abstract void send(final byte[] data);

    public interface CommInterfaceListener
    {
        void onConnected();

        void onDisconnected();

        void onError(IOException e);

        void onDataReceived(final byte[] data);
    }
}