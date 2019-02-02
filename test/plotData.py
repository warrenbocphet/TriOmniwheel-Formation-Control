import matplotlib.pyplot as plt
import numpy as np

time_frame = "25"
# read data from txt file
path0 = time_frame + "/output00.txt"
path1 = time_frame + "/output01.txt"
path2 = time_frame + "/output02.txt"

path3 = time_frame + "/output1.txt"

input_file0 = open(path0, 'r')
input_file1 = open(path1, 'r')
input_file2 = open(path2, 'r')

input_file3 = open(path3, 'r')

sensor_data0 = input_file0.readlines() # data after going through complimentary filter
sensor_data1 = input_file1.readlines()
sensor_data2 = input_file2.readlines()

no_of_data = input_file3.readlines()

for i in range(len(sensor_data0)):
	sensor_data0[i] = float(sensor_data0[i].strip())

for i in range(len(sensor_data1)):
	sensor_data1[i] = float(sensor_data1[i].strip())

for i in range(len(sensor_data0)):
	sensor_data2[i] = float(sensor_data2[i].strip())

for i in range(len(no_of_data)):
	no_of_data[i] = int(no_of_data[i].strip())

# plotting

# Wheel 0
speed00 = [0]*no_of_data[0]
speed01 = [100]*no_of_data[1]
speed02 = [200]*no_of_data[2]
speed03 = [300]*no_of_data[3]
speed04 = [400]*no_of_data[4]
speed05 = [500]*no_of_data[5]
speed06 = [600]*no_of_data[6]
speed07 = [700]*no_of_data[7]

setSpeed0 = speed00 + speed01 + speed02 + speed03 + speed04 + speed05 + speed06 + speed07

# Wheel 1
speed10 = [0]*no_of_data[0]
speed11 = [-100]*no_of_data[1]
speed12 = [-200]*no_of_data[2]
speed13 = [-300]*no_of_data[3]
speed14 = [-400]*no_of_data[4]
speed15 = [-500]*no_of_data[5]
speed16 = [-600]*no_of_data[6]
speed17 = [-700]*no_of_data[7]

setSpeed1 = speed10 + speed11 + speed12 + speed13 + speed14 + speed15 + speed16 + speed17

# Wheel 2
speed20 = [0]*no_of_data[0]
speed21 = [-100]*no_of_data[1]
speed22 = [200]*no_of_data[2]
speed23 = [-300]*no_of_data[3]
speed24 = [400]*no_of_data[4]
speed25 = [-500]*no_of_data[5]
speed26 = [600]*no_of_data[6]
speed27 = [-700]*no_of_data[7]

setSpeed2 = speed20 + speed21 + speed22 + speed23 + speed24 + speed25 + speed26 + speed27


# Kalman filter
n_iter = len(sensor_data0)
sz = (n_iter,)
z0 = sensor_data0
z1 = sensor_data1
z2 = sensor_data2

Q = 1e-4
R = 0.1**2

# Wheel 0
xhat0=np.zeros(sz)      # a posteri estimate of x
P0=np.zeros(sz)         # a posteri error estimate
xhatminus0=np.zeros(sz) # a priori estimate of x
Pminus0=np.zeros(sz)    # a priori error estimate
K0=np.zeros(sz)         # gain or blending factor

for k in range(1,n_iter):
    # time update
    xhatminus0[k] = xhat0[k-1]
    Pminus0[k] = P0[k-1]+Q

    # measurement update
    K0[k] = Pminus0[k]/( Pminus0[k]+R )
    xhat0[k] = xhatminus0[k]+K0[k]*(z0[k]-xhatminus0[k])
    P0[k] = (1-K0[k])*Pminus0[k]


# Wheel 1
xhat1=np.zeros(sz)      # a posteri estimate of x
P1=np.zeros(sz)         # a posteri error estimate
xhatminus1=np.zeros(sz) # a priori estimate of x
Pminus1=np.zeros(sz)    # a priori error estimate
K1=np.zeros(sz)         # gain or blending factor

