import numpy as np
from math import acos, asin, cos, sin, pi
from numpy.linalg import norm

safety_radius = 30
collision_avoicedance_distance = 60

def signed_angle_between(v1, v2) :

    return np.arcsin(np.cross(v1, v2)/(norm(v1)*norm(v2)))

def collision_avoicedance(agent, danger_neighbor, distance, control_vector, agent_id, danger_neighbor_id):
    global safety_radius

    for i in range(len(danger_neighbor)):
        # print(f"\n Danger neighbor number {i}.")
        try:
            alpha = asin(safety_radius/distance[i])
        except:
            print(f"Agent {agent_id} is too close to neighbor {danger_neighbor_id[i]} to move.")
            print(f"The distance is {distance[i]}.")
            control_vector = np.zeros(2)
            return control_vector
        # print(f"danger_neighbor[i]: {danger_neighbor[i]}.")
        # print(f"agent: {agent}.")
        a2n_vector = np.array(danger_neighbor[i] - agent)
        # print(f"a2n_vector: {a2n_vector}.")
        # angle between a2n_vector and control_vectors
        phi = abs(signed_angle_between(a2n_vector, control_vector))
        if (phi<alpha): # true if control vector is in the danger cone
            # print(f"Agent {agent_id} is in danger because of neighbor {danger_neighbor_id[i]}.")
            # print(f"phi: {phi/pi*180}, alpha: {alpha/pi*180}.")
            # print(f"distance: {distance[i]}.")
            rotate_angle = alpha - phi
            # print(f"Rotate angle: {rotate_angle/pi*180}")
            
            if (abs(rotate_angle) > pi/2):
                print(f"Agent {agent_id} has to stop because turning angle is bigger than 90 degree.")
                control_vector = np.zeros(2)
                return control_vector
            
            rotation_matrix1 = np.array([[cos(alpha), -sin(alpha)], [sin(alpha), cos(alpha)]])
            tangential_vector_a2n_1 = np.dot(rotation_matrix1,a2n_vector)

            rotation_matrix2 = np.array([[cos(-alpha), -sin(-alpha)], [sin(-alpha), cos(-alpha)]])
            tangential_vector_a2n_2 = np.dot(rotation_matrix2,a2n_vector)

            turn_angle1 = signed_angle_between(control_vector, tangential_vector_a2n_1)
            turn_angle2 = signed_angle_between(control_vector, tangential_vector_a2n_2)

            if (abs(turn_angle1)<abs(turn_angle2)):
                rotation_matrix3 = np.array([[cos(turn_angle1), -sin(turn_angle1)], [sin(turn_angle1), cos(turn_angle1)]])
                control_vector = np.dot(rotation_matrix3,control_vector)

            else:
                rotation_matrix3 = np.array([[cos(turn_angle2), -sin(turn_angle2)], [sin(turn_angle2), cos(turn_angle2)]])
                control_vector = np.dot(rotation_matrix3,control_vector)

            print(f"Agent {agent_id} has to turn to get away from danger cone.")
            return control_vector
            # the reason I return right when I found the rotation angle is because I don't think there's 
            # a possibility for the control vector to be in between 2 danger cone.
    
    return control_vector

def dynamic_collision_avoicedance(current_pos_fnc, agent_id, control_vector, n_agent):
    global collision_avoicedance_distance
    global safety_radius

    danger_neighbor = []
    distance_to_danger_neighbor = []
    danger_neighbor_id = []

    for i in range(n_agent): # go through all the neighbors
        if (i != agent_id): # if it is not itself(i.e a neighbor), then...
            # Calculate distance from agent to the neighbor
            distance = norm(current_pos_fnc[agent_id] - current_pos_fnc[i])
            # print(f"agent {agent_id} position: {current_pos_fnc[agent_id]}")
            # print(f"neighbor {i} position: {current_pos_fnc[i]}")
            # print(f"distance from agent {agent_id} to neighbor {i}: {distance}.")
            if (distance < collision_avoicedance_distance): # check if this neighbor is the danger_neighbor
                danger_neighbor.append(current_pos_fnc[i])
                distance_to_danger_neighbor.append(distance)
                danger_neighbor_id.append(i)

    if (len(danger_neighbor) != 0): 
        # print(f"Agent {agent_id} is in danger activation range.")

        control_vector = collision_avoicedance(current_pos_fnc[agent_id], danger_neighbor, distance_to_danger_neighbor, control_vector, agent_id, danger_neighbor_id)

    return control_vector

if __name__ == '__main__':
    current_pos = np.array([[0,0],[-30,-40],[40,-30],[40,40]])
    print(f"In main, agent at (0,0) is represent as: {current_pos[0]}")
    agent_id = 0
    control_vector = np.array([5,4])
    n_agent = 4

    control_vector = dynamic_collision_avoicedance(current_pos, agent_id, control_vector, n_agent)
    print(f"The new vector:\n{control_vector}.")
