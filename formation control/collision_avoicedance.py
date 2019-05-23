import numpy as np
from math import acos, cos, sin
from numpy.linalg import norm as norm_np

def unit_vector(vector):
    return vector / np.linalg.norm_np(vector)

def angle_between(v1, v2):

    v1_u = unit_vector(v1)
    v2_u = unit_vector(v2)
    return np.arccos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

def collision_avoicedance(agent, neighbor, distance, u_fnc):
    safety_radius = 30

    alpha = acos(safety_radius/distance) # Calculate half the angle of the cone

    a2n_vector = np.array(agent-neighbor) # Vector from agent to neighbor
    phi = angle_between(a2n_vector, u_fnc) # Calculate the angle between control vector and a2n_vector

    if phi < alpha: # control vector is in danger zone and potentially collide to the neighbor
        delta = alpha - phi # the angle that control vector need to rotate
        rotation_matrix = np.array([[cos(delta), -sin(delta)], [sin(delta), cos(delta)]])

        u_fnc = rotation_matrix*u_fnc.transpose()
        u_fnc = u_fnc.transpose()
        
    return u_fnc

def dynamic_collision_avoicedance(current_pos_fnc, agent_id, u_fnc, n_agent):
    collision_avoicedance_distance = 40

    for i in range(n_agent): # go through all the neighbors
        if (i != agent_id): # if it is not itself(i.e a neighbor), then...
            # Calculate distance from agent to the neighbor
            distance = norm_np((current_pos_fnc[agent_id*2], current_pos_fnc[agent_id*2+1]) - (current_pos_fnc[i*2], current_pos_fnc[i*2+1]))
            if (distance < safety_distance): # activate collision avoicedance if it's within range
                u_fnc = collision_avoicedance((current_pos_fnc[agent_id*2], current_pos_fnc[agent_id*2+1]), (current_pos_fnc[i*2], current_pos_fnc[i*2+1]), distance, u_fnc)

    return u_fnc
