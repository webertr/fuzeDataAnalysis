#!/usr/bin/env python3.8

# This is code to access the Tektronix TPS 2024B scope via RS232, and potentially push
# data to mdsplus.
# Need to run this with "sudo -E ./test.py"
# -E keeps environment variables. sudo allows you access to the /dev stream
# Or run "sudo chmod 666 /dev/ttyUSB0"

import serial
import MDSplus
import struct
import matplotlib.pyplot as plt
import numpy
import os
from decimal import Decimal


def getScopeChData(chNumber=1, portName='/dev/ttyUSB0'):
    """
    This should open a serial port, and get data from the scope.
    The scope if Tektronix TPS 2024B. It represents data with only 1 byte.
    You need the folder /dev/ttyUSB0 to have read/write access. Do this with
    either "sudo chmod 666 /dev/ttyUSB0" or run the scrip with
    "sudo -E ./serialTekScopeComm.py"
    """
    # (Baud=9600, Flow=hardflagging, EOL String=LF,Parity=None).
    ser = serial.Serial(
        baudrate = 9600,
        port = portName,
        parity = serial.PARITY_NONE,
        bytesize = 8,
        timeout = 5,
    )

    # Not sure if this is necessary
    ser.flush()
    ser.write(str.encode("*CLS\n"))

    # Getting scope ID and printing out.
    #ser.write(str.encode("ID?\n"))
    #print("ID: " + str(ser.read_until(str.encode("\n"))))

    # Setting data source to channel number
    stringCommand = "DATA:SOURCE CH?\n"
    stringCommand = stringCommand.replace("?", str(chNumber))
    ser.write(str.encode(stringCommand))
    
    # Getting waveform information and printing out.
    ser.write(str.encode("WFMpre?\n"))
    chInfo = ser.read_until(str.encode("\n"))
    print("Waveform Information: " + str(chInfo))
    
    # Signed range should be 0 to 256. Only 1 byte of data
    ser.write(str.encode("DATA:WIDTH 1\n"))

    # Signed integer with most significant byte is transfered first
    # Range should be 0 to 256. Verifying that correct data encoding is returned
    ser.write(str.encode("DATA:ENCDG RIB\n"))
    ser.write(str.encode("DATA:ENCDG?\n"))
    chDataEncode = ser.read_until(str.encode("\n"))
    if (chDataEncode != b'RIBINARY\n'):
        print("Error. Expected " + str(b'RIBINARY\n') +
              " but got: " + str(chDataEncode))

    # Getting number of points. Need this to know how much to read back
    ser.write(str.encode("WFMPre:NR_Pt?\n"))
    numPoints = int(ser.read_until(str.encode("\n")))

    # Get y multiplication factor
    ser.write(str.encode("WFMPre:YMUlt?\n"))
    yFactor = float(ser.read_until(str.encode("\n")))

    # Get y offset
    ser.write(str.encode("WFMPre:YOFf?\n"))
    yOffset = float(ser.read_until(str.encode("\n")))

    # Get y zero
    ser.write(str.encode("WFMPre:YZEro?\n"))
    yZero = float(ser.read_until(str.encode("\n")))
    
    # Requesting returned data for specified channel
    ser.write(str.encode("CURVE?\n"))
    
    # Reading "CURVE<space>". Junk that we don't want
    byte1 = ser.read(size=6)

    # Reading points
    chList = []
    for ii in range(0, numPoints, 1):
        byte1 = ser.read(size=1)
        chList.append(byte1)

    # Reading last "\n"
    byte1 = ser.read(size=1)
    
    # Reading first character. This method no longer works because sometimes that data i
    # 10 which corresponds to a \n character. This was needed because evidently
    # the read_until method had a maximum number of characters it would read
    #byte1 = ser.read(size=1)
    #chList = []
    #while byte1 != str.encode("\n"):
    #    chList.append(byte1)    
    #    byte1 = ser.read(size=1)
    #print("Byte stop: " + str(byte1))

    # Coverting bytes to integers and appending to a list
    chValueList = []
    for chByte in chList:
        chValueList.append(int.from_bytes(chByte, byteorder='little', signed=True))

    # Doing conversion to scope units
    chValueListConvert = [ (yFactor * val - yOffset) + yZero for val in chValueList]
    
    # Verifying we read the correct number of points
    chNumPoints = len(chValueListConvert)
    if (chNumPoints != numPoints):
        print("Error. Number of points not correct. Expected: " + str(numPoints) +
              " but got " + str(chNumPoints))

    # Closing the serial connection
    ser.close()

    # Returning the integer list
    return chValueListConvert


