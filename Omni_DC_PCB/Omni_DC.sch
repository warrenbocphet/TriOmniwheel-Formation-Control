EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L arduino:Arduino_Due_Shield XA1
U 1 1 5C9ED9AD
P 2550 4400
F 0 "XA1" H 2550 2020 60  0000 C CNN
F 1 "Arduino_Due_Shield" H 2550 1914 60  0000 C CNN
F 2 "Arduino (Alarm Silent Github):Arduino_Due_Shield" H 3250 7150 60  0001 C CNN
F 3 "https://store.arduino.cc/arduino-due" H 3250 7150 60  0001 C CNN
	1    2550 4400
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J6
U 1 1 5C9F6B0E
P 6450 3725
F 0 "J6" H 6500 4042 50  0000 C CNN
F 1 "Motor_conn0" H 6500 3951 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 6450 3725 50  0001 C CNN
F 3 "~" H 6450 3725 50  0001 C CNN
	1    6450 3725
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J13
U 1 1 5C9F6B94
P 8025 3675
F 0 "J13" H 8075 3992 50  0000 C CNN
F 1 "Motor_conn1" H 8075 3901 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 8025 3675 50  0001 C CNN
F 3 "~" H 8025 3675 50  0001 C CNN
	1    8025 3675
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J15
U 1 1 5C9F6BBB
P 9650 3725
F 0 "J15" H 9700 4042 50  0000 C CNN
F 1 "Motor_conn2" H 9700 3951 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 9650 3725 50  0001 C CNN
F 3 "~" H 9650 3725 50  0001 C CNN
	1    9650 3725
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J7
U 1 1 5C9F6E7A
P 6550 4025
F 0 "J7" V 6610 4066 50  0000 L CNN
F 1 "Plus_minus 0" V 6825 3775 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6550 4025 50  0001 C CNN
F 3 "~" H 6550 4025 50  0001 C CNN
	1    6550 4025
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Male J14
U 1 1 5C9F6EF0
P 8125 3975
F 0 "J14" V 8185 4016 50  0000 L CNN
F 1 "Plus_minus 1" V 8425 3700 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8125 3975 50  0001 C CNN
F 3 "~" H 8125 3975 50  0001 C CNN
	1    8125 3975
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Male J16
U 1 1 5C9F6F0C
P 9750 4000
F 0 "J16" V 9810 4041 50  0000 L CNN
F 1 "Plus_minus 2" V 10075 3750 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 9750 4000 50  0001 C CNN
F 3 "~" H 9750 4000 50  0001 C CNN
	1    9750 4000
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x05_Male J9
U 1 1 5C9F7084
P 7325 4550
F 0 "J9" V 7385 4790 50  0000 L CNN
F 1 "Polulu Voltage Regulator" V 7476 4790 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 7325 4550 50  0001 C CNN
F 3 "~" H 7325 4550 50  0001 C CNN
	1    7325 4550
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Male J8
U 1 1 5C9F728B
P 7225 4950
F 0 "J8" V 7285 4990 50  0000 L CNN
F 1 "12V input" V 7376 4990 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 7225 4950 50  0001 C CNN
F 3 "~" H 7225 4950 50  0001 C CNN
	1    7225 4950
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x08_Male J3
U 1 1 5C9F7691
P 5125 5525
F 0 "J3" V 4960 5451 50  0000 C CNN
F 1 "Conn to Driver0" V 5051 5451 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 5125 5525 50  0001 C CNN
F 3 "~" H 5125 5525 50  0001 C CNN
	1    5125 5525
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x08_Male J5
U 1 1 5C9F7824
P 6025 5525
F 0 "J5" V 5860 5451 50  0000 C CNN
F 1 "Conn to Driver 1" V 5951 5451 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 6025 5525 50  0001 C CNN
F 3 "~" H 6025 5525 50  0001 C CNN
	1    6025 5525
	0    1    1    0   
