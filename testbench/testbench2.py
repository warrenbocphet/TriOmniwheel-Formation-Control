import serial
import rapi

ser = serial.Serial("/dev/ttyS0", 9600)
destination = []

while 1:
    print()
    print("Mode: ")
    print("1: Test mode 1.")
    print("2: Test mode 2.")
    print("3: Exit the program.")
    print()
    a = int(input("Enter the mode you want to do: "))
    
    while 1:
        if (a == 1 or a == 2 or a == 3):
            break
        else:
            a = int(input("Wrong mode. Enter the mode you want to do: "))
    
    if (a == 1):
        destination.append([0, 0, 0, 0])
        destination.append([-50, 50, 0, 0])
        destination.append([-50, 50, 45, 0])
        destination.append([-50, 50, 0, 0])
        destination.append([50, 50, 0, 0])
        destination.append([50, 50, -90, 0])
        destination.append([50, 50, 0, 0])
        destination.append([50, -50, 0, 0])
        destination.append([50, -50, 135, 0])
        destination.append([50, -50, 0, 0])
        destination.append([-50, -50, 0, 0])
        destination.append([-50, -50, -180, 0])
        destination.append([-50, -50, 0, 0])
        destination.append([0, 0, 0, 0])
        
    if (a == 2):
        destination.append([0, 0, 0, 0])
        destination.append([-50, 50, 45, 0])
        destination.append([50, 50, -90, 0])
        destination.append([50, -50, 135, 0])
        destination.append([-50, -50, -180, 0])
        destination.append([0, 0, 0, 0])
    
    if (a == 3):
        break
    
    for i in range(len(destination)):
        try:
            rapi.send_coor(ser, destination[i][0], destination[i][1], destination[i][2], destination[i][3])
            print("Sent coordinate: " + str(destination[i]))
            print("Rover is running, waiting for rover to reach destination.")
            signal = ser.read()
            if (signal == 1):
                print("Reach destination.")
            print()
        
        except KeyboardInterrupt:
            print()
            print()
            print("Exception: KeyboardInterrupt")
            print("Reset robot coordinate and stop the robot immediately!")
            rapi.reset_state(ser)
            break
    
    print("Finish the path.")
    rapi.reset_state(ser)
    destination = []