def getScopeTimeBase(chNumber=1, portName='/dev/ttyUSB0'):
    """
    This returns the time base of the scope.
    """
    # (Baud=9600, Flow=hardflagging, EOL String=LF,Parity=None).
    ser = serial.Serial(
        baudrate = 9600,
        port = portName,
        parity = serial.PARITY_NONE,
        bytesize = 8,
        timeout = 5,
    )

    # Not sure if this is necessary    
    ser.flush()
    ser.write(str.encode("*CLS\n"))

    # Setting data source to channel number
    stringCommand = "DATA:SOURCE CH?\n"
    stringCommand = stringCommand.replace("?", str(chNumber))
    ser.write(str.encode(stringCommand))

    # Getting the time base increment
    ser.write(str.encode("WFMPre:XINcr?\n"))
    xDelta = float(ser.read_until(str.encode("\n")))

    # Getting the number of points
    ser.write(str.encode("WFMPre:NR_Pt?\n"))
    numPoints = int(ser.read_until(str.encode("\n")))

    # Closing the serial connection
    ser.close()

    # Converting increment and number of points to time base vector
    timeBase = []
    for ii in range(0, numPoints, 1):
        timeBase.append(xDelta * ii)

    # Returning time base vector
    return timeBase


def pushMDSplusData(timeBase, chData, chNumber, shotNumber):
    """
    Push data to MDSplus tree
    """
    # Opening tree for shot 0
    fuzeTree = MDSplus.Tree('fuze', shotNumber)

    # Creating tree node for specified x-ray channel
    stringCommand = '\\TOP.SIGNALS.XRAY:XRAY_?:RAW'
    stringCommand = stringCommand.replace("?", str(chNumber))
    xRay = fuzeTree.getNode(stringCommand)

    # Allows us to write data
    xRay.setNoWriteShot(False)

    # Creating mdsplus signas
    chArray = MDSplus.Float64Array(chData)
    timeArray = MDSplus.Float64Array(timeBase)
    chSignal = MDSplus.Signal(chArray, None, timeArray)

    # This actually puts the data
    xRay.putData(chSignal)

    # Returns the write protection
    xRay.setNoWriteShot(True)

    
def setBrightness(brightLevel, portName='/dev/ttyUSB0'):
    """
    Set the screen brightness to a percentage level either:
    { 100 | 90 | 75| 60 | 45 | 30 | 15 | 0 }
    """
    
    # It has to be one of the following values
    PERCENT = [100, 90, 75, 60, 45,  30, 10, 0]
    brightLevel = min(PERCENT, key=lambda x:abs(x-brightLevel))

    # (Baud=9600, Flow=hardflagging, EOL String=LF,Parity=None).
    ser = serial.Serial(
        baudrate = 9600,
        port = portName,
        parity = serial.PARITY_NONE,
        bytesize = 8,
        timeout = 5,
    )

    # Not sure if this is necessary    
    ser.flush()
    ser.write(str.encode("*CLS\n"))
    
    # Setting brightness
    stringCommand = "DISplay:BRIGHTness ?\n"
    stringCommand = stringCommand.replace("?", str(brightLevel))
    ser.write(str.encode(stringCommand))

    # Reading brightness
    ser.write(str.encode("DISplay:BRIGHTness?\n"))
    brightLevelRead = int(ser.read_until(str.encode("\n")))

    # Checking for error
    if brightLevelRead != brightLevel:
        print("Error. Brightness level read does not match set. Expected: " +
              str(brightLevel) + " but got " + str(brightLevelRead))

        
def setChScale(chNum, chScale, portName='/dev/ttyUSB0'):
    """
    This sets the value of the ch scale factor
    """
    # (Baud=9600, Flow=hardflagging, EOL String=LF,Parity=None).
    ser = serial.Serial(
        baudrate = 9600,
        port = portName,
        parity = serial.PARITY_NONE,
        bytesize = 8,
        timeout = 5,
    )

    # Not sure if this is necessary    
    ser.flush()
    ser.write(str.encode("*CLS\n"))

    # Setting channel scale CH<x>:SCAle. Need Floating point value with an exponent
    stringCommand = "CH#:SCAle ?\n"
    stringCommand = stringCommand.replace("#", str(chNum))
    stringChScale = '%.1E' % Decimal(chScale)
    stringCommand = stringCommand.replace("?", str(stringChScale))
    ser.write(str.encode(stringCommand))
    
    # Reading channel scale factor
    stringCommand = "CH#:SCAle?\n"
    stringCommand = stringCommand.replace("#", str(chNum))
    ser.write(str.encode(stringCommand))
    chScaleRead = float(ser.read_until(str.encode("\n")))

    if chScaleRead != chScale:
        print("Error. Vertical scale factor read does not match set" +
              "Expected " + str(chScale) + " but got " + str(chScaleRead))

        
