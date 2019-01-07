from time import time, sleep
import serial
import rapi
from math import pi,sin,cos,sqrt,acos

resolution = 8
rover_radius = 16.0 # in cm
wheel_radius = 5.0 # in cm
constant_linearVelocity = 400*pi/(20*resolution) # in cm/s, 400 is the constant speed in step/s
constant_angularVelocity = pi/2.0*wheel_radius/rover_radius # in rad/s
time_frame = 1.0

class Rover:
	def __init__(self):
		self.x = 0.0
		self.y = 0.0
		self.phi = 0.0

	def update_coor(self, delta_x, delta_y, delta_phi):
		self.x = self.x + delta_x
		self.y = self.y + delta_y
		self.phi = self.phi + delta_phi

	def print_coor(self):
		print("Rover.x = ", self.x)
		print("Rover.y = ", self.y)
		print("Rover.phi = ", self.phi/pi*180)
		print()

	def reset_coor(self):
		self.x = 0
		self.y = 0
		self. phi = 0

def calculate_time_limit(rover, x, y, phi):
	global constant_linearVelocity
	global constant_angularVelocity

	dx = x - rover.x
	dy = y - rover.y
	dphi = phi - rover.phi
	if (dx != 0 or dy != 0):
		time_limit = float(sqrt(dx*dx + dy*dy)/constant_linearVelocity)
	elif (dphi != 0):
		time_limit = abs(dphi/constant_angularVelocity)
	elif (dx == 0 and dy == 0):
		time_limit = 1

	return time_limit

def calculate_angularVelocity(rover, trg_phi, time_limit):
    global constant_angularVelocity

    d_phi = trg_phi - rover.phi

    if time_limit != 0:
        if (abs(d_phi) <= (2.0/180.0)*pi):
            omega = 0
            time_to_rotate = -1
        else:
            omega = float(d_phi/time_limit)

            if (abs(omega) > constant_angularVelocity):
                omega = constant_angularVelocity*(omega/abs(omega))
                time_to_rotate = float(d_phi/omega)
            else:
                time_to_rotate = -1.0
    else:
        omega = 0
        time_to_rotate = -1.0

    return omega, time_to_rotate

def calculate_linearVelocity1(rover, x, y):
	global constant_linearVelocity

	d_x = x - rover.x
	d_y = y - rover.y

	print("d_x: " + str(d_x) + ", d_y: " + str(d_y))

	if (d_x >= 0.5 or d_y >= 0.5):
		angle = float(acos(d_x/sqrt(d_x*d_x + d_y*d_y)))
		print("angle: ", angle)

		if (d_x != 0):
			v_x = float(constant_linearVelocity*cos(angle))
		else:
			v_x = 0
		if (d_y != 0):
			v_y = float(constant_linearVelocity*sin(angle))*(d_y/abs(d_y))
		else:
			v_y = 0
	else:
		v_x = 0
		v_y = 0

	return v_x, v_y

def calculate_linearVelocity2(v_x, v_y, phi):
	v = float(float(v_x*cos(phi)) + float(v_y*sin(phi)))

	v_n = float(float(v_x*(-sin(phi))) + float(v_y*cos(phi)))

	return v, v_n

def calculate_wheelVelocity(v, v_n, omega):
	global rover_radius

	v_0 = float(float(-sqrt(3)/2*v) + float(1/2*v_n) + float(omega*rover_radius))
	v_1 = float(float(-v_n) + float(omega*rover_radius))
	v_2 = float(float(sqrt(3)/2*v) + float(1/2*v_n) + float(omega*rover_radius))

	return v_0, v_1, v_2

