# 3-OmniWheels-Rover-Platform
This project developed by students in the University of Adelaide during the Summer Research project 2018-2019, which developed a 3WD  rover platform for autonomous multi-agent systems.

## Overview:
>The distributed system will contain multiple agents. Each agent can make its own decision. However, the four agents still need to communicate with each other about the velocity, the current position, the destination and the angle of the agent. This allows an agreement to be made to achieve the desired formation.

## Hardware setup:
Motor: 251RPM DC geared (43.7:1) motor w/ ecoder.

Encoder: 64 resolution => 2797 count per rotation.

Control board: Arduino Due for controlling, Arduino MKR1000 for WiFi communication.

## Example
### Simple case (user generated coordinate/path)
Real data feedback and put into simulation
![Alt Text](https://i.imgur.com/8nDaBDt.gif)

Real demo. Noticed how the position of the rovers and the feedback position is not entirely the same. Overtime, the system introduce more and more error. Since the system is self guidance, there's no correction during performance.
![Alt Text](https://i.imgur.com/Ef7aGyV.gif)

### Formation control without collision avoidance
Generate path to form a triangle formation. Connected circle represent rover with radius, dash circle represent the safety distance (collision avoicedance is not implement in the GIF below).
![Alt Text](https://i.imgur.com/ODTWwnD.gif)

The same triangle formation with different starting position. Noticed how the formation is rotated with these starting positions. The algorithm [1] does not try to get to a specific coordinate but rather a formation (i.e, a triangle, circle or any arbitrary topology).

![Alt Text](https://i.imgur.com/WZkjorT.gif)

### Formation control with collision avoidance
The collision avoidance will be explained further down, let's first have a look at the simulation and the result:

#### Hexagon
![Alt Text](https://i.imgur.com/Hfp8QQ0.gif)

![Alt Text](https://imgur.com/erGZwwM.gif)

#### Triangle
![Alt Text](https://i.imgur.com/Pr2BR32.gif)

![Alt Text](https://imgur.com/bW4sY1b.gif)


#### X shape
![Alt Text](https://i.imgur.com/LUwJB9V.gif)

![Alt Text](https://imgur.com/bYgxLGv.gif)


#### Cross
![Alt Text](https://i.imgur.com/rR9ngYH.gif)

![Alt Text](https://imgur.com/uuUhYH2.gif)

#### Accumulating error (youtube link)
[![IMAGE ALT TEXT](https://img.youtube.com/vi/cinEIDQ7HyU/0.jpg)](https://www.youtube.com/watch?v=cinEIDQ7HyU "Accumulating error")

#### Reducing accumulating error by reducing the speed (youtube link)
[![IMAGE ALT TEXT](https://img.youtube.com/vi/cinEIDQ7HyU/0.jpg)](https://youtu.be/D_yIDlyvtvU "Reducing accumulating error by reducing the speed")

## Reference
[1] Fathian, Kaveh & Safaoui, Sleiman & Summers, Tyler & Gans, Nicholas. (2018). Robust Distributed Planar Formation Control for Higher-Order Holonomic and Nonholonomic Agents. Link: https://www.researchgate.net/publication/326696917_Robust_Distributed_Planar_Formation_Control_for_Higher-Order_Holonomic_and_Nonholonomic_Agents

The author's website: https://sites.google.com/view/kavehfathian/home?authuser=0
