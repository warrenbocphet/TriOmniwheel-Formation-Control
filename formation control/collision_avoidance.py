import numpy as np
from math import acos, cos, sin, pi
from numpy.linalg import norm as norm_np

safety_radius = 40
collision_avoicedance_distance = 60

def unit_vector(vector):
    return vector / norm_np(vector)

def angle_between(v1, v2) :

    v1_u = unit_vector(v1)
    v2_u = unit_vector(v2)

    # print(f"v1_u: {v1_u}")
    # print(f"v2_u: {v2_u}")

    return np.arccos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

def collision_avoicedance(agent, neighbor, distance, u_fnc, agent_id):
    global safety_radius

    for i in range(len(neighbor)):
        if (i != agent_id):
            alpha = alpha = acos(safety_radius/distance[i])
            a2n_vector = np.array(neighbor[i] - agent)

            # angle between a2n_vector and u_fnc
            phi = angle_between(a2n_vector[0], u_fnc)
            if (phi<alpha): # true if control vector is in the danger cone
                print(f"Agent {agent_id} is in danger.")
                rotate_angle = 0
                rotate_angle = alpha - phi
                # We don't really know the sign of rotate_angle, i.e rotate CW or CCW
                # To find the sign, we need to consider different cases, I'm still trying to find
                # the ultimate way to do this
                # print(f"u_fnc type: {type(u_fnc)}")
                # print(f"u_fnc shape: {u_fnc.shape}")
                # print(f"u_fnc: {u_fnc}")
                a2n_Xaxis_angle = angle_between(a2n_vector, np.array([1,0]))
                u_fnc_Xaxis_angle = angle_between(u_fnc.reshape(2), np.array([1,0]))                
                agent = np.array(agent)

                if (agent[0][1] < neighbor[i][0][1]): # if agent is closer to x axis than neighbor

                    if (u_fnc_Xaxis_angle > a2n_Xaxis_angle): 
                        print("Case 1.")
                        rotate_angle = rotate_angle # rotate angle is positive

                    elif (u_fnc_Xaxis_angle < a2n_Xaxis_angle):
                        print("Case 2.")
                        rotate_angle = -rotate_angle # rotate angle is negative

                elif (agent[0][1] > neighbor[i][0][1]):
                   
                    if (u_fnc_Xaxis_angle < a2n_Xaxis_angle): 
                        print("Case 3.")
                        rotate_angle = rotate_angle # rotate angle is positive

                    elif (u_fnc_Xaxis_angle > a2n_Xaxis_angle):
                        print("Case 4.")
                        rotate_angle = -rotate_angle # rotate angle is negative
                
                if (abs(rotate_angle) < pi/2):
                    print(f"Agent {agent_id} has to stop.")
                    u_fnc = 0
                    return u_fnc

                print(f"Rotate angle: {rotate_angle/pi*180}")
                rotation_matrix = np.matrix([[cos(rotate_angle), -sin(rotate_angle)], [sin(rotate_angle), cos(rotate_angle)]])
                u_fnc = rotation_matrix*u_fnc.reshape(2,1)

                break # the reason I break right when I found the rotation angle is because I don't think there's 
                  # a possibility for the control vector to be in between 2 danger cone.
    
    return u_fnc

def dynamic_collision_avoicedance(current_pos_fnc, agent_id, u_fnc, n_agent):
    global collision_avoicedance_distance
    global safety_radius

    danger_neighbor = []
    distance_to_danger_neighbor = []

    for i in range(n_agent): # go through all the neighbors
        if (i != agent_id): # if it is not itself(i.e a neighbor), then...
            # Calculate distance from agent to the neighbor
            distance = norm_np(current_pos_fnc[agent_id] - current_pos_fnc[i])
            # print(f"distance from agent {agent_id} to neighbor {i}: {distance}.")
            if (distance < collision_avoicedance_distance): # check if this neighbor is the danger_neighbor
                danger_neighbor.append(current_pos_fnc[i])
                distance_to_danger_neighbor.append(distance)

    if (len(danger_neighbor) != 0): 
        # print(f"Agent {agent_id} is in danger activation range.")
        danger_neighbor = np.array(danger_neighbor)
        distance_to_danger_neighbor = np.array(distance_to_danger_neighbor)

        u_fnc = collision_avoicedance(current_pos_fnc[agent_id], danger_neighbor, distance_to_danger_neighbor, u_fnc, agent_id)

    # else:
    #     print(f"Agent {agent_id} not in danger activation range.")

    return u_fnc

if __name__ == '__main__':
    current_pos = np.matrix([[0,0],[-30,-40],[40,-30],[40,40]])
    agent_id = 0
    u = np.array([5,4])
    n_agent = 4

    u = dynamic_collision_avoicedance(current_pos, agent_id, u, n_agent)
    print(f"The new vector:\n{u}.")
