// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __CALIBRATION_SETTINGS__
#define __CALIBRATION_SETTINGS__

#include "roboLibCore.hpp"

#include "IXmlParcable.hpp"
#include "ISignalPayloadMessage.hpp"
#include "SignalData.hpp"

#include "Flags.hpp"

class CalibrationSettings : public ISignalPayloadMessage, public IXmlParcable
{
public:
	enum BoardType
	{
		TYPE_UNKNOWN,
		TYPE_ULTIMATE_V4 = 4,
		TYPE_ULTIMATE_V5 = 5,
		TYPE_BASIC_V1 = 101,
		TYPE_BASIC_V2 = 102,
		TYPE_BASIC_V3 = 103,
	};

	enum FlagId
	{
		IS_GPS_CONNECTED,
		IS_EXTERNAL_MAGNETOMETER_USED,
	};

	Vect3Df gyroOffset;
	Mat3Df accelCalib;
	Mat3Df magnetSoft;
	Vect3Df magnetHard;

	float altimeterSetting; // [hPa]
	float temperatureSetting; // [K]

	Mat4Df radioLevels;
	char pwmInputMapData[8];

	int boardType;

	Flags<unsigned> flags;

	CalibrationSettings(void);
	CalibrationSettings(const unsigned char* src);

	void serialize(unsigned char* dst) const;

	unsigned getDataSize(void) const;

	SignalData::Command getSignalDataType(void) const;
	SignalData::Command getSignalDataCommand(void) const;

	bool isValid(void) const;

	void setCrc(void);

	static CalibrationSettings createDefault(void);

	static Mat4Df SetDefaultRadioLevels(void);

	void SetDefaultPwmInputMap(void);

	void SetPwmInputMap(char* map);

private:
	unsigned crcValue;
};

#endif // __CALIBRATION_SETTINGS__
