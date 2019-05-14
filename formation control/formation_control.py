import socket
import time
from math import sqrt
import numpy as np
from copy import deepcopy

def coor2bytes(coor_fnc):
	coorByte = [0, 0, 0, 0, 0, 0]

	if (coor_fnc[0] >= 0):
		coorByte[0] = (coor_fnc[0] >> 8) & 0xFF # High byte of X
		coorByte[1] = coor_fnc[0] & 0xFF # Low byte of X
	else:
		coor_fnc[0] = coor_fnc[0]*(-1)
		coorByte[0] = (coor_fnc[0] >> 8) & 0xFF # High byte of X
		coorByte[0] = coorByte[0] ^ 0x80
		coorByte[1] = coor_fnc[0] & 0xFF # Low byte of X
	
	if (coor_fnc[1] >= 0):
		coorByte[2] = (coor_fnc[1] >> 8) & 0xFF # High byte of Y
		coorByte[3] = coor_fnc[1] & 0xFF # Low byte of Y
	else:
		coor_fnc[1] = coor_fnc[1]*(-1)
		coorByte[2] = (coor_fnc[1] >> 8) & 0xFF # High byte of X
		coorByte[2] = coorByte[2] ^ 0x80
		coorByte[3] = coor_fnc[1] & 0xFF # Low byte of X
		
	if (coor_fnc[2] >= 0):
		coorByte[4] = (coor_fnc[2] >> 8) & 0xFF # High byte of Phi
		coorByte[5] = coor_fnc[2] & 0xFF # Low byte of Phi
	else:
		coor_fnc[2] = coor_fnc[2]*(-1)
		coorByte[4] = (coor_fnc[2] >> 8) & 0xFF # High byte of Phi
		coorByte[4] = coorByte[4] ^ 0x80
		coorByte[5] = coor_fnc[2] & 0xFF # Low byte of Phi

	return coorByte

def bytes2coor(byte_fnc):
	receivedCoor_fnc = [0, 0, 0]

	receivedCoor_fnc[0] = ((-1)**(byte_fnc[0]>>7)) * ((byte_fnc[1]) | (((byte_fnc[0]&0x7f)<<8)))
	receivedCoor_fnc[1] = ((-1)**(byte_fnc[2]>>7)) * ((byte_fnc[3]) | (((byte_fnc[2]&0x7f)<<8)))
	receivedCoor_fnc[2] = ((-1)**(byte_fnc[4]>>7)) * ((byte_fnc[5]) | (((byte_fnc[4]&0x7f)<<8)))

	return receivedCoor_fnc


class rover:

	def __int__(self):
		self.currentCoor = [0, 0, 0]
		self.currentTarget = [0, 0, 0]
		self.targetList = []
		self.id = -1
		self.arrived = 0

	def __init__(self, socket):
		self.currentCoor = [0, 0, 0]
		self.currentTarget = [0, 0, 0]
		self.targetList = []
		self.id = -1
		self.arrived = 0

		self.socket = socket

	def setCurrentCoor(self, coordinate):
		self.currentCoor = coordinate

	def setCurrentTarget(self, newTarget):
		self.currentTarget = newTarget

	def setTargetList(self, target):
		self.targetList.append(target)

	def setID(self, identification):
		self.id = identification

	def setSocket(self, newSocket):
		self.socket = newSocket

