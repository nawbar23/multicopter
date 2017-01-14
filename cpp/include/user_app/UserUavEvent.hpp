#ifndef __USER_UAV_EVENT__
#define __USER_UAV_EVENT__

class UserUavEvent
{
public:
    enum Type
    {
        ALLOW,
        DONE,
        SKIP,
        BREAK,
        CHECK,
        STOP_APPLICATION_LOOP,
        STOP_FLIGHT_LOOP,
        STOP_MAGNET_CALIB_OK,
        STOP_MAGNET_CALIB_FAIL,
        STOP_RADIO_CHECK,
        STOP_SENSORS_LOGGER,
    };

    const Type type;

    UserUavEvent(const Type& _type) :
        type(_type)
    {
    }

    Type getType(void) const
    {
        return type;
    }
};

#endif // __USER_UAV_EVENT__
