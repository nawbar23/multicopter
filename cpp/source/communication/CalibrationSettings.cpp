#include "CalibrationSettings.hpp"

#include <string.h>

#include "IMessage.hpp"
#include "SignalData.hpp"

CalibrationSettings::CalibrationSettings(void)
{
}

CalibrationSettings::CalibrationSettings(const unsigned char* src)
{
	memcpy((unsigned char*)this + 4, src, getDataSize());
}

void CalibrationSettings::serialize(unsigned char* dst) const
{
	memcpy(dst, (unsigned char*)this + 4, getDataSize());
}

unsigned CalibrationSettings::getDataSize(void) const
{
	return sizeof(CalibrationSettings) - 4;
}

SignalData::Command CalibrationSettings::getSignalDataType(void) const
{
	return SignalData::CALIBRATION_SETTINGS_DATA;
}

SignalData::Command CalibrationSettings::getSignalDataCommand(void) const
{
	return SignalData::CALIBRATION_SETTINGS;
}

IMessage::MessageType CalibrationSettings::getMessageType(void) const
{
    return CALIBRATION_SETTINGS;
}

CalibrationSettings CalibrationSettings::createDefault(void)
{
	CalibrationSettings calibData;
	calibData.gyroOffset = Vect3Df();
	calibData.accelCalib = Mat3Df::eye();
	calibData.magnetSoft = Mat3Df::eye();
	calibData.magnetHard = Vect3Df();
	calibData.altimeterSetting = (float)roboLib::normPressure;
	calibData.temperatureSetting = (float)roboLib::normTemperature;
	calibData.radioLevels = getDefaultRadioLevels();
	calibData.setDefaultPwmInputMap();
	calibData.boardType = TYPE_UNKNOWN;
	calibData.flags = 0;
	calibData.setCrc();
	return calibData;
}

bool CalibrationSettings::isValid(void) const
{
	switch (boardType)
	{
	case TYPE_UNKNOWN:
	case TYPE_ULTIMATE_V4:
	case TYPE_ULTIMATE_V5:
	case TYPE_BASIC_V1:
	case TYPE_BASIC_V2:
	case TYPE_BASIC_V3:
		break;
	default:
		return false;
	}

	unsigned char dataTab[sizeof(CalibrationSettings)];
	serialize(dataTab);
	return IMessage::computeCrc32(dataTab, getDataSize() - 4) == crcValue
		&& magnetSoft.getDet() != 0.0f
		&& accelCalib.getDet() != 0.0f;
}

void CalibrationSettings::setCrc(void)
{
	unsigned char dataTab[sizeof(CalibrationSettings)];
	serialize(dataTab);
	crcValue = IMessage::computeCrc32(dataTab, getDataSize() - 4);
}

Mat4Df CalibrationSettings::getDefaultRadioLevels(void)
{
	Mat4Df inputLevels;

	inputLevels.mat[0] = 1.0f;
	inputLevels.mat[1] = 2.0f;

	inputLevels.mat[2] = 1.0f;
	inputLevels.mat[3] = 2.0f;

	inputLevels.mat[4] = 1.0f;
	inputLevels.mat[5] = 2.0f;

	inputLevels.mat[6] = 1.0f;
	inputLevels.mat[7] = 2.0f;

	inputLevels.mat[8] = 1.0f;
	inputLevels.mat[9] = 2.0f;

	inputLevels.mat[10] = 1.0f;
	inputLevels.mat[11] = 2.0f;

	inputLevels.mat[12] = 1.0f;
	inputLevels.mat[13] = 2.0f;

	inputLevels.mat[14] = 1.0f;
	inputLevels.mat[15] = 2.0f;

	return inputLevels;
}

void CalibrationSettings::setDefaultPwmInputMap(void)
{
	for (unsigned char i = 0; i<8; i++)
	{
		this->pwmInputMapData[i] = -1;
	}
}

void CalibrationSettings::setPwmInputMap(char* map)
{
	for (unsigned i = 0; i < 8; i++)
	{
		this->pwmInputMapData[i] = map[i];
	}
}
