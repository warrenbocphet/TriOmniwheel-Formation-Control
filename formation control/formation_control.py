import numpy as np
from numpy import matrix
from numpy.linalg import svd
from numpy.linalg import norm as norm_np
from math import pi, atan
import cvxpy as cp
from cvxpy.atoms.lambda_min import lambda_min
from cvxpy.atoms.norm import norm
from copy import deepcopy
from simulation import showGraph

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



############################### Find gain A #################################

# Triangle
# initial_position = np.matrix([[0, -94], [94, -94], [94, 0], [0, 94], [-94, 94], [-94, 0]])
# destination = np.array([-1, 0, 0, 0, 1, 0, 0.5, 0.5, 0, 1, -0.5, 0.5])*100

# Hexagon
# initial_position = np.matrix([[0, -47], [94, -47], [94, 47], [0, 47], [-94, 47], [-94, -47]])
# destination = np.array([-0.5, -1, 0.5, -1, 1, 0, 0.5, 1, -0.5, 1, -1, 0])*100

# T - shape (Twice fan)
initial_position = np.matrix([[47, -47], [0, -47], [-47, -47], [-47, 47], [0, 47], [47, 47]])
destination = np.array([0.5, 1, 0, 0, -0.5, 1, -1, 1, 0, 0.5, 1, 1])*100


print(f"Initial position: {initial_position}.")
print(f"Shape of initial_position: {initial_position.shape}.")

print(f"destination: {destination}.")
destination_bar = np.zeros(len(destination))
for i in range(0,len(destination),2):
	destination_bar[i], destination_bar[i+1] = rotate_coordinate((destination[i], destination[i+1]), pi/2)
	# destination_bar[i] = round(destination_bar[i], 2)
	# destination_bar[i+1] = round(destination_bar[i+1], 2)
	destination_bar[i] = destination_bar[i]
	destination_bar[i+1] = destination_bar[i+1]

number_of_agent = int(len(destination)/2)

dij_star = find_dij_star(destination, number_of_agent)

print(f"\n\ndestination: {destination}.")
print(f"rotated destination: {destination_bar}.")	

vector_one = np.ones(len(destination))
vector_one_bar = np.ones(len(destination))
for i in range(0,len(vector_one_bar),2):
	vector_one_bar[i] = -1

print(f"\n\nvector_one: {vector_one}.")
print(f"vector_one_bar: {vector_one_bar}.")

# vector_N = np.concatenate((vector_one, vector_one_bar, destination, destination_bar), axis = 1)
N = np.matrix([vector_one, vector_one_bar, destination, destination_bar]).transpose()
print(f"\n\nMatrix N: {N}.")
print(f"Shape of N: {N.shape}.")

# Solve singular value decomposition (SVD) of N
U,_,_ = svd(N)
U = np.array(U)
print(f"\n\nMatrix U: {U}")
print(f"Shape of U: {U.shape}.")

Q = deepcopy(U[:,4:len(destination)])
print(f"\n\nMatrix Q: {Q}")
print(f"Shape of Q: {Q.shape}.")

# Solve SDP problem
A = cp.Variable(shape = (len(destination), len(destination)))
A_bar = Q.transpose()*A*Q

print(f"\n\nShape of A: {A.shape}.")
print(f"Shape of A_bar: {A_bar.shape}.")

objective = cp.Maximize(lambda_min(-A_bar))
constraint = [A*N == 0,
			  norm(A) <= 10]

problem = cp.Problem(objective, constraint)
problem.solve()

gain_A = np.matrix(A.value).round(5)
# gain_A = np.matrix(A.value)

print(f"Status: {problem.status}. \n")
print(f"Optimal value: {problem.value}. \n")
print(f"Optimal variable: \n{gain_A}. \n")

print(f"Shape of gain_A: {gain_A.shape}")

gain_Ai = np.zeros((number_of_agent,number_of_agent,2,2)) # I try putting everything nice and neat
														  # A will equal to Ai later on

for i in range(0,gain_A.shape[0],2): # go through all agents
	for j in range(0,gain_A.shape[1],2): # go through all neighbor
		# if (i != j):
		#     gain_Aij = np.matrix([[gain_A[i][j], gain_A[i][j+1]],[gain_A[i+1][j], gain_A[i+1][j+1]]])
		#     gain_Ai[int(i/2),int(j/2)] = gain_Aij
		# else:
		#     gain_Ai[int(i/2),int(j/2)] = np.zeros((2,2))
		gain_Aij = np.matrix([[gain_A[i][j], gain_A[i][j+1]],[gain_A[i+1][j], gain_A[i+1][j+1]]])
		gain_Ai[int(i/2),int(j/2)] = gain_Aij

gain_A = gain_Ai # The final gain.

print(f"Gain A: \n{gain_A}\n")
print(f"Shape of gain A: {gain_A.shape}")

########################### Simulation ############################
time_interval = 0.025 # 1 second
current_position = deepcopy(initial_position)
print(f"\nShape of current_position[0]: {current_position[0].shape}\n")

output_file = open("python_generated_path.txt", "w")

# control vector
u = np.zeros((number_of_agent,2,1))
print(f"u: {u}.\n\n")

for i in range(number_of_agent):
	if (i < (number_of_agent-1)):
		output_file.write(f"{float(current_position[i].transpose()[0])},{float(current_position[i].transpose()[1])},")
		# print(f"type: {type}")
	else:
		output_file.write(f"{float(current_position[i].transpose()[0])},{float(current_position[i].transpose()[1])}")
output_file.write("\n")	
for iteration in range(100): # iterate for 5 times
	print(f"Iteration number: {iteration}.")
	previous_position = deepcopy(current_position)
	for i in range(number_of_agent): # calculate new position of all agents
		u[i] = np.zeros((2,1))
		
		# calculate control vector of 1 agent
		for j in range(number_of_agent): 
			u[i] = u[i] + gain_A[i][j]*current_position[j].transpose() + scale_adjustment_fnc(current_position[i].transpose(), current_position[j].transpose(), dij_star[i][j])

		current_position[i] = current_position[i] + u[i].transpose()*time_interval
		# print(f"control vector of agent {i}: {u[i].transpose()}")
		# print(f"current_position of agent {i}: {current_position[i]}\n")

		if (i < (number_of_agent-1)):
			output_file.write(f"{float(current_position[i].transpose()[0])},{float(current_position[i].transpose()[1])},")
		else:
			output_file.write(f"{float(current_position[i].transpose()[0])},{float(current_position[i].transpose()[1])}")
	
	output_file.write("\n")	

	if np.array_equal(current_position, previous_position):
		print("Formation is formed.")
		break	
	# print("\n")


output_file.close()

showGraph()
