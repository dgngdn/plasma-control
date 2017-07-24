EESchema Schematic File Version 2
LIBS:74xgxx
LIBS:74xx
LIBS:ac-dc
LIBS:actel
LIBS:adc-dac
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:analog_switches
LIBS:atmel
LIBS:audio
LIBS:battery_management
LIBS:bbd
LIBS:bc
LIBS:bosch
LIBS:brooktre
LIBS:cmos4000
LIBS:cmos_ieee
LIBS:conn
LIBS:contrib
LIBS:cypress
LIBS:dc-dc
LIBS:device
LIBS:digital-audio
LIBS:diode
LIBS:display
LIBS:dsp
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:infineon
LIBS:intel
LIBS:interface
LIBS:intersil
LIBS:ir
LIBS:Lattice
LIBS:leds
LIBS:LEM
LIBS:linear
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:memory
LIBS:microchip
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic24mcu
LIBS:microchip_pic32mcu
LIBS:microcontrollers
LIBS:modules
LIBS:motor_drivers
LIBS:motorola
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:opto
LIBS:Oscillators
LIBS:philips
LIBS:power
LIBS:powerint
LIBS:Power_Management
LIBS:pspice
LIBS:references
LIBS:regul
LIBS:relays
LIBS:rfcom
LIBS:RFSolutions
LIBS:sensors
LIBS:silabs
LIBS:siliconi
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:texas
LIBS:transf
LIBS:transistors
LIBS:triac_thyristor
LIBS:ttl_ieee
LIBS:valves
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:xilinx
LIBS:zetex
LIBS:Zilog
LIBS:EXAR
LIBS:function-generator-cache
LIBS:testbed-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
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
L MCP4922-E/P U?
U 1 1 59372389
P 7400 3525
F 0 "U?" H 7000 3925 50  0000 L CNN
F 1 "MCP4922-E/P" H 7550 3925 50  0000 L CNN
F 2 "" H 7400 3525 50  0001 C CIN
F 3 "" H 7400 3525 50  0001 C CNN
	1    7400 3525
	-1   0    0    1   
$EndComp
$Comp
L LD271 D?
U 1 1 5937238A
P 6450 4275
F 0 "D?" H 6470 4345 50  0000 L CNN
F 1 "LD271" H 6410 4165 50  0000 C CNN
F 2 "LEDs:LED_D5.0mm_Horizontal_O6.35mm_Z3.0mm" H 6450 4450 50  0001 C CNN
F 3 "" H 6400 4275 50  0000 C CNN
	1    6450 4275
	1    0    0    1   
$EndComp
$Comp
L Q_Photo_NPN_CE Q?
U 1 1 5937238B
P 6525 4650
F 0 "Q?" H 6725 4700 50  0000 L CNN
F 1 "Q_Photo_NPN_CE" H 6725 4600 50  0000 L CNN
F 2 "" H 6725 4750 50  0001 C CNN
F 3 "" H 6525 4650 50  0001 C CNN
	1    6525 4650
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5937238C
P 6000 4575
F 0 "R?" V 6080 4575 50  0000 C CNN
F 1 "220" V 6000 4575 50  0000 C CNN
F 2 "" V 5930 4575 50  0001 C CNN
F 3 "" H 6000 4575 50  0001 C CNN
	1    6000 4575
	0    1    1    0   
$EndComp
$Comp
L Arduino_UNO_R3 A?
U 1 1 5937238D
P 5625 3400
F 0 "A?" H 5425 4450 50  0000 R CNN
F 1 "Arduino_UNO_R3" H 5425 4350 50  0000 R CNN
F 2 "Modules:Arduino_UNO_R3" H 5775 2350 50  0001 L CNN
F 3 "" H 5425 4450 50  0001 C CNN
	1    5625 3400
	1    0    0    -1  
$EndComp
NoConn ~ 7300 3025
NoConn ~ 6800 3325
Entry Wire Line
	3325 3700 3425 3800
Entry Wire Line
	3325 3600 3425 3700
Entry Wire Line
	4800 3900 4900 4000
Entry Wire Line
	4800 3800 4900 3900
Entry Wire Line
	4800 4000 4900 4100
Entry Wire Line
	3325 3500 3425 3600
Text Label 3425 3700 0    60   ~ 0
sck
Text Label 3425 3800 0    60   ~ 0
mosi
Text Label 3425 3900 0    60   ~ 0
miso
Text Label 3425 3600 0    60   ~ 0
cs1
Entry Wire Line
	3325 3800 3425 3900
Entry Wire Line
	4800 3700 4900 3800
