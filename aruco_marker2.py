import numpy as np
import cv2
from cv2 import aruco
import copy

cap = cv2.VideoCapture(0)
flip_constant = 1

# Capture 1 frame
ret, source = cap.read()
source = cv2.flip(source, flip_constant)

homography_fromSource = copy.deepcopy(source)
destination = copy.deepcopy(source)
source_point = np.zeros((9,2))
destination_point = np.array([[-50, 50], [0, 50], [50, 50], [-50, 0], [0, 0], [50, 0], [-50, -50], [0, -50], [50, -50]])
homography_flag = 0

def flip_coor(x, y):
	return 640 - x, y

# event driven function to choose 9 points for the homography matrix calculation
def show_coordinate(event,x,y,flags,param):
	global source
	global homography_fromSource
	global select_sourcePoint
	global source_point

	if (event == cv2.EVENT_LBUTTONDOWN):
		if (select_sourcePoint < 9):
			source_point[select_sourcePoint] = flip_coor(x, y)
			homography_fromSource = copy.deepcopy(source)
			print("Coordinate: " + str(x) + ", " + str(y))
			cv2.putText(homography_fromSource, "Coordinate: " + str(x) + ", " + str(y), (x,y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255))
			select_sourcePoint = select_sourcePoint + 1
		elif (select_sourcePoint == 9):
			homography_fromSource = copy.deepcopy(source)

cv2.namedWindow('Calibration')
cv2.setMouseCallback('Calibration',show_coordinate)

select_sourcePoint = 0

# Calculate homography matrix
while(homography_flag == 0):
	cv2.imshow('Calibration',homography_fromSource)

	if (select_sourcePoint == 9):
		print("Calculating homography matrix")
		homography, status = cv2.findHomography(source_point, destination_point)
		homography_flag = 1

	k = cv2.waitKey(1) & 0xFF
	if k == ord('q'):
		break

cv2.destroyAllWindows()

while(True):
	# Capture frame-by-frame
	ret, frame = cap.read()

	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_6X6_250)
	parameters = aruco.DetectorParameters_create()
	corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

	if (len(corners) != 0):
		aruco_marker = [(corners[0][0][0][0]+corners[0][0][2][0])/2.0, (corners[0][0][0][1] + corners[0][0][2][1])/2.0, 1]	# rejected = aruco.drawDetectedMarkers(frame.copy(), rejectedImgPoints)
		middle_point = copy.deepcopy(aruco_marker)	
		aruco_marker = flip_coor(aruco_marker[0], aruco_marker[1])
		
		# map the middle point to the real world coordinate system
		middle_point = np.transpose(middle_point)
		middle_point = np.dot(homography, middle_point)

		# display coordinate on screen
		frame = cv2.flip(frame, flip_constant)
		cv2.putText(frame, str("Coordinate: ") + str(round(middle_point[0],2)) + ", " + str(round(middle_point[1],2)), (int(aruco_marker[0]), int(aruco_marker[1]) + 10), cv2.FONT_HERSHEY_DUPLEX, 0.5, (0,0,255))
	else:
		frame = cv2.flip(frame, flip_constant)
		print("Fail to detect.")

	cv2.imshow("Path", frame)

	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()