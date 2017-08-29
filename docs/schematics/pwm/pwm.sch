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
LIBS:bc
LIBS:pwm-cache
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
L 4066 U?
U 1 1 5935F1B5
P 4150 1950
F 0 "U?" H 4350 1801 50  0000 C CNN
F 1 "4066" H 4350 2100 50  0000 C CNN
F 2 "" H 4150 1950 60  0001 C CNN
F 3 "" H 4150 1950 60  0001 C CNN
	1    4150 1950
	1    0    0    -1  
$EndComp
$Comp
L 4066 U?
U 2 1 5935F1EC
P 4150 2700
F 0 "U?" H 4350 2551 50  0000 C CNN
F 1 "4066" H 4350 2850 50  0000 C CNN
F 2 "" H 4150 2700 60  0001 C CNN
F 3 "" H 4150 2700 60  0001 C CNN
	2    4150 2700
	1    0    0    -1  
$EndComp
$Comp
L LM339 U?
U 1 1 59365433
P 3100 2100
F 0 "U?" H 3100 2300 50  0000 L CNN
F 1 "LM339" H 3100 1900 50  0000 L CNN
F 2 "" H 3050 2200 50  0001 C CNN
F 3 "" H 3150 2300 50  0001 C CNN
	1    3100 2100
	1    0    0    -1  
$EndComp
$Comp
L LM339 U?
U 2 1 59365452
P 3100 2850
F 0 "U?" H 3100 3050 50  0000 L CNN
F 1 "LM339" H 3100 2650 50  0000 L CNN
F 2 "" H 3050 2950 50  0001 C CNN
F 3 "" H 3150 3050 50  0001 C CNN
	2    3100 2850
	1    0    0    -1  
$EndComp
$Comp
L Arduino_UNO_R3 A?
U 1 1 5936E9C3
P 1700 2050
F 0 "A?" H 1500 3100 50  0000 R CNN
F 1 "Arduino_UNO_R3" H 1500 3000 50  0000 R CNN
F 2 "Modules:Arduino_UNO_R3" H 1850 1000 50  0001 L CNN
F 3 "" H 1500 3100 50  0001 C CNN
	1    1700 2050
	1    0    0    -1  
$EndComp
$Comp
L +9V #PWR?
U 1 1 5936EA19
P 2525 1250
F 0 "#PWR?" H 2525 1100 50  0001 C CNN
F 1 "+9V" H 2525 1390 50  0000 C CNN
F 2 "" H 2525 1250 50  0001 C CNN
F 3 "" H 2525 1250 50  0001 C CNN
	1    2525 1250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5936EA37
P 2525 1750
F 0 "#PWR?" H 2525 1500 50  0001 C CNN
F 1 "GND" H 2525 1600 50  0000 C CNN
F 2 "" H 2525 1750 50  0001 C CNN
F 3 "" H 2525 1750 50  0001 C CNN
	1    2525 1750
	1    0    0    -1  
$EndComp
$Comp
L R_Small R2.2k
U 1 1 5936EAC1
P 2525 1350
F 0 "R2.2k" H 2555 1370 50  0000 L CNN
F 1 "R_Small" H 2555 1310 50  0000 L CNN
F 2 "" H 2525 1350 50  0001 C CNN
F 3 "" H 2525 1350 50  0001 C CNN
	1    2525 1350
	1    0    0    -1  
$EndComp
$Comp
L R_Small R8.2k
U 1 1 5936EAFE
P 2525 1650
F 0 "R8.2k" H 2555 1670 50  0000 L CNN
F 1 "R_Small" H 2555 1610 50  0000 L CNN
F 2 "" H 2525 1650 50  0001 C CNN
F 3 "" H 2525 1650 50  0001 C CNN
	1    2525 1650
	1    0    0    -1  
$EndComp
$Comp
L +9V #PWR?
U 1 1 5936EE43
P 3000 1800
F 0 "#PWR?" H 3000 1650 50  0001 C CNN
F 1 "+9V" H 3000 1940 50  0000 C CNN
F 2 "" H 3000 1800 50  0001 C CNN
F 3 "" H 3000 1800 50  0001 C CNN
	1    3000 1800
	1    0    0    -1  
$EndComp
$Comp
L +9V #PWR?
U 1 1 5936EE82
P 2800 2550
F 0 "#PWR?" H 2800 2400 50  0001 C CNN
F 1 "+9V" H 2800 2690 50  0000 C CNN
F 2 "" H 2800 2550 50  0001 C CNN
F 3 "" H 2800 2550 50  0001 C CNN
	1    2800 2550
	1    0    0    -1  
$EndComp
$Comp
L -9V #PWR?
U 1 1 5936F071
P 3000 3150
F 0 "#PWR?" H 3000 3000 50  0001 C CNN
F 1 "-9V" H 3000 3290 50  0000 C CNN
F 2 "" H 3000 3150 50  0001 C CNN
F 3 "" H 3000 3150 50  0001 C CNN
	1    3000 3150
	-1   0    0    1   
$EndComp
$Comp
L -9V #PWR?
U 1 1 5936F0D0
P 3150 2400
F 0 "#PWR?" H 3150 2250 50  0001 C CNN
F 1 "-9V" H 3150 2540 50  0000 C CNN
F 2 "" H 3150 2400 50  0001 C CNN
F 3 "" H 3150 2400 50  0001 C CNN
	1    3150 2400
	-1   0    0    1   
$EndComp
Text GLabel 3625 1475 0    60   Input ~ 0
FXGEN_OUT
Text GLabel 4725 2350 2    60   Output ~ 0
AMP_IN
Wire Wire Line
	4450 1950 4550 1950
Wire Wire Line
	4550 1950 4550 2700
Wire Wire Line
	4550 2700 4450 2700
Wire Wire Line
	4725 2350 4550 2350
Connection ~ 4550 2350
Wire Wire Line
	3400 2100 3850 2100
Wire Wire Line
	3400 2850 3850 2850
Wire Wire Line
	1200 1650 1000 1650
Wire Wire Line
	1000 1650 1000 3400
Wire Wire Line
	1000 3400 2700 3400
Wire Wire Line
	2700 3400 2700 2000
Wire Wire Line
	2700 2000 2800 2000
Wire Wire Line
	2800 2950 2700 2950
Connection ~ 2700 2950
Wire Wire Line
	2525 1550 2525 1450
Wire Wire Line
	2525 1475 2425 1475
Wire Wire Line
	2425 1475 2425 2750
Connection ~ 2525 1475
Wire Wire Line
	2425 2750 2800 2750
Wire Wire Line
	2800 2200 2425 2200
Connection ~ 2425 2200
Wire Wire Line
	2800 2550 3000 2550
Wire Wire Line
	3625 1475 3850 1475
Wire Wire Line
	3850 1475 3850 2700
Connection ~ 3850 1950
Wire Wire Line
	3000 2400 3150 2400
$EndSCHEMATC