for k in range(1,n_iter):
    # time update
    xhatminus1[k] = xhat1[k-1]
    Pminus1[k] = P1[k-1]+Q

    # measurement update
    K1[k] = Pminus1[k]/( Pminus1[k]+R )
    xhat1[k] = xhatminus1[k]+K1[k]*(z1[k]-xhatminus1[k])
    P1[k] = (1-K1[k])*Pminus1[k]


# Wheel 2
xhat2=np.zeros(sz)      # a posteri estimate of x
P2=np.zeros(sz)         # a posteri error estimate
xhatminus2=np.zeros(sz) # a priori estimate of x
Pminus2=np.zeros(sz)    # a priori error estimate
K2=np.zeros(sz)         # gain or blending factor

for k in range(1,n_iter):
    # time update
    xhatminus2[k] = xhat2[k-1]
    Pminus2[k] = P2[k-1]+Q

    # measurement update
    K2[k] = Pminus2[k]/( Pminus2[k]+R )
    xhat2[k] = xhatminus2[k]+K2[k]*(z2[k]-xhatminus2[k])
    P2[k] = (1-K2[k])*Pminus2[k]

# allocate space for arrays
# xhat=np.zeros(sz)      # a posteri estimate of x
# P=np.zeros(sz)         # a posteri error estimate
# xhatminus=np.zeros(sz) # a priori estimate of x
# Pminus=np.zeros(sz)    # a priori error estimate
# K=np.zeros(sz)         # gain or blending factor

# for k in range(1,n_iter):
#     # time update
#     xhatminus[k] = xhat[k-1]
#     Pminus[k] = P[k-1]+Q

#     # measurement update
#     K[k] = Pminus[k]/( Pminus[k]+R )
#     xhat[k] = xhatminus[k]+K[k]*(z[k]-xhatminus[k])
#     P[k] = (1-K[k])*Pminus[k]

# plt.rcParams['figure.figsize'] = (10, 8)
# plt.figure()

# plt.plot(sensor_data,'k+',label='complimentary filter')
# plt.plot(xhat,'k+',label='kalman filter + complimentary filter', color = 'red')
# plt.plot(setSpeed,'k+',label='set speed (real value)', color = 'blue')
# plt.title('Estimate vs. iteration step', fontweight='bold')
# plt.xlabel('No. of measurements')
# plt.ylabel('Step/s')
# plt.legend()
# plt.show()

plt.rcParams['figure.figsize'] = (10, 8)


# Plot wheel 0
# plt.subplot(221)
plt.figure()
plt.title("Wheel 0", fontsize = 16)
plt.plot(sensor_data0, 'k+', label = 'Wheel 0 data after complimentary filter')
plt.plot(xhat0, 'k+', label = 'Wheel 0 data after Kalman and complimentary filter', color = 'red')
plt.plot(setSpeed0,'k+',label='set speed (expected value)', color = 'blue')
plt.xlabel('No. of measurements')
plt.ylabel('Step/s')
plt.legend()

plt.figure()
# plt.subplot(222)
plt.title("Wheel 1", fontsize = 16)
plt.plot(sensor_data1, 'k+', label = 'Wheel 1 data after complimentary filter')
plt.plot(xhat1, 'k+', label = 'Wheel 1 data after Kalman and complimentary filter', color = 'red')
plt.plot(setSpeed1,'k+',label='set speed (expected value)', color = 'blue')
plt.xlabel('No. of measurements')
plt.ylabel('Step/s')
plt.legend()

# plt.subplot(223)
plt.figure()
plt.title("Wheel 2", fontsize = 16)
plt.plot(sensor_data2, 'k+', label = 'Wheel 2 data after complimentary filter')
plt.plot(xhat2, 'k+', label = 'Wheel 2 data after Kalman and complimentary filter', color = 'red')
plt.plot(setSpeed2,'k+',label='set speed (expected value)', color = 'blue')
plt.xlabel('No. of measurements')
plt.ylabel('Step/s')
plt.legend()

plt.show()