$EndComp
Text GLabel 7225 4750 3    50   Input ~ 0
12V
Text GLabel 7325 4750 3    50   Input ~ 0
Gnd
Text GLabel 7525 4750 3    50   Input ~ 0
5V
Text GLabel 7125 5150 3    50   Input ~ 0
12V
Text GLabel 7225 5150 3    50   Input ~ 0
Gnd
Text GLabel 6450 4225 3    50   Input ~ 0
A-
Text GLabel 6550 4225 3    50   Input ~ 0
A+
Text GLabel 8025 4175 3    50   Input ~ 0
B-
Text GLabel 8125 4175 3    50   Input ~ 0
B+
Text GLabel 9650 4200 3    50   Input ~ 0
C-
Text GLabel 9750 4200 3    50   Input ~ 0
C+
Text GLabel 6250 3825 3    50   Input ~ 0
A-
Text GLabel 6750 3825 3    50   Input ~ 0
A+
Text GLabel 7825 3775 3    50   Input ~ 0
B-
Text GLabel 8325 3775 3    50   Input ~ 0
B+
Text GLabel 9450 3825 3    50   Input ~ 0
C-
Text GLabel 9950 3825 3    50   Input ~ 0
C+
Text GLabel 6250 3625 1    50   Input ~ 0
Gnd
Text GLabel 6750 3625 1    50   Input ~ 0
3.3V
Text GLabel 7825 3575 1    50   Input ~ 0
Gnd
Text GLabel 8325 3575 1    50   Input ~ 0
3.3V
Text GLabel 9450 3625 1    50   Input ~ 0
Gnd
Text GLabel 9950 3625 1    50   Input ~ 0
3.3V
Text GLabel 6250 3725 0    50   Input ~ 0
Encoder0_A
Text GLabel 6750 3725 2    50   Input ~ 0
Encoder0_B
Text GLabel 9950 3725 2    50   Input ~ 0
Encoder2_B
Text GLabel 9450 3725 0    50   Input ~ 0
Encoder2_A
Text GLabel 7825 3675 0    50   Input ~ 0
Encoder1_A
Text GLabel 8325 3675 2    50   Input ~ 0
Encoder1_B
NoConn ~ 7425 4750
NoConn ~ 7125 4750
Text GLabel 4725 5725 3    50   Input ~ 0
EnA
Text GLabel 4825 5725 3    50   Input ~ 0
In1
Text GLabel 4925 5725 3    50   Input ~ 0
In2
Text GLabel 5025 5725 3    50   Input ~ 0
In3
Text GLabel 5125 5725 3    50   Input ~ 0
In4
Text GLabel 5225 5725 3    50   Input ~ 0
EnB
NoConn ~ 5325 5725
NoConn ~ 5425 5725
NoConn ~ 6225 5725
NoConn ~ 6325 5725
Text GLabel 5625 5725 3    50   Input ~ 0
EnC
Text GLabel 5725 5725 3    50   Input ~ 0
In5
Text GLabel 5825 5725 3    50   Input ~ 0
In6
NoConn ~ 6125 5725
NoConn ~ 6025 5725
NoConn ~ 5925 5725
Text GLabel 5275 4275 2    50   Input ~ 0
UART1
Text GLabel 1250 2250 0    50   Input ~ 0
UART0
Text GLabel 1250 2350 0    50   Input ~ 0
UART1
Text GLabel 5275 3775 2    50   Input ~ 0
5V
Text GLabel 5275 3975 2    50   Input ~ 0
Gnd
Text GLabel 1250 6250 0    50   Input ~ 0
5V
Text GLabel 1250 5650 0    50   Input ~ 0
Gnd
Text GLabel 1250 3950 0    50   Input ~ 0
In1
Text GLabel 1250 4050 0    50   Input ~ 0
In2
Text GLabel 1250 4150 0    50   Input ~ 0
In3
Text GLabel 1250 4250 0    50   Input ~ 0
In4
Text GLabel 1250 4350 0    50   Input ~ 0
In5
Text GLabel 1250 4450 0    50   Input ~ 0
In6
Text GLabel 3850 2250 2    50   Input ~ 0
EnA
Text GLabel 3850 2350 2    50   Input ~ 0
EnB
Text GLabel 3850 2450 2    50   Input ~ 0
EnC
Text GLabel 3850 2850 2    50   Input ~ 0
Encoder0_A
Text GLabel 3850 2950 2    50   Input ~ 0
Encoder0_B
Text GLabel 3850 3050 2    50   Input ~ 0
Encoder1_A
Text GLabel 3850 3150 2    50   Input ~ 0
Encoder1_B
Text GLabel 3850 3250 2    50   Input ~ 0
Encoder2_A
Text GLabel 3850 3350 2    50   Input ~ 0
Encoder2_B
NoConn ~ 1250 2450
NoConn ~ 1250 2550
NoConn ~ 1250 2750
NoConn ~ 1250 2850
NoConn ~ 1250 2950
NoConn ~ 1250 3050
NoConn ~ 1250 3150
NoConn ~ 1250 3350
NoConn ~ 1250 3450
NoConn ~ 1250 3650
NoConn ~ 1250 3750
NoConn ~ 1250 3850
NoConn ~ 1250 4550
NoConn ~ 1250 4650
NoConn ~ 1250 4750
NoConn ~ 1250 4850
NoConn ~ 1250 4950
NoConn ~ 1250 5050
NoConn ~ 1250 5250
NoConn ~ 1250 5350
NoConn ~ 1250 5450
NoConn ~ 1250 5550
NoConn ~ 1250 5750
NoConn ~ 1250 5850
NoConn ~ 1250 5950
NoConn ~ 1250 6050
NoConn ~ 1250 6150
NoConn ~ 1250 6350
NoConn ~ 1250 6450
NoConn ~ 1250 6550
NoConn ~ 2300 1800
NoConn ~ 2400 1800
NoConn ~ 2500 1800
NoConn ~ 2600 1800
NoConn ~ 2700 1800
NoConn ~ 2800 1800
NoConn ~ 3850 6550
NoConn ~ 3850 6450
NoConn ~ 3850 6350
NoConn ~ 3850 6250
NoConn ~ 3850 6150
NoConn ~ 3850 6050
NoConn ~ 3850 5950
NoConn ~ 3850 5850
NoConn ~ 3850 5750
NoConn ~ 3850 5650
NoConn ~ 3850 5550
NoConn ~ 3850 5450
NoConn ~ 3850 5350
NoConn ~ 3850 5250
NoConn ~ 3850 5050
NoConn ~ 3850 5150
NoConn ~ 3850 4950
NoConn ~ 3850 4850
NoConn ~ 3850 4750
NoConn ~ 3850 4650
NoConn ~ 3850 4550
NoConn ~ 3850 4450
NoConn ~ 3850 4350
NoConn ~ 3850 4250
NoConn ~ 3850 4150
NoConn ~ 3850 4050
NoConn ~ 3850 3950
NoConn ~ 3850 3850
NoConn ~ 3850 3750
NoConn ~ 3850 3650
NoConn ~ 3850 3550
NoConn ~ 3850 3450
NoConn ~ 3850 2550
NoConn ~ 3850 2650
NoConn ~ 3850 2750
$Comp
L Connector:Conn_01x14_Female J2
U 1 1 5C9FEC8B
P 4225 4275
F 0 "J2" H 4252 4251 50  0000 L CNN
F 1 "Header_MKR_left" H 3975 3425 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x14_P2.54mm_Vertical" H 4225 4275 50  0001 C CNN
F 3 "~" H 4225 4275 50  0001 C CNN
	1    4225 4275
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x14_Female J4
U 1 1 5C9FED26
P 5725 4275
F 0 "J4" H 5752 4251 50  0000 L CNN
F 1 "Header_MKR_right" H 5075 3425 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x14_P2.54mm_Vertical" H 5725 4275 50  0001 C CNN
F 3 "~" H 5725 4275 50  0001 C CNN
	1    5725 4275
	1    0    0    -1  
