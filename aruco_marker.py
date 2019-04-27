import numpy as np
import cv2
from cv2 import aruco
import copy

# constant
flip_constant = 1

# function
def flip_coor(point):
	# print(point[0])
	return 640 - point[0], point[1]

# class
class arucoMarker:
	def __init__(self):
		self.id = -1
		self.corner0 = 0
		self.corner1 = 0
		self.corner2 = 0
		self.corner3 = 0
		middle_point = [0, 0]

	def initialize(self, input_id, input_corner0, input_corner1, input_corner2, input_corner3):
		self.id = input_id
		self.corner0 = input_corner0
		self.corner1 = input_corner1
		self.corner2 = input_corner2
		self.corner3 = input_corner3

	def compute_midPoint(self):
		self.middle_point = [int((self.corner0[0] + self.corner2[0])/2), int((self.corner0[1] + self.corner2[1])/2)]

cap = cv2.VideoCapture(0)

# See if I can identify all 9 aruco marker
source_point = np.zeros((9,2))
destination_point = np.array([[-50, 50], [0, 50], [50, 50], [-50, 0], [0, 0], [50, 0], [-50, -50], [0, -50], [50, -50]])

while True:
	ret, frame = cap.read()
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_ARUCO_ORIGINAL)
	parameters = aruco.DetectorParameters_create()
	corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)
	frame_markers = aruco.drawDetectedMarkers(frame.copy(), corners, ids)

	if (len(corners) == 9 and ids.sum() == 36):
		captureAruco = copy.deepcopy(frame)
		break

	cv2.imshow("Current frame", frame_markers)
	key = cv2.waitKey(1) & 0xFF
	if key == ord('q'):
		break

cv2.destroyAllWindows()

# find the coordinate of those markers so I can compute the homography matrix
gray = cv2.cvtColor(captureAruco, cv2.COLOR_BGR2GRAY)
aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_ARUCO_ORIGINAL)
parameters = aruco.DetectorParameters_create()
corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

while True:
	cv2.imshow("Detection", captureAruco)

	key = cv2.waitKey(1) & 0xFF
	if key == ord('q'):
		source_point = np.array(source_point)
		print("Done finding all aruco markers.")
		break

	if key == ord('c'):
		for i in range(9):
			aruco_marker = arucoMarker()
			aruco_marker.initialize(ids[i], corners[i][0][0], corners[i][0][1], corners[i][0][2], corners[i][0][3])
			aruco_marker.compute_midPoint()

			source_point[aruco_marker.id] = aruco_marker.middle_point
			source_point[aruco_marker.id] = flip_coor(source_point[aruco_marker.id][0])
			cv2.putText(captureAruco, "id: " + str(aruco_marker.id), (aruco_marker.middle_point[0], aruco_marker.middle_point[1]), cv2.FONT_HERSHEY_DUPLEX, 0.5, (0,0,255))

############################################ Calculate homography matrix #########################################
print("Calculating homography matrix")
homography, status = cv2.findHomography(source_point, destination_point)

cv2.destroyAllWindows()

###################################### Tracking Aruco marker in the real world ##################################

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
		aruco_marker = flip_coor(aruco_marker)
		
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