Text Label 4900 3800 0    60   ~ 0
cs1
Entry Wire Line
	4800 3600 4900 3700
Text Label 4900 3700 0    60   ~ 0
cs2
Text Label 4900 3900 0    60   ~ 0
mosi
Text Label 4900 4000 0    60   ~ 0
miso
Text Label 4900 4100 0    60   ~ 0
sck
Text Label 8000 3525 0    60   ~ 0
cs2
Text Label 8000 3625 0    60   ~ 0
sck
Text Label 8000 3725 0    60   ~ 0
mosi
$Comp
L GND #PWR?
U 1 1 5937238E
P 5725 4625
F 0 "#PWR?" H 5725 4375 50  0001 C CNN
F 1 "GND" H 5725 4475 50  0000 C CNN
F 2 "" H 5725 4625 50  0001 C CNN
F 3 "" H 5725 4625 50  0001 C CNN
	1    5725 4625
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5937238F
P 8425 3425
F 0 "#PWR?" H 8425 3175 50  0001 C CNN
F 1 "GND" H 8425 3275 50  0000 C CNN
F 2 "" H 8425 3425 50  0001 C CNN
F 3 "" H 8425 3425 50  0001 C CNN
	1    8425 3425
	1    0    0    -1  
$EndComp
Entry Wire Line
	8250 3525 8350 3625
Entry Wire Line
	8250 3625 8350 3725
Entry Wire Line
	8250 3725 8350 3825
$Comp
L MCP4162 U?
U 1 1 59372390
P 4100 3750
F 0 "U?" H 3800 4050 50  0000 L CNN
F 1 "MCP4162" H 4200 4050 50  0000 L CNN
F 2 "" H 4100 3700 50  0001 C CNN
F 3 "" H 4100 3700 50  0001 C CNN
	1    4100 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3425 3800 3600 3800
Wire Wire Line
	3425 3700 3600 3700
Wire Wire Line
	5525 4575 5525 4500
Wire Wire Line
	5825 2400 6625 2400
Wire Wire Line
	4600 4850 6625 4850
Wire Wire Line
	6625 2400 6625 4450
Wire Wire Line
	7300 4025 7300 4075
Connection ~ 6625 4075
Wire Wire Line
	7500 4075 7500 4025
Connection ~ 7300 4075
Connection ~ 7500 4075
Wire Wire Line
	4900 3800 5125 3800
Wire Wire Line
	4900 3900 5125 3900
Wire Wire Line
	4900 4000 5125 4000
Wire Wire Line
	4900 4100 5125 4100
Wire Wire Line
	3425 3600 3600 3600
Wire Wire Line
	3425 3900 3600 3900
Wire Wire Line
	4900 3700 5125 3700
Wire Wire Line
	8000 3725 8250 3725
Wire Wire Line
	8000 3625 8250 3625
Wire Wire Line
	8000 3525 8250 3525
Wire Wire Line
	5725 4500 5725 4625
Wire Wire Line
	5625 4575 5625 4500
Connection ~ 5525 4575
Connection ~ 5725 4575
Connection ~ 5625 4575
Wire Wire Line
	6800 4275 6800 3725
Wire Wire Line
	6250 3400 6250 4575
Wire Wire Line
	6250 4575 6150 4575
Wire Wire Line
	4100 4575 5850 4575
Wire Wire Line
	6250 3400 6125 3400
Connection ~ 6250 4275
Wire Wire Line
	6125 3500 6200 3500
Wire Wire Line
	6200 3500 6200 4850
Connection ~ 6200 4850
Connection ~ 4650 4575
Wire Wire Line
	5825 2400 5825 2300
Wire Wire Line
	5825 2300 4100 2300
Wire Wire Line
	6550 4275 6800 4275
Wire Wire Line
	8425 3425 8000 3425
Wire Wire Line
	7500 3025 8350 3025
Wire Wire Line
	8350 3025 8350 3425
Connection ~ 8350 3425
Wire Wire Line
	8000 3325 8550 3325
Wire Wire Line
	8550 3325 8550 4075
Wire Wire Line
	8550 4075 6625 4075
Wire Bus Line
	3325 3800 3325 3350
Wire Bus Line
	3325 3350 4800 3350
Wire Bus Line
	4800 3350 4800 4925
Wire Bus Line
	8350 3625 8350 4925
Wire Bus Line
	8350 4925 4800 4925
Wire Wire Line
	4100 4200 4100 4575
Wire Wire Line
	4100 2300 4100 3300
Wire Wire Line
	4650 4575 4650 3600
Wire Wire Line
	4650 3600 4600 3600
Wire Wire Line
	4600 3700 4600 4850
$EndSCHEMATC
