from struct import *

class SensorsData:

    # sensors reading data container
    messageFormat = '<f9h5H'
    # '<'               encoding = network = big endian
    # 0           'f'         preassure  
    # 1:9        '9h'      gyro, accel, magnet - Vect3Ds
    # 10:14   '5H'      coded GPS data: lat, lon, speed, course, altitude

    values = None

    def __init__(self, packet):
        packetString = ''.join(packet)
        self.values = unpack(self.messageFormat, packetString)

    def toLogString(self):
        result = ''
        for d in self.values:
            result += str(d) + ", "
        return result  

    @staticmethod
    def getSeparators():
        return [4,10,16,22]      
