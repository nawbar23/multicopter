package com.multicopter.java.data;

import com.multicopter.java.CommMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by ebarnaw on 2016-10-14.
 */
public class CalibrationSettings implements SignalPayloadData {

    private  float[] gyroOffset; // Vect3Df
    private float[] accelCalib; // Mat3Df
    private float[] magnetSoft; // Mat3Df
    private float[] magnetHard; // Vect3Df
    private  float altimeterSetting;
    private float temperatureSetting;
    private float[] radioLevels; // Mat4Df
    private byte[] pwmInputLevels; // 8 params
    private int boardType;
    private Flags flags;
    private int crcValue;

    public CalibrationSettings() {
        gyroOffset = new float[3];
        accelCalib = new float[9];
        magnetSoft = new float[9];
        magnetHard = new float[3];
        radioLevels = new float[16];
        pwmInputLevels = new byte[8];
        flags = new Flags(32);

        accelCalib[0] = 1.0f;
        accelCalib[4] = 1.0f;
        accelCalib[8] = 1.0f;

        magnetSoft[0] = 1.0f;
        magnetSoft[4] = 1.0f;
        magnetSoft[8] = 1.0f;

        altimeterSetting = 1013.2f;
        temperatureSetting = 288.15f;
        crcValue = computeCrc();

        System.out.println(crcValue);
    }

    public CalibrationSettings(final byte[] dataArray) {
        ByteBuffer buffer = ByteBuffer.wrap(dataArray);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        for (int i = 0; i < gyroOffset.length; i++) {
            gyroOffset[i] = buffer.getFloat();
        }
        for (int i = 0; i < accelCalib.length; i++) {
            accelCalib[i] = buffer.getFloat();
        }
        for (int i = 0; i < magnetSoft.length; i++) {
            magnetSoft[i] = buffer.getFloat();
        }
        for (int i = 0; i < magnetHard.length; i++) {
            magnetHard[i] = buffer.getFloat();
        }

        altimeterSetting = buffer.getFloat();
        temperatureSetting = buffer.getFloat();

        for (int i = 0; i < radioLevels.length; i++) {
            radioLevels[i] = buffer.getFloat();
        }
        for (int i = 0; i < pwmInputLevels.length; i++) {
            pwmInputLevels[i] = buffer.get();
        }

        boardType = buffer.getInt();
        flags.setFlags(buffer.getInt());
        crcValue = buffer.getInt();
    }

    @Override
    public SignalData.Command getDataType() {
        return SignalData.Command.CALIBRATION_SETTINGS_DATA;
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
        return 188;
    }

    public byte[] serialize() {
        byte[] dataArray = new byte[getDataArraySize()];
        ByteBuffer buffer = ByteBuffer.allocate(dataArray.length);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        for (float aGyroOffset : gyroOffset) {
            buffer.putFloat(aGyroOffset);
        }
        for (float anAccelCalib : accelCalib) {
            buffer.putFloat(anAccelCalib);
        }
        for (float aMagnetSoft : magnetSoft) {
            buffer.putFloat(aMagnetSoft);
        }
        for (float aMagnetHard : magnetHard) {
            buffer.putFloat(aMagnetHard);
        }

        buffer.putFloat(altimeterSetting);
        buffer.putFloat(temperatureSetting);

        for (float radioLevel : radioLevels) {
            buffer.putFloat(radioLevel);
        }
        for (byte pwmInputLevel : pwmInputLevels) {
            buffer.put(pwmInputLevel);
        }

        buffer.putInt(boardType);
        buffer.putInt(flags.getFlags());
        buffer.putInt(crcValue);

        System.arraycopy(buffer.array(), 0, dataArray, 0, dataArray.length);
        return dataArray;
    }

    public float[] getGyroOffset() {
        return gyroOffset;
    }

    public float[] getAccelCalib() {
        return accelCalib;
    }

    public float[] getMagnetSoft() {
        return magnetSoft;
    }

    public float[] getMagnetHard() {
        return magnetHard;
    }

    public float getAltimeterSetting() {
        return altimeterSetting;
    }

    public float getTemperatureSetting() {
        return temperatureSetting;
    }

    public float[] getRadioLevels() {
        return radioLevels;
    }

    public byte[] getPwmInputLevels() {
        return pwmInputLevels;
    }

    public int getBoardType() {
        return boardType;
    }

    public Flags getFlags() {
        return flags;
    }
}