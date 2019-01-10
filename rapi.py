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

def send_coor(ser, x, y, phi, time):
    ser.write([255])

    write_coor(ser, int(x))
    write_coor(ser, int(y))
    write_coor(ser, int(phi))
    write_coor(ser, int(time))

def getState(ser):
    
    ser.write([253])
    while True:
        
        startByte = int.from_bytes(ser.read(), byteorder = 'little')
        
        if startByte == 255:
            buf = ser.read(20)
            
            x = buf[1] | (buf[0] << 8)
            x_sign = buf[2] - 2
            x = x * x_sign
            
            y = buf[4] | (buf[3] << 8)
            y_sign = buf[5] - 2
            y = y * y_sign
            
            phi = buf[7] | (buf[6] << 8)
            phi_sign = buf[8] - 2
            phi = phi * phi_sign

            v0 = buf[10] | (buf[9] << 8)
            v0_sign = buf[11] - 2
            v0 = v0 * v0_sign

            v1 = buf[13] | (buf[12] << 8)
            v1_sign = buf[14] - 2
            v1 = v1 * v1_sign

            v2 = buf[16] | (buf[15] << 8)
            v2_sign = buf[17] - 2
            v2 = v2 * v2_sign
            
            timer = buf[19] | (buf[18] << 8)
            
            returnState = [x, y, phi, v0, v1, v2, timer]

            return x, y, phi, v0/100.0, v1/100.0, v2/100.0, timer

def reset_state(ser):
    ser.write([254])
