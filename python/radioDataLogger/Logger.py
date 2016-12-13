#!/usr/bin/python

from UartController import UartController
from CommDispatcher import CommDispatcher

from SensorsData import SensorsData

from FileLog import FileLog
from datetime import datetime
import time
import struct

logsDir = "logs/"
maxNoReceptionTime = 1500

print "Starting logger over UART interface"

fileLog = None
loggingStarted = datetime.now()
receptionCounter = 0
lastReception = datetime.now()

def getMilis(startDate):
    dt = datetime.now() - startDate
    return dt.seconds * 1000.0 + dt.microseconds / 1000.0

def onDataReceived(packet):
    global fileLog
    global loggingStarted
    global receptionCounter
    global lastReception

    # parse SensorsData to string logs
    logLine = SensorsData(packet).toLogString()
    #print "Received: " + logLine

    # if there is no opened log file, create it
    if fileLog is None:
        fileLog = FileLog(logsDir)
        loggingStarted = datetime.now()
        receptionCounter = 0

    fileLog.putLine(logLine)
    receptionCounter += 1
    lastReception = datetime.now()

def getClosingInfo(loggingStarted, receptionCounter):
    loggingTimeSeconds = (getMilis(loggingStarted) - maxNoReceptionTime) / 1000.0
    receptionFreq = receptionCounter / loggingTimeSeconds
    return "Logging time: " + str(loggingTimeSeconds) + " s, freq: " + str(receptionFreq) + " Hz"


uart = UartController("/dev/serial0", 230400)
dispatcher = CommDispatcher(onDataReceived)

# main logger loop
while True:
    data = uart.recv()
    dataLength = len(data)
    if dataLength > 0:
        dispatcher.processData(data)
    elif fileLog is not None and getMilis(lastReception) > maxNoReceptionTime:
        closeingInfo = getClosingInfo(loggingStarted, receptionCounter)
        print closeingInfo + "\n"
        fileLog.putLine(closeingInfo)         
        fileLog.close()
        fileLog = None

uart.close()

if fileLog is not None:
    fileLog.close()

print "Logger done!"
