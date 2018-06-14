# Advanced Process Control for Atmospheric Pressure Plasmas

This repository has been forked and modified from [**Plasma Analysis and Control**](https://github.com/brandoncurtis/plasma-control) by Brandon Curtis

**** This repository is being updated/ Changes in organization and clean-up is due ****

Thermal image of the APPJ translating over a surface!

![Helium plasma jet](/results/moving_jet.gif)

This repository contains code used for implementing classical (proportional-integral-derivative) and advanced (optimization-based) feedback control on a kHz-exctited atmospheric pressure plasma jet in helium 

## Instrumentation 
The control system consists of embedded measurements and actuation managed by two Arduino UNO controllers. A Raspberry Pi 3 is used to coordinate the microntrollers and pheripheral instruments. The Master Arduino Manages actuation of applied voltage, frequency, duty cycle and gas flow rates via homebrewed circutiry. (See \hardware section for details on instruments and ciruit diagrams). The Master Arduino also measures RMS applied voltage and RMS current via AC-to-RMS converter circuits as well as total light intensity via a photodarlington. The x,y and z position of the jet is coordinate by the Complementry Arduino equipped with two Adafruit Motor Control Shields. See the diagram below for information flow relating to the Arduino UNOs. 

![Helium plasma jet](/results/APPJ_diagram_embedded.png)

As pheripherals, a FLIR lepton 2.5 radiometric thermal camera, RIGOL D1000Z oscilloscope and Oceanoptics optical emission spectrometer are configured. Information flow relating to the pheripheral instruments are shown below.

![Helium plasma jet](/results/APPJ_diagram_pheripherals.png)

## Firmware
The Arduino firmware allows exchanging commands with the Arduni via serial monitor. The latest version (as of 14/6/18) is version 14, available under /firmware/examples/dac/. The firmware is used for both the Master and Complementary Arduinos and is capable of managing two gas flow rates, 3D position, applied voltage and frequency. Default measurements include RMS voltage, RMS current, total emission intensity and ambient temperature.

## Software 
Detailed information about the software is under /experiments directory.Scripts are available for gathering detailed data from individual instruments under /experiments/scripts. The measure_server_PI and measure_server_OL scripts under /experiments/scripts allow for coordinated measurements from pheripherals and embedded systems simultaneously. These scripts also allow for interfacing with a websocket client to externally govern applied inputs or controller setpoints.
