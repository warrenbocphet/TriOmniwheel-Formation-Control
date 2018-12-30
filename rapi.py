import serial
import time
    
def getState(ser):
    
    ser.write([254])
    while True:
        
        startByte = int.from_bytes(ser.read(), byteorder = 'little')
        
        if startByte == 255:
            buf = ser.read(9)
            
            x = buf[1] | (buf[0] << 8)
            x_sign = buf[2] - 2
            x = x * x_sign
            
            y = buf[4] | (buf[3] << 8)
            y_sign = buf[5] - 2
            y = y * y_sign
            
            phi = buf[7] | (buf[6] << 8)
            phi_sign = buf[8] - 2
            phi = phi * phi_sign
            
            returnState = [x, y, phi]

            return returnState

def write_spd_dir(ser_fnc, speed):
    if (speed < 0):
        speed = speed * (-1)
        dataLow = speed & 0xFF
        dataHigh = (speed >> 8) & 0xFF
        ser_fnc.write([dataHigh])
        ser_fnc.write([dataLow])
        ser_fnc.write([1])
    else:
        dataLow = speed & 0xFF
        dataHigh = (speed >> 8) & 0xFF
        ser_fnc.write([dataHigh])
        ser_fnc.write([dataLow])
        ser_fnc.write([3])

def send_spd(ser, x_speed, y_speed, z_speed, stop_byte):
    # write to serial port
    ser.write([255]) # Write the starting byte

    write_spd_dir(ser, int(x_speed))
    write_spd_dir(ser, int(y_speed))
    write_spd_dir(ser, int(z_speed))

    ser.write([stop_byte])

def reset_state(ser):
    ser.write([253])
