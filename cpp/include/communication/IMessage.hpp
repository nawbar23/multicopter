// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef __I_MESSAGE__
#define __I_MESSAGE__

class IMessage
{
public:
	enum PreambleType
	{
		EMPTY,
		CONTROL,
		SIGNAL,
		AUTOPILOT,
	};

	virtual unsigned getPayloadSize(void) const;
	virtual unsigned getMessageSize(void) const;

	virtual PreambleType getPreambleType(void) const = 0;

	virtual void serialize(unsigned char* data) const = 0;
	virtual void serializeMessage(unsigned char* data) const;

	// creates dynamically allocated message array with binary communication data
	unsigned char* createMessage(void) const;

	// virtual destructor for memmory safeness
	virtual ~IMessage(void);

	static unsigned char getPreambleCharByType(const PreambleType type);
	static PreambleType getPreabmleTypeByChar(const unsigned char data);

	static unsigned getPayloadSizeByType(const PreambleType type);

	static unsigned short computeCrc16(const unsigned char* data, const unsigned dataSize);
	static unsigned computeCrc32(const unsigned char* data, const unsigned dataSize);

	static const unsigned PREAMBLE_SIZE = 4;
	static const unsigned SIGNAL_COMMAND_SIZE = 4;
	static const unsigned SIGNAL_CONSTRAINT_SIZE = 8;
	static const unsigned SIGNAL_DATA_PAYLOAD_SIZE = 50;
	static const unsigned CRC_SIZE = 2;

	static const unsigned SIGNAL_COMMAND_MESSAGE_SIZE =
			PREAMBLE_SIZE + SIGNAL_CONSTRAINT_SIZE + CRC_SIZE;
	static const unsigned SIGNAL_DATA_MESSAGE_SIZE =
			PREAMBLE_SIZE + SIGNAL_CONSTRAINT_SIZE + SIGNAL_DATA_PAYLOAD_SIZE + CRC_SIZE;
	static const unsigned MAX_DATA_SIZE = SIGNAL_DATA_MESSAGE_SIZE;
};

#endif // __I_MESSAGE__