def main():
	ser = serial.Serial("/dev/ttyS0", 9600)
	rover = Rover()

	while True:
		# getting input
		cont = int(input("Insert 1 token to continue: "))
		if (cont != 1):
			break

		global constant_linearVelocity
		global time_frame

		constant_linearVelocity = float(input("constant_linearVelocity (in step/s): "))
		constant_linearVelocity = constant_linearVelocity*pi/(20*resolution)
		time_frame = float(input("time_frame: "))
		trg_x = float(input("X (in cm): "))
		trg_y = float(input("Y (in cm): "))
		trg_phi = float(input("phi (in degree): "))
		trg_phi = (trg_phi/180)*pi # convert back to radius
		print()

		# calculate required variable
		time_limit = calculate_time_limit(rover, trg_x, trg_y, trg_phi)
		print("Time limit is: ", time_limit, sep = " ")

		v_x, v_y = calculate_linearVelocity1(rover, trg_x, trg_y)
		print("v_x:", v_x, ", v_y:", v_y, sep = " ")

		iteration = int(time_limit/time_frame)
		print("number of iteration:", iteration, sep = " ")
		last_iteration_duration1 = float(time_limit - iteration*time_frame)
		print("last_iteration_duration1:", last_iteration_duration1, sep=" ")

		omega, time_to_rotate = calculate_angularVelocity(rover, trg_phi, time_limit)

		if (time_to_rotate == -1.0):
			time_to_rotate = time_limit
			last_iteration_duration2 = last_iteration_duration1
		else:
			last_iteration_duration2= float(time_to_rotate - iteration*time_frame)

		print("omega (in degree/s) is: ", omega/pi*180, sep = " ")
		print("last_iteration_duration2:", last_iteration_duration2, sep=" ")


		input("Press enter to continue.")
		print("Start running.")
		print()
		rover.print_coor()

		flag = 1
		for i in range(iteration):
			print("iteration number: ", i)
			
			if (v_x != 0 or v_y != 0):
				# run straight
				v, v_n = calculate_linearVelocity2(v_x, v_y, rover.phi)
				v_0, v_1, v_2 = calculate_wheelVelocity(v, v_n, omega)

				v_0 = v_0*(20*resolution/pi)
				v_1 = v_1*(20*resolution/pi)
				v_2 = v_2*(20*resolution/pi)
				rapi.send_spd(ser, v_0, v_1, v_2, 0)

				print("Going straight: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)

				# update coordinate
				rover.update_coor(v_x*time_frame, v_y*time_frame, 0)
				rover.print_coor()
				# delay for 1 time frame
				sleep(time_frame)

			if (omega != 0 ):
				# turning
				v_0, v_1, v_2 = calculate_wheelVelocity(0, 0, omega)

				v_0 = v_0*(20*resolution/pi)
				v_1 = v_1*(20*resolution/pi)
				v_2 = v_2*(20*resolution/pi)

				rapi.send_spd(ser, v_0, v_1, v_2, 0)

				print("Turning: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)

				# update coordinate
				rover.update_coor(0, 0, omega*time_frame)
				rover.print_coor()
				# delay for 1 time frame
				sleep(time_frame)

		# left over distance 

		# going straight
		print()
		print("Left over distance:")
		v, v_n = calculate_linearVelocity2(v_x, v_y, rover.phi)
		v_0, v_1, v_2 = calculate_wheelVelocity(v, v_n, omega)

		v_0 = v_0*(20*resolution/pi)
		v_1 = v_1*(20*resolution/pi)
		v_2 = v_2*(20*resolution/pi)

		rapi.send_spd(ser, v_0, v_1, v_2, 0)

		# delay for last_iteration_duration1
		sleep(last_iteration_duration1)

		print("Going straight: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)

		if (omega != 0 ):
			# turning

			rover.update_coor(v_x*last_iteration_duration1, v_y*last_iteration_duration1, 0)	
			rover.print_coor()

			v_0, v_1, v_2 = calculate_wheelVelocity(0, 0, omega)

			v_0 = v_0*(20*resolution/pi)
			v_1 = v_1*(20*resolution/pi)
			v_2 = v_2*(20*resolution/pi)

			rapi.send_spd(ser, v_0, v_1, v_2, 0)

			rover.update_coor(0, 0, omega*last_iteration_duration2)
			print("Turning: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)
			rover.print_coor()

			# delay for last_iteration_duration1
			sleep(last_iteration_duration2)

		# reset the state of the rover back to (0, 0, 0) for easy debugging.
		rapi.send_spd(ser, 0, 0, 0, 1)
		rover.reset_coor()

if __name__ == '__main__':
	main()
