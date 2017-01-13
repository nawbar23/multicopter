package com.multicopter.java.data;

import com.multicopter.java.CommMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by ebarnaw on 2016-10-14.
 */
public class RouteContainer implements SignalPayloadData {

    public class Waypoint {
        public double latitude, longintude;
        public float absoluteAltitude;
        public float relativeAltitude;
        public float velocity;

        public Waypoint() {
        }

        public Waypoint(final byte[] dataArray) {

            // TODO implement deserialization

        }

        public byte[] serialize() {
            byte[] dataArray = new byte[getDataArraySize()];
            ByteBuffer buffer = ByteBuffer.allocate(dataArray.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);

            // TODO implement serialization

            System.arraycopy(buffer.array(), 0, dataArray, 0, dataArray.length);
            return dataArray;
        }
    }

    private int routeSize;
    private float waypointTime; // [s]
    private float baseTime; // [s]

    private ArrayList<Waypoint> route;

    private int crcValue;

    public RouteContainer() {
        routeSize = 0;
        waypointTime = 10;
        baseTime = 20;
        route = new ArrayList<>();
        crcValue = computeCrc();
    }

    public RouteContainer(final byte[] dataArray) {

        // TODO implement deserialization

    }

    @Override
    public SignalData.Command getDataType() {
        return SignalData.Command.ROUTE_CONTAINER_DATA;
    }

    private int computeCrc() {
        // compute CRC value from whole data, excluding last 4 bytes (CRC value)
        return CommMessage.computeCrc32(Arrays.copyOfRange(serialize(), 0, getDataArraySize() - 4));
    }

    public boolean isValid() {
        return crcValue == computeCrc();
    }

    public ArrayList<CommMessage> getMessages() {
        return CommMessage.buildMessagesList(getDataType(), serialize());
    }

    private static int getWaypointDataSize() {
        return 28;
    }

    private int getDataArraySize() {
        // whole route container size is "Constraint"(16b) + routeSize*"Waypoint"(28b)
        return 16 + routeSize * getWaypointDataSize();
    }

    public byte[] serialize() {
        byte[] dataArray = new byte[getDataArraySize()];
        ByteBuffer buffer = ByteBuffer.allocate(dataArray.length);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        // TODO implement serialization

        System.arraycopy(buffer.array(), 0, dataArray, 0, dataArray.length);
        return dataArray;
    }

    public ArrayList<Waypoint> getRoute() {
        return route;
    }

    public void resetRoute() {
        route.clear();
        routeSize = 0;
    }

    public void addWaypoint(Waypoint waypoint) {
        route.add(waypoint);
        routeSize++;
    }

    public float getWaypointTime() {
        return waypointTime;
    }

    public float getBaseTime() {
        return baseTime;
    }

    public void setWaypointTime(float waypointTime) {
        this.waypointTime = waypointTime;
    }

    public void setBaseTime(float baseTime) {
        this.baseTime = baseTime;
    }
}
