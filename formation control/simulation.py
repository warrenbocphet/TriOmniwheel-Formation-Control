import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

fig = plt.figure("Map")
ax = plt.axes()

iteration = 0
x_boundary = 282
y_boundary = 282
color = ['r', 'b', 'g', 'y', 'c', 'b']

def animate(i):
	global iteration

	# read input text file from matlab and set target list
	input_file = open("old_liveCoordinate.txt","r")
	input_matrix = [] 
	while True:
		input_line = input_file.readline()
		if len(input_line) == 0:
			break
		input_line = input_line.rstrip("\n")
		input_matrix.append(input_line.split(","))

	for i in range(len(input_matrix)):
		for j in range(len(input_matrix[0])):
			input_matrix[i][j] = float(input_matrix[i][j])

	# plot data
	if (len(input_matrix) > iteration):
		# Clear out old stuffs
		ax.clear() # graph
		circle = [] # patches

		# Draw the boundary
		ax.add_patch(plt.Rectangle((-x_boundary/2, -y_boundary/2), x_boundary, y_boundary, fill = 0))

		# Draw the xy axis
		ax.add_patch(plt.Rectangle((-x_boundary/2, 0), x_boundary, 0, fill = 0))
		ax.add_patch(plt.Rectangle((0, -y_boundary/2), 0, y_boundary, fill = 0))

		graph_data = input_matrix[iteration]
		for j in range(0,len(input_matrix[0]),2):	
			# Draw the agent
			ax.add_patch(plt.Circle((input_matrix[iteration][j], input_matrix[iteration][j+1]), radius=15, fc='r', fill = 0, color = color[int(j/2)]))
			ax.add_patch(plt.Circle((input_matrix[iteration][j], input_matrix[iteration][j+1]), radius=2, fc='b', fill = 1, color = color[int(j/2)]))

		plt.axis('scaled')

		iteration = iteration + 1


ani = animation.FuncAnimation(fig, animate, interval=50)
plt.show()