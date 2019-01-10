import serial
import rapi

ser = serial.Serial("/dev/ttyS0", 9600)

while 1:
    x = int(input("X: "))
    y = int(input("Y: "))
    phi = int(input("Phi: "))
    
    rapi.send_coor(ser, x, y, phi)