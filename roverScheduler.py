from time import time, sleep
import serial
import rapi0
from math import pi,sin,cos,sqrt,acos

resolution = 8
constant_linearVelocity = 50 # in cm/s, 300 is the constant speed in step/s
rover_radius = 16 # in cm
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

def calculate_time_limit(rover, x, y):
	global constant_linearVelocity
	dx = x - rover.x
	dy = y - rover.y
	time_limit = float(sqrt(dx*dx + dy*dy)/constant_linearVelocity)

	return time_limit

def calculate_angularVelocity(rover, trg_phi, time_limit):
	if time_limit != 0:
		omega = float((trg_phi - rover.phi)/time_limit)
	else:
		omega = 0

	return omega

def calculate_linearVelocity1(rover, x, y):
	global constant_linearVelocity

	d_x = x - rover.x
	d_y = y - rover.y
	if (d_x != 0 or d_y != 0):
		angle = float(acos(d_x/sqrt(d_x*d_x + d_y*d_y)))

		v_x = float(constant_linearVelocity*cos(angle))

		v_y = float(constant_linearVelocity*sin(angle))
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
		cont = int(input("Insert 1 token to continue: "))
		if (cont != 1):
			break

		global constant_linearVelocity
		global time_frame

		constant_linearVelocity = float(input("constant_linearVelocity: "))
		time_frame = float(input("time_frame: "))
		trg_x = float(input("X (in cm): "))
		trg_y = float(input("Y (in cm): "))
		trg_phi = float(input("phi (in degree): "))
		trg_phi = (trg_phi/180)*pi # convert back to radius
		print()

		time_limit = calculate_time_limit(rover, trg_x, trg_y)
		print("Time limit is: ", time_limit, sep = " ")

		v_x, v_y = calculate_linearVelocity1(rover, trg_x, trg_y)
		print("v_x:", v_x, ", v_y:", v_y, sep = " ")

		iteration = int(time_limit/time_frame)
		print("number of iteration:", iteration, sep = " ")
		last_iter_duration = float(time_limit - iteration*time_frame)
		print("last_iter_duration:", last_iter_duration, sep=" ")

		omega = calculate_angularVelocity(rover, trg_phi, time_limit)
		print("omega (in degree/s) is: ", omega/pi*180, sep = " ")

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
				rapi0.send_spd(ser, v_0, v_1, v_2, 0)

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

				rapi0.send_spd(ser, v_0, v_1, v_2, 0)

				print("Turning: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)

				# update coordinate
				rover.update_coor(0, 0, omega*time_frame)
				rover.print_coor()
				# delay for 1 time frame
				sleep(time_frame)

		# left over distance 

		if (last_iter_duration != 0):
			# going straight
			print()
			print("Left over distance:")
			v, v_n = calculate_linearVelocity2(v_x, v_y, rover.phi)
			v_0, v_1, v_2 = calculate_wheelVelocity(v, v_n, omega)

			v_0 = v_0*(20*resolution/pi)
			v_1 = v_1*(20*resolution/pi)
			v_2 = v_2*(20*resolution/pi)

			rapi0.send_spd(ser, v_0, v_1, v_2, 0)

			# delay for last_iter_duration
			sleep(last_iter_duration)

			print("Going straight: v_0: ", v_0, ", v_1: ", v_1, ", v_2: ", v_2)

			if (omega != 0 ):
				# turning

				rover.update_coor(v_x*last_iter_duration, v_y*last_iter_duration, 0)	
				rover.print_coor()

				v_0, v_1, v_2 = calculate_wheelVelocity(0, 0, omega)

				v_0 = v_0*(20*resolution/pi)
				v_1 = v_1*(20*resolution/pi)
				v_2 = v_2*(20*resolution/pi)

				rapi0.send_spd(ser, v_0, v_1, v_2, 0)

				rover.update_coor(0, 0, omega*last_iter_duration)
				rover.print_coor()

				# delay for last_iter_duration
				sleep(last_iter_duration)



		# reset the state of the rover back to (0, 0, 0) for easy debugging.
		rapi0.send_spd(ser, 0, 0, 0, 1)
		rover.reset_coor()

if __name__ == '__main__':
	main()