package com.multicopter.java.data;

import com.multicopter.java.CommMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by ebarnaw on 2016-10-14.
 */
public class ControlSettings implements SignalPayloadData {

    // base control settings
    private int uavType;
    private int initialSolverMode;
    private int manualThrottleMode;

    // auto landing settings
    private float autoLandingDescendRate;
    private float maxAutoLandingTime;

    // control values
    private float maxRollPitchControlValue;
    private float maxYawControlValue;

    // regulator control settings
    // stabilization
    private float[] pidRollRate, pidPitchRate, pidYawRate; // Vect3Df
    // angle
    private float rollProp, pitchProp, yawProp;

    // throttle controller settings
    private float altPositionProp;
    private float altVelocityProp;
    private float[] pidThrottleAccel; // Vect3Df

    // throttle controller stick settings
    private float throttleAltRateProp;

    // autopilot settings
    private float maxAutoAngle;
    private float maxAutoVelocity;
    private float autoPositionProp;
    private float autoVelocityProp;
    private float[] pidAutoAccel; // Vect3Df

    // autopilot stick settings
    private float stickPositionRateProp;
    private int stickMovementMode;

    // battery type
    private int batteryType;

    // error handling type
    private int errorHandlingAction;

    // esc controller PWM frequency
    private int escPwmFreq;

    // gps sensors position in rigid body coordinate system
    private float[] gpsSensorPosition; // Vect3Df

    // flags for any boolean settings
    private Flags flags;

    private int crcValue;

    public ControlSettings() {
        pidRollRate = new float[3];
        pidPitchRate = new float[3];
        pidYawRate = new float[3];
        pidThrottleAccel = new float[3];
        pidAutoAccel = new float[3];
        gpsSensorPosition = new float[3];
        flags = new Flags(32, 0);

        uavType = UavType.QUADROCOPTER_X.getValue();
        initialSolverMode = ControlData.SolverMode.ANGLE_NO_YAW.getValue();
        manualThrottleMode = ThrottleMode.DYNAMIC.getValue();

        rollProp = 0.0f;
        pitchProp = 0.0f;
        yawProp = 0.0f;
        autoLandingDescendRate = 0.0f;
        maxAutoLandingTime = 0.0f;
        maxRollPitchControlValue = 0.0f;
        maxYawControlValue = 0.0f;

        // throttle controller
        altPositionProp = 0.0f;
        altVelocityProp = 0.0f;
        throttleAltRateProp = 0.0f;

        // autopilot
        autoPositionProp = 0.0f;
        autoVelocityProp = 0.0f;
        maxAutoAngle = 0.0f;
        maxAutoVelocity = 0.0f;
        stickPositionRateProp = 0.0f;
        stickMovementMode = StickMovementMode.COPTER.getValue();

        batteryType = BatteryType.UNDEFINED.getValue();
        errorHandlingAction = DebugData.ControllerState.AUTOLANDING.getValue();
        escPwmFreq = EscPwmFreq.MEDIUM.getValue();

        crcValue = computeCrc();
    }

    public ControlSettings(final byte[] dataArray) {

        // TODO implement deserialization

    }