$EndComp
$Comp
L MKR_1010:MKR_1010 J1
U 1 1 5C9F6945
P 4125 4225
F 0 "J1" H 4875 5040 50  0000 C CNN
F 1 "MKR_1010" H 4875 4949 50  0000 C CNN
F 2 "MKR1010:MKR_1010" H 5075 4175 50  0001 C CNN
F 3 "~" H 5075 4175 50  0001 C CNN
	1    4125 4225
	1    0    0    -1  
$EndComp
Wire Wire Line
	4475 3675 4025 3675
Wire Wire Line
	4025 3775 4475 3775
Wire Wire Line
	4475 3875 4025 3875
Wire Wire Line
	4025 3975 4475 3975
Wire Wire Line
	4475 4075 4025 4075
Wire Wire Line
	4025 4175 4475 4175
Wire Wire Line
	4475 4275 4025 4275
Wire Wire Line
	4025 4375 4475 4375
Wire Wire Line
	4025 4475 4475 4475
Wire Wire Line
	4475 4575 4025 4575
Wire Wire Line
	4025 4675 4475 4675
Wire Wire Line
	4025 4775 4475 4775
Wire Wire Line
	4475 4875 4025 4875
Wire Wire Line
	4475 4975 4025 4975
Wire Wire Line
	5275 4975 5525 4975
Wire Wire Line
	5525 4875 5275 4875
Wire Wire Line
	5275 4775 5525 4775
Wire Wire Line
	5525 4675 5275 4675
Wire Wire Line
	5275 4575 5525 4575
Wire Wire Line
	5525 4475 5275 4475
Wire Wire Line
	5275 4375 5525 4375
Wire Wire Line
	5525 4075 5275 4075
Wire Wire Line
	5275 3675 5525 3675
NoConn ~ 1250 3250
NoConn ~ 1250 3550
NoConn ~ 1250 5150
Text GLabel 5275 4175 2    50   Input ~ 0
UART0
Wire Wire Line
	5525 3775 5950 3775
Wire Wire Line
	5950 3775 5950 3900
Text GLabel 5950 3900 2    50   Input ~ 0
5V
Wire Wire Line
	5525 3975 5950 3975
Wire Wire Line
	5950 3975 5950 4025
Text GLabel 5950 4025 2    50   Input ~ 0
Gnd
Wire Wire Line
	5525 4175 6050 4175
Wire Wire Line
	5525 4275 6075 4275
Text GLabel 6050 4175 2    50   Input ~ 0
UART0
Text GLabel 6075 4275 3    50   Input ~ 0
UART1
Text GLabel 5275 3875 2    50   Input ~ 0
3.3V
Text GLabel 5525 3875 3    50   Input ~ 0
3.3V
NoConn ~ 1250 2650
$EndSCHEMATC
