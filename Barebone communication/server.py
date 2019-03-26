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
	# print(str(((byte_fnc[1]) | ((byte_fnc[0]&0x7f<<8)))))
	receivedCoor_fnc[1] = ((-1)**(byte_fnc[2]>>7)) * ((byte_fnc[3]) | (((byte_fnc[2]&0x7f)<<8)))
	# print(str(((byte_fnc[3]) | ((byte_fnc[2]&0x7f<<8)))))
	receivedCoor_fnc[2] = ((-1)**(byte_fnc[4]>>7)) * ((byte_fnc[5]) | (((byte_fnc[4]&0x7f)<<8)))

	return receivedCoor_fnc


if __name__ == '__main__':
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

	s.bind((socket.gethostname(), 1234)) # bind(ip, port)
	print("Done binding.")
	s.listen(5)

	clientsocket, address = s.accept()
	print(f"Connection from {address} has been established!")

	while True:
		print();
		print("What you want to do?")
		print("0. Send target")
		print("1. Get current coordinate")
		print("2. Set current coordinate (not yet implement)")
		try:
			a = int(input("I choose: "))
		except Exception:
			print("Error.")
			a = -1;

		if (a == 0):
			print("You want to send the target coordinate.")

			coor = [0, 0, 0]
			try:
				coor[0] = int(input("X: "))
				coor[1] = int(input("y: "))
				coor[2] = int(input("phi: "))

				coorByte = coor2bytes(coor)

			except Exception:
					print("Error.")

			clientsocket.send(bytes([0]))
			clientsocket.send(bytes(coorByte))
			print("I already sent the command.")

		elif (a == 1):
			print("You want to get the current coordinate.")
			clientsocket.send(bytes([1]))
			print("I already sent the command.")
			bytesReceived = []
			full_msg = []
			while (len(full_msg) < 8):
				bytesReceived = clientsocket.recv(8)
				# print("length of this message: " + str(len(bytesReceived)))
				for x in range(len(bytesReceived)):
					full_msg.append(bytesReceived[x])
			
			print("Full message:")
			for x in range(len(full_msg)):
				print(full_msg[x])
			receivedCoor = bytes2coor(full_msg)
			print("coordinate received: " + str(receivedCoor))

		else:
			print("Not yet implement.")
