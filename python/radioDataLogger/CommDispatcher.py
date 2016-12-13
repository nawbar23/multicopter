from struct import *

class CommDispatcher:
    PREAMBLE_SIZE = 4
    PREAMBLE_BYTE = '$'

    preambleBuffer = ['0'] * 3 # TODO hardcode
    preambleBufferCnt = 0

    dataBuffer = ['0'] * 34 # TODO hardcode
    dataBufferCnt = 0
    
    def __init__(self, onDataReceived):
        self.onDataReceived = onDataReceived    

        #self.reset()

    def reset(self):
        self.preambleBuffer = ['0'] * 3
        self.preambleBufferCnt = 0       
        self.dataBuffer = ['0'] * 34
        self.dataBufferCnt = 0

    def processData(self, data):
        for byte in data:
            self.processByte(byte)

    def processByte(self, byte):
        if self.updatePreamble(byte):
            # new preamble received
            self.activatePreamble()
            return

        self.dataBuffer[self.dataBufferCnt] = byte;
        self.dataBufferCnt += 1

        if self.dataBufferCnt == 34: # TODO hardcode
            # pacekt received, check CRC
            packet = self.dataBuffer[ : self.dataBufferCnt]
            if self.isCrcValid(packet):
                self.onDataReceived(packet[ : -2])
            self.dataBufferCnt = 0

    def updatePreamble(self, byte):
        result = False        
        if (ord(byte) == 0):
            allEquals = True
            for e in self.preambleBuffer:
                if self.preambleBuffer[0] != e:
                    allEqual = False
                    break
            if allEquals and self.preambleBuffer[0] == self.PREAMBLE_BYTE:
                result = True

        self.preambleBuffer[self.preambleBufferCnt] = byte
        self.preambleBufferCnt += 1
        if self.preambleBufferCnt >= self.PREAMBLE_SIZE - 1:
            self.preambleBufferCnt = 0

        return result

    def activatePreamble(self):
        self.dataBufferCnt = 0

    def isCrcValid(self, data):
        crcShort = 0
        b = map(ord, data[0:-2])
        for byte in b:
            crcShort = ((crcShort >> 8) | (crcShort << 8) )& 0xffff
            crcShort ^= (byte & 0xff)
            crcShort ^= ((crcShort & 0xff) >> 4)
            crcShort ^= (crcShort << 12) & 0xffff
            crcShort ^= ((crcShort & 0xFF) << 5) & 0xffff
        crcShort &= 0xffff
        if crcShort != unpack("<H", ''.join(data[-2:]))[0]:
            print "Error wrong packet CRC"
            return False
        else:
            return True

    @staticmethod
    def toStringHex(data, sep = None):
        i = 0
        if sep is None:
            sep = [4,8,12,16,20,24,28,32,36]
        ret = ''
        for b in data:
          if i in sep: 
            ret += '|'
          ret += b.encode('hex')
          i = i + 1
        return ret    



