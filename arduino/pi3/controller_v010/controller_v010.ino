/**
 * control_v009, Brandon Curtis
 * this code is public domain
 * status: TESTED; WORKS AS DESCRIBED
 * hardware requirements:
 * 2x IR thermal sensors (Melexis LX90614), I2C
 * 1x digital potentiometer (Microchip MCP4261), SPI
 * 1x real-time clock, DS1307 or DS3121 -compatible
 * Thermal sensors must have unique addresses on the i2c bus
 * (this is accomplished with another program)
 * Temperature (degC) is read from both sensors.
 * DEFINE statements switch on and off a PID controller
 * 
 * if PID_CONTROL is true
 * Digital potentiometer is PID controlled
 * PID setpoint: 60C
 * PID inputs: _sum_ of the thermal readings
 * PID output: potentiometer wiper position
 * endif
 * 
 * if PID_CONTROL is falselhost
 * Digital potentiometer is stepped through its range
 * 20% steps, 10 seconds per step
 * endif
 * 
 * potentiometer wiper voltage read at pin A3
 * print: thermal1_degC,thermal2_degC,A3
 * 
 * This software is based on libraries licensed with the GNU
   Lesser General Public License (v2.1+).

  This work is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define PID_CONTROL false

// Set the cycle time
long timeoutInterval = 20000; //300000; // this is in milliseconds
long previousMillis = 0;
int counter = 1;

// Watchdog Timer Setup
// http://www.megunolink.com/articles/how-to-detect-lockups-using-the-arduino-watchdog/
#include <avr/wdt.h>

/**
 * /////////////////
 * /// SETUP PID ///
 * /////////////////
 * Configure the process-integral-derivative controller
 * Arduino library written by Brett Beauregard
 * http://playground.arduino.cc/Code/PIDLibary
 * https://github.com/br3ttb/Arduino-PID-Library/
 * http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
 */

#if PID_CONTROL
#include <PID_v1.h>
// Define Variables we'll be connecting the controller to
double Setpoint, Input, Output;
double Kp = 0.1; // 0.1, process value control 
double Ki = 1.0; // 1.0, integral control
double Kd = 0.0; // 0.0, derivative control
// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); // DIRECT or REVERSE
// controlled mode, setpoints
int setVals[] = {20,70,20,70,30,70,40,70,50,70,60,70,20,30,20,40,20,50,20,60,20,70};

#else
// uncontrolled mode, applied voltage
//int setVals[] = {0,100,0,100,50,100,60,100,70,100,80,100,90,100,50,0,60,0,70,0,80,0,90,0,100};  // step analysis
//int setVals[] = {0,0,100,95,90,85,80,75,70,65,60,55,50,45,40,35,40,45,50,55,60,65,70,75,80,85,90,95,100};  // power
int setVals[] = {100,98,96,94,92,90,88,86,84,82,80,78,76,74,72,70,68,66,64,62,60,58,56,54,52,50,48,46,44,42,40,38,36,34,32};  // power
//int setVals[] = {75};
#endif


/**
 * /////////////////////
 * /// SETUP MCP4261 ///
 * /////////////////////
 * Configured the Microchip digital potentiometers
 * Uses the Spi library by Cam Thompson, originally part of FPU library (micromegacorp.com)
 * from http://arduino.cc/playground/Code/Fpu or http://www.arduino.cc/playground/Code/Spi
 * Including SPI.h vv below initializea the MOSI, MISO, and SPI_CLK pins as per ATMEGA 328P
 */
#include <SPI.h>

// McpDigitalPot library available from https://github.com/teabot/McpDigitalPot
// Adapted from https://github.com/dreamcat4/McpDigitalPot
#include <McpDigitalPot.h>

