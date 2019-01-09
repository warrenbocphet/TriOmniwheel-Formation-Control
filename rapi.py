import serial
import time

def write_coor(ser_fnc, data):
    if (data < 0):
        data = data * (-1)
        dataLow = data & 0xFF
        dataHigh = (data >> 8) & 0xFF
        ser_fnc.write([dataHigh])
        ser_fnc.write([dataLow])
        ser_fnc.write([1])
    else:
        dataLow = data & 0xFF
        dataHigh = (data >> 8) & 0xFF
        ser_fnc.write([dataHigh])
        ser_fnc.write([dataLow])
        ser_fnc.write([3])

def send_coor(ser, x, y, phi):
    ser.write([255])

    write_coor(ser, int(x))
    write_coor(ser, int(y))
    write_coor(ser, int(phi))

def reset_state(ser):
    ser.write([254])
