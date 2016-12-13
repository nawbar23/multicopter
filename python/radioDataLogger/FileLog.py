from datetime import datetime
import threading

class FileLog:
    start = None
    logfile = None
    lock = None

    def __init__(self, dir):
        date = datetime.now().strftime('%d-%b-%Y_%H%M%S')
        fileName = dir + date + '.log'
        print("FileLog opened at: '"  + fileName  + "'")
        self.logfile = open(fileName, 'w')
        self.lock = threading.Lock()
        self.start = datetime.now()

    def timeStamp(self):
        dt = datetime.now() - self.start
        s = dt.seconds + dt.microseconds / 1000000.0
        return str(s)

    def putLine(self, description):
        self.lock.acquire()
        self.logfile.write(self.timeStamp() + ", " + description + "\n")
        self.lock.release()

    def close(self):
        self.logfile.close()