// Wire up the SPI Interface common lines:
// #define SPI_CLOCK            13 //arduino <-> SPI Slave Clock Input   -> SCK (Pin 02 on McpDigitalPot DIP)
// #define SPI_MOSI             11 //arduino <-> SPI Master Out Slave In -> SDI (Pin 03 on McpDigitalPot DIP)
// #define SPI_MISO             12 //arduino <-> SPI Master In Slave Out -> SDO (Pin 13 on McpDigitalPot DIP)

// Then choose any other free pin as the Slave Select (default pin 10)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select      -> CS  (Pin 01 on McpDigitalPot DIP)

// CALIBRATE ME: end-to-end potentiometer resistance, terminal A-terminal B
float rAB_ohms = 10000.00; // 10k Ohm 

// CALIBRATE ME: rW = wiper resistance (75-160 ohms)
//McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );
// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );


/**
 * //////////////////////
 * /// SETUP MLX90614 ///
 * //////////////////////
 * Configure the Melexis IR thermal sensors
 * based on "two infrared thermometers", by Jaime Patarroyo
 * based on "Is it hot? Arduino + MLX90614 IR Thermometer" by bildr.blog
 * i2c bus device address: 0x5A
 * bus addresses modified with software based on 'remap_mlx90614' by Chris Ramsay
 * http://www.chrisramsay.co.uk/posts/2014/09/arduino-and-multiple-mlx90614-sensors/
 *
 * Returns the temperature in Celcius from two MLX90614 Infrared Thermometers,
 * connected to the TWI/I²C pins (on the Wiring v1 board 0 (SCL) and 1 (SDA)
 * and on Wiring S board 8 (SCL) and 9 (SDA)).
 */

#include <i2cmaster.h>

int device1Address = 0x5A<<1;   // 0x5A is the assigned address for I²C 
                                // communication for sensor 1.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.
int device2Address = 0x5B<<1;   // 0x5B is the assigned address for I²C 
                                // communication for sensor 2.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.

float celcius1 = 0;             // Variable for degC, sensor 1
float celcius2 = 0;             // Variable for degC, sensor 2


/**
 * /////////////////
 * /// SETUP RTC ///
 * /////////////////
 * Configure the temperature-compensated realtime clock
 * DS1307RTC library v1.4, by Michael Margolis; maintained by Paul Stoffregen
 * utilizes the Arduino "Time" library
 * http://playground.arduino.cc/code/time
 * This is GPL software; license details in the header
 * i2c bus device address: 0x68
 */

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

/**
 * //////////////////////
 * /// SETUP FUNCTION ///
 * //////////////////////
 */

void setup()
{
  Serial.begin(57600); // allowed: 4800,9600,14400,19200,28800,57600,115200,0.5M,1.0M,2.0M
  
  i2c_init();                               // Initialise the i2c bus.
  PORTC = (1 << PORTC4) | (1 << PORTC5);    // Enable pullups.
  
  // initialize SPI:
  SPI.begin();
  
  // (optional)
  // Set the scaling method, 100.0 for percentages, 1.0 for fractions
  digitalPot.scale = 100.0;

#if PID_CONTROL
  // controlled mode
  // turn the PID on
  //myPID.SetTunings(Kp,Ki,Kd) // set the tuning parameters
  Setpoint = setVals[0];                // controller drives toward this value
  myPID.SetControllerDirection(DIRECT); // DIRECT or REVERSE
  myPID.SetOutputLimits(0,100);         // PWM output range is 0-100 (off-on)
  myPID.SetSampleTime(50);              // in milliseconds
  myPID.SetMode(AUTOMATIC);             // AUTOMATIC = on
#endif

  timeout(); // set the initial voltage
  
  // enable the watchdog timer
  // Watchdog Timeouts: WDTO_{1,2,4,8}s
  // WDTO_{15,30,60,120,250,500}MS
  wdt_enable(WDTO_1S);
}

/**
 * ////////////////////////
 * /// GET TEMPERATURES ///
 * //////////////////////
 */