# Defining files and deleting if they already exist
timeBaseFileName = 'data/timeBaseNumPyArray.npy'
ch1FileName = 'data/ch1NumPyArray.npy'
ch2FileName = 'data/ch2NumPyArray.npy'
ch3FileName = 'data/ch3NumPyArray.npy'
ch4FileName = 'data/ch4NumPyArray.npy'

# Port that has serial RS282 device
portName = '/dev/ttyUSB0'

getData = False
pushMDSplus = False
plotData = False

# Setting the screen brightness to 0
#setBrightness(0, portName='/dev/ttyUSB0')

# Changing channel vertical scale
#setChScale(1, 0.01, portName='/dev/ttyUSB0')

# Get data from scope and save
if getData:

    if not os.path.exists('data'):
        os.makedirs('data')
    
    # Getting and saving time base
    timeBase = getScopeTimeBase(1, portName)
    timeBaseNumPyArray = numpy.asarray(timeBase, dtype=numpy.float32)
    if os.path.exists(timeBaseFileName):
        os.remove(timeBaseFileName)
    numpy.save(timeBaseFileName, timeBaseNumPyArray)

    # Getting and saving channel 1 data
    ch1Data = getScopeChData(1, portName)
    ch1NumPyArray = numpy.asarray(ch1Data, dtype=numpy.float32)
    if os.path.exists(ch1FileName):
        os.remove(ch1FileName)
    numpy.save(ch1FileName, ch1NumPyArray)

    # Getting and saving channel 2 data
    ch2Data = getScopeChData(2, portName)
    ch2NumPyArray = numpy.asarray(ch2Data, dtype=numpy.float32)
    if os.path.exists(ch2FileName):
        os.remove(ch2FileName)
    numpy.save(ch2FileName, ch2NumPyArray)

    # Getting and saving channel 3 data
    ch3Data = getScopeChData(3, portName)
    ch3NumPyArray = numpy.asarray(ch3Data, dtype=numpy.float32)
    if os.path.exists(ch3FileName):
        os.remove(ch3FileName)
    numpy.save(ch3FileName, ch3NumPyArray)

    # Getting and saving channel 4 data
    ch4Data = getScopeChData(4, portName)
    ch4NumPyArray = numpy.asarray(ch4Data, dtype=numpy.float32)
    if os.path.exists(ch4FileName):
        os.remove(ch4FileName)
    numpy.save(ch4FileName, ch4NumPyArray)

# Push data to MDSplus
if pushMDSplus:
    # Loading numpy data
    timeBase = numpy.load(timeBaseFileName).tolist()
    ch1Data = numpy.load(ch1FileName).tolist()
    ch2Data = numpy.load(ch2FileName).tolist()
    ch3Data = numpy.load(ch3FileName).tolist()
    ch4Data = numpy.load(ch4FileName).tolist()

    # Pushing mdsplus data
    pushMDSplusData(timeBase, ch1Data, 1, 0)
    pushMDSplusData(timeBase, ch2Data, 2, 0)
    pushMDSplusData(timeBase, ch3Data, 3, 0)
    pushMDSplusData(timeBase, ch4Data, 4, 0)

# Plot results
if plotData:
    timeBase = numpy.load(timeBaseFileName).tolist()
    ch1Data = numpy.load(ch1FileName).tolist()
    ch2Data = numpy.load(ch2FileName).tolist()
    ch3Data = numpy.load(ch3FileName).tolist()
    ch4Data = numpy.load(ch4FileName).tolist()

    # Plotting data
    plt.plot(timeBase,ch1Data, 'r')
    plt.plot(timeBase,ch2Data, 'g')
    plt.plot(timeBase,ch3Data, 'b')
    plt.plot(timeBase,ch4Data, 'y')
    plt.ylabel('ScopeData')
    plt.show()
