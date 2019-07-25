import numpy as np
from numpy import matrix
from numpy.linalg import svd
from numpy.linalg import norm as norm_np
from math import pi, atan, sqrt
import cvxpy as cp
from cvxpy.atoms.lambda_min import lambda_min
from cvxpy.atoms.norm import norm
from copy import deepcopy
import socket
import time

from collision_avoidance import dynamic_collision_avoicedance

def unit_vector(vector):
    return vector / np.linalg.norm_np(vector)

def angle_between(v1, v2):

    v1_u = unit_vector(v1)
    v2_u = unit_vector(v2)
    return np.arccos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

def find_dij_star(destination_fnc, n_agent):
    dij_star_fnc = np.zeros((n_agent, n_agent))
    for i in range(n_agent): # Go through all the agents
        for j in range(n_agent): # Go through all the neighbors
            agent = np.array([destination_fnc[i*2], destination_fnc[i*2+1]])
            neighbor = np.array([destination_fnc[j*2], destination_fnc[j*2+1]])
            dij_star_fnc[i][j] = norm_np(np.array([destination_fnc[i*2], destination_fnc[i*2+1]]) - np.array([destination_fnc[j*2], destination_fnc[j*2+1]]))

    return dij_star_fnc

def scale_adjustment_fnc(qi, qj, dij_star):
	k = 10
	dij = norm_np(qj-qi)
	# print(f"dij - dij_star: {dij - dij_star}.")
	push_pull_fnc = 1/k*atan(dij - dij_star)

	return push_pull_fnc*(qj - qi)

def rotate_coordinate(xy, radians):
	"""Use numpy to build a rotation matrix and take the dot product."""
	x, y = xy
	c, s = np.cos(radians), np.sin(radians)
	j = np.matrix([[-c, -s], [s, -c]])
	m = np.dot(j, [x, y])

	return float(m.T[0]), float(m.T[1])

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

	def clearTargetList(self):
		self.targetList = []

	def setID(self, identification):
		self.id = identification

	def setSocket(self, newSocket):
		self.socket = newSocket