float temperatureCelcius(int address) {
  int dev = address;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  // Write
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // Read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck();       // Read 1 byte and then send ack.
  data_high = i2c_readAck();      // Read 1 byte and then send ack.
  pec = i2c_readNak();
  i2c_stop();

  // This converts high and low bytes together and processes temperature, 
  // MSB is a error bit and is ignored for temps.
  double tempFactor = 0.02;       // 0.02 degrees per LSB (measurement 
                                  // resolution of the MLX90614).
  double tempData = 0x0000;       // Zero out the data
  int frac;                       // Data past the decimal point

  // This masks off the error bit of the high byte, then moves it left 
  // 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;
  float celcius = tempData - 273.15;
  
  // Returns temperature in Celcius.
  return celcius;
}

/**
 * ///////////////////////////////////
 * /// ZERO-PAD TIMESTAMP ELEMENTS ///
 * ///////////////////////////////////
 */

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void print3digits(int number) {
  if (number >= 0 && number < 100) {
    Serial.write('0');
  }
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

/**
 * //////////////////////////////////
 * /// TIME-TRIGGERED ADJUSTMENTS ///
 * ////////////////////////////////
 */

void timeout()
{
  // set the wiper position
  // pot0 uses pins 8,9,10; pot1 uses pins 5,6,7

  //digitalPot.setResistance(pot#, 0-100);
  //digitalPot.setResistance(1, 100-counter); // start high and go low
  //digitalPot.setResistance(1, counter); // start low and go high
  //digitalPot.setResistance(1, 100); // fixed value
  //int setVal = setVals[random(0,sizeof(setVals)/sizeof(int))];
  //digitalPot.setResistance(1,setVal); // value from selection                                                                   
  //counter += 10.0; // change in increments of 10
  
  if( counter >= sizeof(setVals) / sizeof(int) ) {
    counter = 0;
  }
  
#if PID_CONTROL
  Setpoint = setVals[ counter ];
#else
  digitalPot.setResistance( 1, setVals[counter] );
#endif
  counter += 1;
}

/**
 * /////////////////
 * /// MAIN LOOP ///
 * /////////////////
 */

void loop()
{
  // rest the watchdog timer
  // if this doesn't occur within WDTO_X time
  // the system will reset
  wdt_reset();
  
  celcius1 = temperatureCelcius(device1Address);// Reads data from MLX90614
  celcius2 = temperatureCelcius(device2Address);// with the given address,
                                                // converts to °C and stores
#if PID_CONTROL
  // traversed only in controlled mode
  Input = celcius2; //celcius1 + celcius2;
  
  if ( myPID.Compute() ) {
    // digitalPot.setResistance(pot#, 0-100);
    digitalPot.setResistance(1, Output); // this is what the PID is controlling
  }
#endif

  // traversed in controlled AND uncontrolled mode
  // timeout loop for time-triggered adjustments
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }

  // Read the input from the potentiometer wiper at A3
  int sensorValue = analogRead(A3);

  // TIMESTAMP AND PRINT over the serial interface  
  tmElements_t tm;
  if (RTC.read(tm)) {
    //int starttime = micros(); //START TIMING
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print('-');
    print2digits(tm.Month);
    Serial.print('-');
    print2digits(tm.Day);
    Serial.print('_');
    print2digits(tm.Hour);
    Serial.print(':');
    print2digits(tm.Minute);
    Serial.print(':');
    print2digits(tm.Second);
    Serial.print('.');
    print3digits(millis()%1000);
    Serial.print(",");
    //int endtime = micros(); //END TIMING
  }
  Serial.print(celcius1);
  Serial.print(",");
  Serial.print(celcius2);
  Serial.print(",");
  Serial.print(sensorValue);
#if PID_CONTROL
  Serial.print(",");
  Serial.print(Input);
  Serial.print(",");
  Serial.print(Setpoint);
  Serial.print(",");
  Serial.print(Output);
#endif
  Serial.println();

  //delay(100);                         // pause before looping
}
