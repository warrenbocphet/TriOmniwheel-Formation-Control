import serial
from time import time

ser = serial.Serial("/dev/ttyS0", 9600)

path0 = "output0.txt"
path1 = "output1.txt"

output0 = open(path0, "w+")
output1 = open(path1, "w+")

n_sample = [0, 0, 0, 0, 0, 0, 0, 0]

i = 0
start = time()

while True:
	try:
		ser.write([1])
		b = ser.readline()
		c = ser.readline()
		b = b.decode("utf-8")
		c = c.decode("utf-8")

		n_sample[i] = n_sample[i] + 1

		output0.write(b)

		elapsedTime = time() - start
		if (elapsedTime > 3):
			output1.write("No. of sample at speed " + str(i*100) + ": ")
			output1.write(str(n_sample[i]))
			output1.write("\n\n")
			ser.write([0])
			i = i + 1

		if (i == 8):
			print("Done.")
			break


	except KeyboardInterrupt:
		break