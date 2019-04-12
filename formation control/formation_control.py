import socket
import time
import copy

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


if __name__ == '__main__':
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	s.bind((socket.gethostname(), 1234)) # bind(ip, port)
	print("Done binding.")
	s.listen(5)

	clientsocket, address = s.accept()
	print(f"Connection from {address} has been established!")

	target0 = [[0, 0, 0], [50, 50, 0], [-50, 50, 0], [-50, -50, 0], [50, -50, 0], [0, 0, 0]]
	target1 = [[0, 0, 0], [50, 50, 180], [-50, 50, 0], [-50, -50, -180], [50, -50, 90], [0, 0, 0]]

	while (True):
		print()
		choice = int(input("Target: "))
		if (choice == 0): # send target0
			for x in range(len(target0)):
				# send target
				targetSend = copy.deepcopy(target0[x])
				targetSend[1] = -targetSend[1]
				coorByte = coor2bytes(targetSend)
				clientsocket.send(bytes([0]))
				clientsocket.send(bytes(coorByte))
				print("I already sent the target " + str(target0[x]) + ".")

				# check position in order to send the next target.
				arrive = 0
				start = time.time()
				while (not arrive):
					elapsedTime = time.time() - start
					if (elapsedTime > 0.1): # check every 100ms
						start = time.time()
						clientsocket.send(bytes([1]))
						bytesReceived = []
						full_msg = []

						while (len(full_msg) < 8):
							bytesReceived = clientsocket.recv(8)
							for y in range(len(bytesReceived)):
								full_msg.append(bytesReceived[y])
						
						receivedCoor = bytes2coor(full_msg)
						print(" ")
						print(target0[x])
						print(receivedCoor)

						if ((abs(target0[x][0] - receivedCoor[0]) <= 2) and (abs(target0[x][1] - receivedCoor[1]) <= 2) and (abs(target0[x][2] - receivedCoor[2]) <= 10)): # arrive.
							arrive = 1


		if (choice == 1): # send target1	
			for x in range(len(target1)):
				# send target
				targetSend = copy.deepcopy(target1[x]) # flip the X axis
				targetSend[1] = -targetSend[1]
				coorByte = coor2bytes(targetSend)
				clientsocket.send(bytes([0]))
				clientsocket.send(bytes(coorByte))

				print("I already sent the target " + str(target1[x]) + ".")

				# check position in order to send the next target.
				arrive = 0
				start = time.time()
				while (not arrive):
					elapsedTime = time.time() - start
					if (elapsedTime > 0.1): # check every 100ms
						start = time.time()
						clientsocket.send(bytes([1]))
						bytesReceived = []
						full_msg = []

						while (len(full_msg) < 8):
							bytesReceived = clientsocket.recv(8)
							for y in range(len(bytesReceived)):
								full_msg.append(bytesReceived[y])
						
						receivedCoor = bytes2coor(full_msg)
						print("coordinate received: " + str(receivedCoor))

						if ((abs(target1[x][0] - receivedCoor[0]) <= 2) and (abs(target1[x][1] - receivedCoor[1]) <= 2) and (abs(target1[x][2] - receivedCoor[2]) <= 6)): # arrive.
							arrive = 1
		if (choice == -1): # stop the program.
			break
		else:
			print("Not yet implement.")