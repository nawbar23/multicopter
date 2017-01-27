package com.multicopter.java.simulator;


import com.addrone.AdapterMain;
import com.addrone.ServerMain;

import com.multicopter.java.CommInterface;
import com.multicopter.java.CommMessage;



import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;

/**
 * Created by ebarnaw on 2017-01-03.
 */
public class TcpPeer extends CommInterface implements Runnable  {

    //tylko do dzialania adaptera, pozniej zostanie tylko serwer.
    private AdapterMain adapterMain;
    private ServerMain serverMain;

    private ExecutorService executorService;

    private Socket socket;
    private ServerSocket serverSocket;

    private DataOutputStream outputStream;

    private int port;

    private State state;

    private boolean adapterMode;

    public TcpPeer(ExecutorService executorService, ServerMain serverMain) {
        super();
        this.serverMain = serverMain;
        this.executorService = executorService;
        this.state = State.DISCONNECTED;
    }

    public TcpPeer(ExecutorService executorService, boolean adapterMode, AdapterMain adapterMain){
        this.adapterMain = adapterMain;
        this.executorService = executorService;
        this.state = State.DISCONNECTED;
        this.adapterMode = adapterMode;
    }


    private enum State {
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECTED
    }

    @Override
    public void connect(String ipAddress, int port) {
//        if (state != State.DISCONNECTED) {
//            throw new Exception("Can not connect when already in working state!");
//        }
        this.port = port;
        state = State.CONNECTING;
        executorService.execute(this);
    }

    @Override
    public void disconnect()  {
        state = State.DISCONNECTING;
    }

    @Override
    public void send(byte[] data) {
        try {
            System.out.println("Sending: 0x" + CommMessage.byteArrayToHexString(data));
            outputStream.write(data, 0, data.length);
        } catch (IOException e) {
            System.out.println("Error while sending: " + e.getMessage());
            disconnect();
        }
    }

    @Override
    public void run() {
        DataInputStream inputStream = null;
        try {
            if(adapterMode) {
                serverSocket = new ServerSocket(port);
                System.out.println("Server started, waiting for connection");
                socket = serverSocket.accept();
            }
            else{
                socket = new Socket("localhost", 6666);
            }

            outputStream = new DataOutputStream(socket.getOutputStream());
            inputStream = new DataInputStream(socket.getInputStream());

            System.out.println("Client connected, running receiving loop!");
            state = State.CONNECTED;
            listener.onConnected();

            byte buffer[] = new byte[1024];
            while(state != State.DISCONNECTING) {
                int len = inputStream.available();
                if (len > 1024) len = 1024;
                int dataSize = inputStream.read(buffer, 0, len);
                listener.onDataReceived(buffer, dataSize);
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
            listener.onError(e);
        }

        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        if(serverSocket != null) {
            try {
                serverSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
                System.out.println(e.getMessage());
            }
        }

        state = State.DISCONNECTED;
        listener.onDisconnected();
        if(serverMain == null){
            adapterMain.restartTcpPeer();
        }
        else{
            serverMain.restartTcpPeer();
        }
    }
}
