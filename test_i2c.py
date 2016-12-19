#!/usr/bin/python2
import smbus
import time
import sys
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x12

def writeNumber(value):
    bus.write_byte(address, value)
    # bus.write_byte_data(address, 0, value)
    return -1

def readNumber():
    number = bus.read_byte(address)
    # number = bus.read_byte_data(address, 1)
    return number


var = int(sys.argv[1])

writeNumber(var)
#print "RPI to arduino: ", var
# sleep one second
number = readNumber()
#print "Arduino to rpi:", number