if __name__ == '__main__':
	########################################### Setting up communication ########################################
	number_of_agent = int(input("Number of agent: "))

	id_max = 10
	scaling_factor = 1
	rate_of_checking = 0.1

	# setup server
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind((socket.gethostname(), 1234)) # bind(ip, port)
	print("Done binding, now listening for connection.")
	print()
	s.listen(5)

	roverPlatform = []

	# connect client to server
	for i in range(number_of_agent):
		# Listen to the client's connection
		clientSocket, address = s.accept()
		print(f"Connection number {i} from {address} has been established!")
		clientSocket.settimeout(1)

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

	initial_position = np.zeros((number_of_agent, 2))

	######################## Choose the formation ########################
	init_choice = 0
	while (init_choice != -1):
		# Clear all current target list before doing anything else.
		for i in range(number_of_agent):
			roverPlatform[i].clearTargetList()

		# Choose the path
		print("\nWhat file you want to run now?")
		print(" 1. [triangle]")
		print(" 2. [hexagon]")
		print(" 3. [tank]")
		print(" 4. [2_rows]")
		print(" 5. [3_stick]")
		print("-1. Exit.")
		init_choice = int(input("I choose: "))

		if (init_choice == -1):
			break

		# Setup initial position
		print("\nDo you want to setup initial position?")
		print("1 for yes, 0 for no.")
		init_position = int(input("I choose: "))
		if (init_position == 0):
			print("Using the current coordinate.")

		elif (init_position == 1):
			print("Setting up new position.")
			########################################### Set the starting point ###########################################
			for i in range(number_of_agent):
				print(f"Rover with ID {roverPlatform[i].id}'s' starting point is: ")
				setStartingPoint = [0, 0, 0]
				while (True):
					try:
						setStartingPoint[0] = int(input("X: "))
						setStartingPoint[1] = int(input("Y: "))
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
				initial_position[i] = [roverPlatform[i].currentCoor[0], roverPlatform[i].currentCoor[1]]
				print()

		if (init_choice == 1): # triangle
			destination = np.array([1, 0, 0, 0, -1, 0, -0.5, 0.5, 0, 1, 0.5, 0.5])*100
		elif (init_choice == 2): # hexagon
			destination = np.array([1, 0, 0.5, -1, -0.5, -1, -1, 0, -0.5, 1, 0.5, 1])*60
		elif (init_choice == 3): # tank
			destination = np.array([0.5, -0.5, 0, -1, -0.5, -0.5, -0.5, 0.5, 0, 0, 0.5, 0.5])*100
		elif (init_choice == 4): # 2_row
			destination = np.array([0.5, -0.5, 0, -0.5, -0.5, -0.5, -0.5, 0.5, 0, 0.5, 0.5, 0.5])*100
		elif (init_choice == 5): # 3_stick
			destination = np.array([1, -1, 0.5, -0.5, -1, -1, -0.5, -0.5, 0, 0.5, 0, 1])*100

		############################### Find gain A #################################
		print(f"Initial position: {initial_position}.")
		# print(f"Shape of initial_position: {initial_position.shape}.")

		# print(f"destination: {destination}.")
		destination_bar = np.zeros(len(destination))
		for i in range(0,len(destination),2):
			destination_bar[i], destination_bar[i+1] = rotate_coordinate((destination[i], destination[i+1]), pi/2)
			destination_bar[i] = destination_bar[i]
			destination_bar[i+1] = destination_bar[i+1]

		dij_star = find_dij_star(destination, number_of_agent)

		# print(f"\n\ndestination: {destination}.")
		# print(f"rotated destination: {destination_bar}.")	

		vector_one = np.ones(len(destination))
		vector_one_bar = np.ones(len(destination))
		for i in range(0,len(vector_one_bar),2):
			vector_one_bar[i] = -1

		# print(f"\n\nvector_one: {vector_one}.")
		# print(f"vector_one_bar: {vector_one_bar}.")

		# vector_N = np.concatenate((vector_one, vector_one_bar, destination, destination_bar), axis = 1)
		N = np.array([vector_one, vector_one_bar, destination, destination_bar]).transpose()
		# print(f"\n\nMatrix N: {N}.")
		# print(f"Shape of N: {N.shape}.")

		# Solve singular value decomposition (SVD) of N
		U,_,_ = svd(N)
		U = np.array(U)
		# print(f"\n\nMatrix U: {U}")
		# print(f"Shape of U: {U.shape}.")

		Q = deepcopy(U[:,4:len(destination)])
		# print(f"\n\nMatrix Q: {Q}")
		# print(f"Shape of Q: {Q.shape}.")

		# Solve SDP problem
		A = cp.Variable(shape = (len(destination), len(destination)))
		A_bar = Q.transpose()*A*Q

		# print(f"\n\nShape of A: {A.shape}.")
		# print(f"Shape of A_bar: {A_bar.shape}.")

		objective = cp.Maximize(lambda_min(-A_bar))
		constraint = [A*N == 0,
					  norm(A) <= 10]

		problem = cp.Problem(objective, constraint)
		problem.solve()

		gain_A = np.array(A.value).round(5)
		# gain_A = np.matrix(A.value)

		# print(f"Status: {problem.status}. \n")
		# print(f"Optimal value: {problem.value}. \n")
		# print(f"Optimal variable: \n{gain_A}. \n")

		# print(f"Shape of gain_A: {gain_A.shape}")

		gain_Ai = np.zeros((number_of_agent,number_of_agent,2,2)) # I try putting everything nice and neat
																  # A will equal to Ai later on

		for i in range(0,gain_A.shape[0],2): # go through all agents
			for j in range(0,gain_A.shape[1],2): # go through all neighbor
				gain_Aij = np.array([[gain_A[i][j], gain_A[i][j+1]],[gain_A[i+1][j], gain_A[i+1][j+1]]])
				gain_Ai[int(i/2),int(j/2)] = gain_Aij

		gain_A = gain_Ai # The final gain.

		# print(f"Gain A: \n{gain_A}\n")
		# print(f"Shape of gain A: {gain_A.shape}")

		############################# Calculate new destination for each agent #############################
		time_interval = 0.05
		u = np.zeros((number_of_agent,2))
		print(f"u: {u}.\n\n")
		current_position = deepcopy(initial_position)
		next_position = deepcopy(current_position)

		for iteration in range(10000): # iterate for n times
			print(f"\n\n\nIteration number: {iteration}.")
			for i in range(number_of_agent): # calculate new position of all agents
				# Ask for the current position of the current agent
				for j in range(number_of_agent): # do same stuffs with each of the rover
					# ask for current coordinate
					roverPlatform[j].socket.send(bytes([1]))
					bytesReceived = []
					full_msg = []

					while (len(full_msg) < 8):
						try:
							bytesReceived = roverPlatform[j].socket.recv(8)
							for k in range(len(bytesReceived)):
								full_msg.append(bytesReceived[k])

						except socket.timeout:
							print(f"Socket timed out on rover {roverPlatform[j].id}.")
							pass

					receivedCoor = bytes2coor(full_msg)
					roverPlatform[j].currentCoor = receivedCoor

					# update the current position to calculate next position in the next step
					current_position[j] = [roverPlatform[j].currentCoor[0], roverPlatform[j].currentCoor[1]]
				
				# Calculate the control vector for the current agent
				u[i] = np.zeros(2)
				
				for j in range(number_of_agent): 
					u[i] = u[i] + np.dot(gain_A[i][j],current_position[j].transpose()) + scale_adjustment_fnc(current_position[i].transpose(), current_position[j].transpose(), dij_star[i][j])
				
				# print(f"Control vector of agent {i} before collision avoidance is {u[i]}.")
				# print(f"Current position of agent {i} before this iteration: {current_position[i]}")
				u[i] = dynamic_collision_avoicedance(current_position, i, u[i], number_of_agent)
				# print(f"Control vector of agent {i} after collision avoidance is {u[i]}.")	
				next_position[i] = current_position[i] + u[i]*time_interval
				roverPlatform[i].currentTarget = [int(next_position[i,0]), int(next_position[i,1]), 0]
				# print(f"Current position of agent {i} after this iteration: {current_position[i]}\n")

			print(f"Current position: \n{current_position}")

			for i in range(number_of_agent): # send new set of coordinate to all agents
				targetSend = deepcopy(roverPlatform[i].currentTarget)
				coorByte = coor2bytes(targetSend)
				roverPlatform[i].socket.send(bytes([0]))
				roverPlatform[i].socket.send(bytes(coorByte))
				print(f"I sent the next target: {roverPlatform[i].currentTarget} to rover with id: {roverPlatform[i].id}")

			time.sleep(time_interval)

			# if np.array_equal(current_position.astype(int), next_position.astype(int)):
			# 	print("Formation is formed.")
			# 	break	
			if np.all(abs(current_position-next_position) < 3):
				print("Formation is formed (or deadlock).")
				break
