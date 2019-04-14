import socket
import time

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
	s.listen(2)

	clientsocket, address = s.accept()
	print(f"Connection from {address} has been established!")

	clientsocket.settimeout(1)

	while True:
		print();
		print("What you want to do?")
		print("0. Send target")
		print("1. Get current coordinate")
		print("2. Set current coordinate and make the current target equal to current coordinate (so the rover won't move)")
		print("3. Just set the current coordinate")
		print("4. Ask for id.")

		try:
			a = int(input("I choose: "))
		except Exception:
			print("Error.")
			a = -1;

		if (a == 0): # send target
			coor = [0, 0, 0]
			try:
				coor[0] = int(input("X: "))
				coor[1] = -int(input("y: "))
				coor[2] = int(input("phi: "))

				coorByte = coor2bytes(coor)

				clientsocket.send(bytes([0]))
				clientsocket.send(bytes(coorByte))
				print("I already sent the target.")

			except Exception:
					print("Error.")

		elif (a == 1): # get current coordinate
			receive = 0
			while (not receive):
				try:
					clientsocket.send(bytes([1]))
					bytesReceived = []
					full_msg = []

					while (len(full_msg) < 8):
						bytesReceived = clientsocket.recv(8)
						for x in range(len(bytesReceived)):
							full_msg.append(bytesReceived[x])
					
					receivedCoor = bytes2coor(full_msg)
					print("coordinate received: " + str(receivedCoor))
					receive = 1
				except socket.timeout:
					print("Time out. Will try again.")

		elif (a == 2): # set new starting point
			setStartingPoint = [0, 0, 0]
			try:
				setStartingPoint[0] = int(input("X: "))
				setStartingPoint[1] = -int(input("y: "))
				setStartingPoint[2] = int(input("phi: "))

				setStartingPointByte = coor2bytes(setStartingPoint)

				clientsocket.send(bytes([2]))
				clientsocket.send(bytes(setStartingPointByte))

				print("I already sent the new starting coordinate.")
			except Exception:
					print("Error.")

		elif (a == 3): # set new current coordinate (to readjust the rover)
			setCurrentCoor = [0, 0, 0]
			try:
				setCurrentCoor[0] = int(input("X: "))
				setCurrentCoor[1] = -int(input("Y: "))
				setCurrentCoor[2] = int(input("Phi: "))

				setCurrentCoorByte = coor2bytes(setCurrentCoor)

				clientsocket.send(bytes([3]))
				clientsocket.send(bytes(setCurrentCoorByte))

				print("I already sent the new coordinate")
			except Exception:
				print(Exception)
				print("Error.")

		elif (a == 4): # Ask for ID.
			receive = 0
			clientsocket.send(bytes([4]))
			while (not receive):
				try:
					roverID = clientsocket.recv(8)
					receive = 1

					roverID = int.from_bytes(roverID, "little")
					print(roverID)
				except socket.timeout:
					print("Time out. Will try again.")



		else:
			print("Not yet implement.")
