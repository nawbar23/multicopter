#include "ISignalPayloadMessage.hpp"

IMessage::PreambleType ISignalPayloadMessage::getPreambleType(void) const
{
	return IMessage::SIGNAL;
}