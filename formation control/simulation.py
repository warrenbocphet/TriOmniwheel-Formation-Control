import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as patches
import numpy as np

fig = plt.figure("Map")
ax = plt.axes()

iteration = 0
x_boundary = 282
y_boundary = 282
color = ['r', 'b', 'g', 'y', 'c', 'b']

number_of_agent = 6
scale_constant = 0.1
# color = ['black', 'dimgray', 'dimgrey', 'gray', 'grey', 'darkgray','darkgrey']

# read input text file from matlab and set target list
input_file1 = open("python_generated_path.txt","r")
input_matrix1 = [] 

input_file2 = open("python_generated_control_vector.txt","r")
input_matrix2 = []

# Coordinate
while True:
	input_line1 = input_file1.readline()
	if len(input_line1) == 0:
		break
	input_line1 = input_line1.rstrip("\n")
	input_matrix1.append(input_line1.split(","))

for i in range(len(input_matrix1)):
	for j in range(len(input_matrix1[0])):
		input_matrix1[i][j] = int(float(input_matrix1[i][j]))

# Control vector (velocity)
while True:
	input_line2 = input_file2.readline()
	if len(input_line2) == 0:
		break
	input_line2 = input_line2.rstrip("\n")
	input_matrix2.append(input_line2.split(","))

for i in range(len(input_matrix2)):
	for j in range(len(input_matrix2[0])):
		input_matrix2[i][j] = int(float(input_matrix2[i][j]))

# print(f"len(input_matrix1): {len(input_matrix1)}")
# print(f"len(input_matrix2): {len(input_matrix2)}")

def animate(i):
	global iteration
	global number_of_agent
	global scale_constant

	# print(f"iteration: {iteration}.")

	# plot data
	if (len(input_matrix1) > iteration):
		# Clear out old stuffs
		ax.clear() # graph
		circle = [] # patches

		# Draw the boundary
		ax.add_patch(plt.Rectangle((-x_boundary/2, -y_boundary/2), x_boundary, y_boundary, fill = 0))

		# Draw the xy axis
		ax.add_patch(plt.Rectangle((-x_boundary/2, 0), x_boundary, 0, fill = 0))
		ax.add_patch(plt.Rectangle((0, -y_boundary/2), 0, y_boundary, fill = 0))

		graph_data = input_matrix1[iteration]
		for j in range(0,number_of_agent):	
			# Draw the agent		
			ax.add_patch(plt.Circle((input_matrix1[iteration][j*2], input_matrix1[iteration][j*2+1]), radius=15, fc='r', fill = 0, color = color[j]))
			ax.add_patch(plt.Circle((input_matrix1[iteration][j*2], input_matrix1[iteration][j*2+1]), radius=30, fc='r', fill = 0, color = color[j], linestyle = 'dashed'))
			ax.add_patch(plt.Circle((input_matrix1[iteration][j*2], input_matrix1[iteration][j*2+1]), radius=2, fc='b', fill = 1, color = color[j]))
			plt.text(input_matrix1[iteration][j*2], input_matrix1[iteration][j*2+1], str(j), fontsize = 'xx-large')
			
			# Draw agent's control vector
			ax.add_patch(patches.Arrow(input_matrix1[iteration][j*2], input_matrix1[iteration][j*2+1], input_matrix2[iteration][j*2]*scale_constant, input_matrix2[iteration][j*2+1]*scale_constant, width = 10.0))

		plt.axis('scaled')

		iteration = iteration + 1

def showGraph():
	ani = animation.FuncAnimation(fig, animate, interval=100)
	plt.show()


if __name__ == '__main__':
	ani = animation.FuncAnimation(fig, animate, interval=100)
	# ani.save("simulation.mp4")
	plt.show()