if __name__ == '__main__':

	########################################### Setting up communication ########################################
	number_of_rover = int(input("Number of rover: "))
	# id_max = int(input("Maximum number of identification: "))
	# scaling_factor = int(input("Scaling factor for target: "))
	id_max = 10
	scaling_factor = 1

	# setup server
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind((socket.gethostname(), 1234)) # bind(ip, port)
	print("Done binding, now listening for connection.")
	print()
	s.listen(0)

	roverPlatform = []

	# connect client to server
	for i in range(number_of_rover):
		# Listen to the client's connection
		clientSocket, address = s.accept()
		print(f"Connection number {i} from {address} has been established!")
		clientSocket.settimeout(0.5)

		roverPlatform.append(rover(clientSocket))

		# Ask for ID
		print("Asking for identification.")
		roverPlatform[i].socket.send(bytes([4]))

		while (True):
			try:
				roverID = roverPlatform[i].socket.recv(8)
				roverID = int.from_bytes(roverID, "little")
				roverPlatform[i].id = roverID
				print("The ID is: " + str(roverPlatform[i].id))
				print()
				break
			except socket.timeout:
				pass

	########################################### Read input text file ###########################################
	input_file = open("testMatrix3.txt","r")
	

	input_matrix = [] 
	while True:
		input_line = input_file.readline()
		if len(input_line) == 0:
			break
		input_line = input_line.rstrip("\n")
		input_matrix.append(input_line.split(","))

	for i in range(len(input_matrix)): # iterate through all the target coordinate in the text file
		for j in range(number_of_rover): # iterate through all the rover platform
			for k in range(id_max): # iterate through all the possible ID.
				if (roverPlatform[j].id == k): # the target of a rover will be located at column id (x) and column id+1 (y)
					roverPlatform[j].setTargetList([int(float(input_matrix[i][k*2])*scaling_factor), int(float(input_matrix[i][k*2+1])*scaling_factor), 0])

	########################################### Set the starting point ###########################################
	for i in range(number_of_rover):
		print(f"Rover with ID {roverPlatform[i].id}'s' starting point is: ")
		setStartingPoint = [0, 0, 0]
		while (True):
			try:
				setStartingPoint[0] = int(input("X: "))
				setStartingPoint[1] = int(input("y: "))
				# setStartingPoint[2] = int(input("phi: "))
				setStartingPoint[2] = 0

				setStartingPointByte = coor2bytes(setStartingPoint)

				roverPlatform[i].socket.send(bytes([2]))
				roverPlatform[i].socket.send(bytes(setStartingPointByte))

				print("I already sent the new starting coordinate.")
				break

			except Exception:
					print("Error.")

		print(f"Asking rover with ID {roverPlatform[i].id} current position.") # double check
		while (True):
				try:
					roverPlatform[i].socket.send(bytes([1]))
					bytesReceived = []
					full_msg = []

					while (len(full_msg) < 8):
						bytesReceived = roverPlatform[i].socket.recv(8)
						for x in range(len(bytesReceived)):
							full_msg.append(bytesReceived[x])
					
					receivedCoor = bytes2coor(full_msg)
					roverPlatform[i].setCurrentCoor(receivedCoor)
					break

				except socket.timeout:
					pass


		print(f"Rover with ID {roverPlatform[i].id} current position is: {roverPlatform[i].currentCoor}")
		print()

	########################################### Start sending target ###########################################

	# send first target
	for i in range(number_of_rover):
		roverPlatform[i].currentTarget = roverPlatform[i].targetList[0]
		targetSend = deepcopy(roverPlatform[i].currentTarget)
		coorByte = coor2bytes(targetSend)
		roverPlatform[i].socket.send(bytes([0]))
		roverPlatform[i].socket.send(bytes(coorByte))
		print(f"I sent the first target: {roverPlatform[i].currentTarget} to rover with id: {roverPlatform[i].id}")

	# Send the rest of target point	
	for n in range(0,len(input_matrix)): # send target from target[1] to final target
		print()
		arrival = 0 # initally, none of the rover arrived at the target yet
		for i in range(number_of_rover):
			roverPlatform[i].arrived = 0

		start = time.time()
		while (arrival < number_of_rover): # loop continue until all rover arrive at final destination
			# check position in order to send the next target.
			elapsedTime = time.time() - start
			if (elapsedTime > 0.5): # check if arrive every 100ms
				start = time.time()
				output_file = open("liveCoordinate.txt", "w")
				
				for i in range(number_of_rover): # do same stuffs with each of the rover
					# ask for current coordinate
					roverPlatform[i].socket.send(bytes([1]))
					bytesReceived = []
					full_msg = []

					while (len(full_msg) < 8):
						try:
							bytesReceived = roverPlatform[i].socket.recv(8)
							for j in range(len(bytesReceived)):
								full_msg.append(bytesReceived[j])

						except socket.timeout:
							pass

					receivedCoor = bytes2coor(full_msg)
					roverPlatform[i].currentCoor = receivedCoor

					if ((abs(roverPlatform[i].currentTarget[0] - roverPlatform[i].currentCoor[0]) <= 2) and (abs(roverPlatform[i].currentTarget[1] - roverPlatform[i].currentCoor[1]) <= 2) and (roverPlatform[i].arrived == 0)):
						# send the next target
						roverPlatform[i].arrived = 1
						arrival = arrival + 1

					if (i < (number_of_rover-1)):
						output_file.write(f"{roverPlatform[i].currentCoor[0]},{roverPlatform[i].currentCoor[1]},")
					else:
						output_file.write(f"{roverPlatform[i].currentCoor[0]},{roverPlatform[i].currentCoor[1]}")
				
				output_file.write("\n")
				output_file.close()

		# After all arrived, send the next batch of coordinate.
		for i in range(number_of_rover):
			roverPlatform[i].targetList[n][2] = roverPlatform[i].currentCoor[2]
			roverPlatform[i].currentTarget = roverPlatform[i].targetList[n]
			targetSend = deepcopy(roverPlatform[i].currentTarget)
			coorByte = coor2bytes(targetSend)
			roverPlatform[i].socket.send(bytes([0]))
			roverPlatform[i].socket.send(bytes(coorByte))
			print(f"I sent the next target: {roverPlatform[i].currentTarget} to rover with id: {roverPlatform[i].id}")


	print("Formation has been formed.")

	
