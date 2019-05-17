# 3-OmniWheels-Rover-Platform
Summer research 2018-2019. Rover platform development for autonomous and multi agent system.

## Overview:
>The distributed system will contain multiple agents. Each agent can make its own decision. However, the four agents still need to communicate with each other about the velocity, the current position, the destination and the angle of the agent. This allows an agreement to be made to achieve the desired formation.

## Hardware setup:
Motor: 251RPM DC geared (43.7:1) motor w/ ecoder.

Encoder: 64 resolution => 2797 count per rotation.

Control board: Arduino Due for controlling, Arduino MKR1000 for WiFi communication.

## Example
### Simple case (user generated coordinate/path)
![Alt Text](https://i.imgur.com/8nDaBDt.gif)
