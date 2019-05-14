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
	input_file = open("liveCoordinate.txt","r")

	input_line = input_file.readline()
	input_line = input_line.rstrip("\n")
	input_line = input_line.split(",")

	input_coor = []
	for i in range(len(input_line)):
		if (input_line[i] != ""):
			input_coor.append(float(input_line[i]))

	# plot data
	if (len(input_coor) != 0):
		# Clear out old stuffs
		ax.clear() # graph
		circle = [] # patches

		# Draw the boundary
		ax.add_patch(plt.Rectangle((-x_boundary/2, -y_boundary/2), x_boundary, y_boundary, fill = 0))

		# Draw the xy axis
		ax.add_patch(plt.Rectangle((-x_boundary/2, 0), x_boundary, 0, fill = 0))
		ax.add_patch(plt.Rectangle((0, -y_boundary/2), 0, y_boundary, fill = 0))

		for i in range(0,len(input_coor),2):	
			# Draw the agent
			ax.add_patch(plt.Circle((input_coor[i], input_coor[i+1]), radius=15, fc='r', fill = 0, color = color[int(i/2)]))
			ax.add_patch(plt.Circle((input_coor[i], input_coor[i+1]), radius=2, fc='b', fill = 1, color = color[int[i/2]]))

		plt.axis('scaled')

	input_file.close()

ani = animation.FuncAnimation(fig, animate, interval=10)
plt.show()