EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:74xgxx
LIBS:ac-dc
LIBS:actel
LIBS:allegro
LIBS:Altera
LIBS:analog_devices
LIBS:battery_management
LIBS:bbd
LIBS:bc
LIBS:bosch
LIBS:brooktre
LIBS:cmos_ieee
LIBS:dc-dc
LIBS:diode
LIBS:elec-unifil
LIBS:ESD_Protection
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:infineon
LIBS:intersil
LIBS:ir
LIBS:Lattice
LIBS:leds
LIBS:LEM
LIBS:logo
LIBS:maxim
LIBS:mechanical
LIBS:microchip_dspic33dsc
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip_pic18mcu
LIBS:microchip_pic24mcu
LIBS:microchip_pic32mcu
LIBS:modules
LIBS:motor_drivers
LIBS:motors
LIBS:msp430
LIBS:nordicsemi
LIBS:nxp
LIBS:nxp_armmcu
LIBS:onsemi
LIBS:Oscillators
LIBS:powerint
LIBS:Power_Management
LIBS:pspice
LIBS:references
LIBS:relays
LIBS:rfcom
LIBS:RFSolutions
LIBS:sensors
LIBS:silabs
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:switches
LIBS:transf
LIBS:triac_thyristor
LIBS:ttl_ieee
LIBS:video
LIBS:wiznet
LIBS:Worldsemi
LIBS:Xicor
LIBS:zetex
LIBS:Zilog
EELAYER 25 0
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
L MCP41HV51 U?
U 1 1 5933A248
P 2800 3050
F 0 "U?" H 2500 3350 50  0000 L CNN
F 1 "MCP41HV51" H 2900 3350 50  0000 L CNN
F 2 "" H 2800 3000 50  0001 C CNN
F 3 "" H 2800 3000 50  0001 C CNN
	1    2800 3050
	1    0    0    -1  
$EndComp
$Comp
L Arduino_UNO_R3 A?
U 1 1 5933A2C6
P 4850 3150
F 0 "A?" H 4650 4200 50  0000 R CNN
F 1 "Arduino_UNO_R3" H 4650 4100 50  0000 R CNN
F 2 "Modules:Arduino_UNO_R3" H 5000 2100 50  0001 L CNN
F 3 "" H 4650 4200 50  0001 C CNN
	1    4850 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 4300 4750 4250
Wire Wire Line
	2800 4300 4750 4300
Wire Wire Line
	2900 4300 2900 3700
Wire Wire Line
	2800 4300 2800 3700
Connection ~ 2900 4300
Connection ~ 3350 4300
$Comp
L R R?
U 1 1 5933A543
P 3500 2775
F 0 "R?" V 3580 2775 50  0000 C CNN
F 1 "R" V 3500 2775 50  0000 C CNN
F 2 "" V 3430 2775 50  0001 C CNN
F 3 "" H 3500 2775 50  0001 C CNN
	1    3500 2775
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 J?
U 1 1 5933A5AF
P 3550 1900
F 0 "J?" H 3550 2050 50  0000 C CNN
F 1 "CONN_01X02" V 3650 1900 50  0000 C CNN
F 2 "" H 3550 1900 50  0001 C CNN
F 3 "" H 3550 1900 50  0001 C CNN
	1    3550 1900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3500 2100 3500 2625
Wire Wire Line
	3600 2100 3600 2150
Wire Wire Line
	3600 2150 3700 2150
Wire Wire Line
	3700 2150 3700 4300
Connection ~ 3700 4300
Wire Bus Line
	2000 2500 2000 3100
Entry Wire Line
	2000 2800 2100 2900
Entry Wire Line
	2000 2900 2100 3000
Entry Wire Line
	2000 3000 2100 3100
Entry Wire Line
	2000 3100 2100 3200
Wire Wire Line
	2100 3200 2300 3200
Wire Wire Line
	2100 3100 2300 3100
Wire Wire Line
	2100 3000 2300 3000
Wire Wire Line
	2100 2900 2300 2900
Text Label 2100 2900 0    60   ~ 0
sck
Text Label 2100 3000 0    60   ~ 0
cs
Text Label 2100 3100 0    60   ~ 0
mosi
Text Label 2100 3200 0    60   ~ 0
miso
Wire Wire Line
	4350 3550 4150 3550
Wire Wire Line
	4350 3650 4150 3650
Wire Wire Line
	4350 3750 4150 3750
Wire Wire Line
	4350 3850 4150 3850
Entry Wire Line
	4050 3450 4150 3550
Entry Wire Line
	4050 3550 4150 3650
Entry Wire Line
	4050 3650 4150 3750
Entry Wire Line
	4050 3750 4150 3850
Wire Bus Line
	4050 2500 4050 3750
Wire Bus Line
	4050 2500 2000 2500
Text Label 4150 3850 0    60   ~ 0
sck
Text Label 4150 3550 0    60   ~ 0
cs
Text Label 4150 3650 0    60   ~ 0
mosi
Text Label 4150 3750 0    60   ~ 0
miso
Wire Wire Line
	3500 2925 3500 3100
Wire Wire Line
	3500 3100 3300 3100
Wire Wire Line
	3300 3200 3350 3200
Wire Wire Line
	3350 3200 3350 4300
NoConn ~ 3300 3000
$EndSCHEMATC
