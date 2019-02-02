from guizero import App, Text, TextBox, PushButton, Picture, Window, ButtonGroup
from picamera import PiCamera
from multiprocessing import Process, Array, Value
import socket
import os
import time
from plot_map import plot
import serial
import rapi

title_size = 13
text_size = 10

ser = serial.Serial("/dev/ttyS0", 9600)

##setup raspberry pi's ip address and kill the process on port
os.system('sudo fuser -n tcp -k 50000')
os.system('sudo ifconfig wlan0 down')
os.system('sudo ifconfig wlan0 129.127.29.125')
os.system('sudo ifconfig wlan0 up')

#declare the ip address and port
ip = '129.127.29.125'
port = 50000

##comminucate with host computer
address = (ip, port)
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)
server.bind(address)

print('binded')
server.listen(10)
(client, addr) = server.accept()
print('connected with ', client)

#camera = PiCamera()
#camera.resolution = (800,480)

#open camera window
def open_window():
    camera.start_preview(fullscreen = False, window = (50, 50, 600, 400))

#close camera window
def close_window():  
    camera.stop_preview()

def send_input():
    
    rapi.send_coor(ser, int(inx_box.value),int(iny_box.value),
                            int(inr_box.value), int(ins_box.value))
    flag.value = -1
    to_host = ('R'+str(format(int(inx_box.value),'03'))+' '+
               str(format(int(iny_box.value),'03'))+' '+
               str(format(int(inr_box.value),'03'))+' '+
               str(format(int(ins_box.value),'03'))).encode()
    client.send(to_host)
    
    
    
def send_reset():
   rapi.reset_state(ser)
   flag.value = 1
   client.send(b'reset01234567890')
    
def update_state():
    x,y,theta,wx,wy,wz,time = rapi.getState(ser)
    current_value[:] = [x,y,theta,wx,wy,wz]
    X_displacement_data.set(x)
    Y_displacement_data .set(y)
    X_velocity_data.set(wx)
    Y_velocity_data.set(wy)
    Z_velocity_data.set(wz)
    Theta_data.set(theta)
    time_counter.set(time)
    X_displacement_data.after(100, update_state)
    

##control touch screen's switch
def update_text():
    if switch.value == 'off':
        confirm.disable()
        client.send(b'Host start')
    else:
        confirm.enable()
        client.send(b'Host stop')
        
##receive and classify message from host computer
def receiver(n,a):
    while 1:
        print('listen again')
        data = client.recv(1024).decode()
        print(data)
        if data == 'Pi stop':
            n.value = 0
        elif data == 'Pi start':
            n.value = 1
        elif data == 'Reset':
            send_reset()
            
        elif data[0] == 'R':
            data = data[1:]
            R_in = list(map(int,data.split(' ')))
            for x in range(len(R_in)):
                a[x]=R_in[x]

        else:
            print('Wrong command received',data)
            
##check shared memory variables to update switch
def update_switch():
    if num.value == 0:
        switch.set('off')
        confirm.disable()
    elif num.value == 1:
        switch.set('on')
        confirm.enable()    
    num.value = -1
   
    if arr[0] != -1:
        inx_box.set(arr[0])
        iny_box.set(arr[1])
        inr_box.set(arr[2])
        ins_box.set(arr[3])
        arr[0] = -1
        send_input()
        print('called send input')
        
    switch.after(1000, update_switch)

app = App(title = 'Main Window', layout = 'grid', width = 500, height = 200)

##input plane
input_plane = Text(app, text = 'Input', size = title_size, grid = [0,0,2,1])
inx_text = Text(app, text = 'x (cm)', size = text_size, grid = [0,1])
iny_text = Text(app, text = 'y (cm)', size = text_size, grid = [0,2])
inr_text = Text(app, text = 'Rotation (deg)', size = text_size, grid = [0,3])
ins_text = Text(app, text = 'Time (s)', size = text_size, grid = [0,4])

inx_box = TextBox(app, grid = [1,1], width = 5)
iny_box = TextBox(app, grid = [1,2], width = 5)
inr_box = TextBox(app, grid = [1,3], width = 5)
ins_box = TextBox(app, grid = [1,4], width = 5)

##confrim box, press for sending input to controller
confirm = PushButton(app, text = 'Ok', command = send_input, grid = [1,5],
                     align = 'right', height = 1, pady = 1)

reset = PushButton(app, text = 'Reset', command = send_reset, grid = [0,7,2,2])

##Display sensors
Sensor_data = Text(app, text = 'Sensors    data', size = title_size, grid = [2,0,2,1])
X_velocity = Text(app, text = 'velocity 0 (cm/s)', size = text_size, grid = [2,1])
Y_velocity = Text(app, text = 'velocity 1 (cm/s)', size = text_size, grid = [2,2])
Z_velocity = Text(app, text = 'velocity 2 (cm/s)', size = text_size, grid = [2,3])
X_displacement = Text(app, text = 'X displacement (cm)', size = text_size, grid = [2,4],width = 20)
Y_displacement = Text(app, text = 'Y displacement (cm)', size = text_size, grid = [2,5])
Theta = Text(app, text = 'Theta (deg)', size = text_size, grid = [2,6])
X_gesture = Text(app, text = 'X gesture', size = text_size, grid = [2,7])
Y_gesture = Text(app, text = 'Y gesture', size = text_size, grid = [2,8])



X_velocity_data = Text(app, text = '-', size = text_size, grid = [3,1], width = 8)
Y_velocity_data = Text(app, text = '-', size = text_size, grid = [3,2], width = 8)
Z_velocity_data = Text(app, text = '-', size = text_size, grid = [3,3], width = 8)
X_displacement_data = Text(app, text = '-', size = text_size, grid = [3,4], width = 8)
Y_displacement_data = Text(app, text = '-', size = text_size, grid = [3,5], width = 8)
Theta_data = Text(app, text = '-', size = text_size, grid = [3,6], width = 8)



#switch for other device control
swtich_title = Text(app, text = 'Rover control switch', grid = [4,0,2,1])
switch = ButtonGroup(app,options = [ 'on', 'off'],selected = 'on',command = update_text,
                     horizontal = 'Ture', grid = [4,1,2,1])

#camera window control
window_control = Text(app, text = 'Camera    Switch', grid = [4,3,2,1])
open_button = PushButton(app, text = 'Open', command = open_window, grid = [4,4], pady= 1)
close_button = PushButton(app, text = 'Close', command = close_window, grid = [5,4],pady = 1)

#time counter display
time_text = Text(app,text = 'Time spent', grid = [4,6])
time_counter = Text(app, text = '-', size = text_size, grid = [5,6])

if __name__ == "__main__":
    
    
    arr = Array('i', [-1,-1,-1,-1])
    num = Value('i', -1)
    
    current_value = Array('f', [0.0,0.0,0.0,0.0,0.0,0.0])
    flag = Value('i', -1)
    proce1 = Process(target = receiver, args = (num,arr))
    proce1.start()
    update_state()
    update_switch()
    plot(current_value,flag)
    app.display()