    @Override
    public SignalData.Command getDataType() {
        return SignalData.Command.CONTROL_SETTINGS_DATA;
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

    private int getDataArraySize() {
        // TODO wrong data size !!!
        return 188;
    }

    public byte[] serialize() {
        byte[] dataArray = new byte[getDataArraySize()];
        ByteBuffer buffer = ByteBuffer.allocate(dataArray.length);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        // TODO implement serialization

        System.arraycopy(buffer.array(), 0, dataArray, 0, dataArray.length);
        return dataArray;
    }

    public enum UavType
    {
        TRICOPTER(1000),
        QUADROCOPTER_X(2000),
        QUADROCOPTER_PLUS(2500),
        HEXACOPTER_X(3000),
        HEXACOPTER_PLUS(3500),
        OCTOCOPTER_X(4000),
        OCTOCOPTER_PLUS(4500);

        private final int value;

        UavType(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }

        public static UavType getUavType(final int value) {
            for (UavType uavType : UavType.values()) {
                if (uavType.getValue() == value) {
                    return uavType;
                }
            }
            return QUADROCOPTER_X; // TODO throw some exception
        }
    }

    public enum ThrottleMode
    {
        STATIC(10),
        DYNAMIC(20),
        RATE(30),
        ALTITUDE(40);

        private final int value;

        ThrottleMode(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }
    }

    public enum StickMovementMode
    {
        COPTER(0),
        GEOGRAPHIC(1),
        BASE_POINT(2);

        private final int value;

        StickMovementMode(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }
    }

    public enum BatteryType
    {
        UNDEFINED(0),
        BATTERY_2S(2),
        BATTERY_3S(3),
        BATTERY_4S(4),
        BATTERY_5S(5),
        BATTERY_6S(6);

        private final int value;

        BatteryType(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }
    }

    public enum EscPwmFreq
    {
        SLOW(0),
        MEDIUM(1),
        FAST(2),
        VERY_FAST(3),
        ONESHOT_125(4);

        private final int value;

        EscPwmFreq(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }
    }

    public enum FlagId
    {
        ENABLE_FLIGHT_LOGGER(0),
        ALLOW_DYNAMIC_AUTOPILOT(1),
        GPS_SENSORS_POSITION_DEFINED(2);

        private final int value;

        FlagId(int value){
            this.value = value;
        }

        int getValue(){
            return value;
        }
    }

    public int getUavType() {
        // TODO fix it, should return ENUM
        return uavType;
    }

    public int getInitialSolverMode() {
        // TODO fix it, should return ENUM
        return initialSolverMode;
    }

    public int getManualThrottleMode() {
        // TODO fix it, should return ENUM
        return manualThrottleMode;
    }

    public float getAutoLandingDescendRate() {
        return autoLandingDescendRate;
    }

    public float getMaxAutoLandingTime() {
        return maxAutoLandingTime;
    }

    public float getMaxRollPitchControlValue() {
        return maxRollPitchControlValue;
    }

    public float getMaxYawControlValue() {
        return maxYawControlValue;
    }

    public float[] getPidRollRate() {
        return pidRollRate;
    }

    public float[] getPidPitchRate() {
        return pidPitchRate;
    }

    public float[] getPidYawRate() {
        return pidYawRate;
    }

    public float getRollProp() {
        return rollProp;
    }

    public float getPitchProp() {
        return pitchProp;
    }

    public float getYawProp() {
        return yawProp;
    }

    public float getAltPositionProp() {
        return altPositionProp;
    }

    public float getAltVelocityProp() {
        return altVelocityProp;
    }

    public float[] getPidThrottleAccel() {
        return pidThrottleAccel;
    }

    public float getThrottleAltRateProp() {
        return throttleAltRateProp;
    }

    public float getMaxAutoAngle() {
        return maxAutoAngle;
    }

    public float getMaxAutoVelocity() {
        return maxAutoVelocity;
    }

    public float getAutoPositionProp() {
        return autoPositionProp;
    }

    public float getAutoVelocityProp() {
        return autoVelocityProp;
    }

    public float[] getPidAutoAccel() {
        return pidAutoAccel;
    }

    public float getStickPositionRateProp() {
        return stickPositionRateProp;
    }

    public int getStickMovementMode() {
        // TODO fix it, should return ENUM
        return stickMovementMode;
    }

    public int getBatteryType() {
        // TODO fix it, should return ENUM
        return batteryType;
    }

    public int getErrorHandlingAction() {
        // TODO fix it, should return ENUM
        return errorHandlingAction;
    }

    public int getEscPwmFreq() {
        // TODO fix it, should return ENUM
        return escPwmFreq;
    }

    public float[] getGpsSensorPosition() {
        return gpsSensorPosition;
    }

    public Flags getFlags() {
        return flags;
    }
}
