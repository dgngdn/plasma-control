/**
 * control_, Brandon Curtis
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
 * RELEASE NOTES
 * 
 * === ANALOG INPUTS
 * A0  analog potentiometer output - manual control setpoint // NOT USED
 * A1  digital potentiometer output - voltage control
 * A2  digital potentiometer output - frequency control
 * A3  digital potentiometer output - flow control
 * A4  I2C data line
 * A5  I2C data line
 * 
 * NOT USED:  sensor - embedded thermocouple amp output
 * 
 * === SERIAL PRINT LOOP ===
 * time_datetime, time_millis,
 *   measured_out_volt, measured_out_freq, measured_out_flow,
 *     ctrl_out_volt, ctrl_out_freq, ctrl_out_flow,
 *       temp_surf, temp_tube, temp_embd
 *         IF PID: input, setpoint, output
 * 
 
 
 
 === FOR V016
 RTC reset needs to be updated to use syntax in new version of library
 
 * === SINCE V014 ===
 * reconfigured ADC connections
 * added active monitoring of control potentiometer outputs
 * added active monitoring of all internal setpoint outputs
 * removed embedded thermocouple reading, pending calibration
 * removed 'manual setpoint' print hack
 * removed unused define: PRINT_TEMP
 * added setpoint min/max for each variable
 * added global variable to keep track of setpoint for each variable
 * 
 * === SINCE V013 ===
 * 
 * === SINCE V012 ===
 * added potentiometers to control frequency and gas flow rate
 * (attempted to) add #defines to make code easier to read and change
 * printing more values (e.g. pot outputs) for debugging
 * 
 * === SINCE V009 ===
 * added watchdog timer to auto-restart on freezes
 * added automatic RC programming
 * PID controller appears to be broken :(
 * 
 * if PID_ON is true
 * Digital potentiometer is PID controlled
 * PID setpoint: e.g. 40C
 * PID inputs: non-contact thermal reading
 * PID output: potentiometer wiper position
 * endif
 * 
 * if PID_ON is false
 * Digital potentiometer is stepped through its range
 * e.g. 20% steps, 10 seconds per step
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
  License along with this library; if not, write to the Free Soft
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define DEBUG false
#define PID_ON false
#define WDT_ON true             // set false for v009 behavior
#define RTC_ON true
#define RTC_RESET false         // set false for v009 behavior
#define MANUAL_SETPOINT false   // set the setpoint with the potentiometer
#define RANDOM_INPUTS false

#define CTRL_VAR set_volt      
#define CTRL_VAR2 set_flow
#define CTRL_VAR3 set_freq
#define CTRL_DEV DEV_VOLT       // set the control potentiometer
#define CTRL_POT POT_VOLT       // set the control variable

#define PID_OUTPUT_MIN 1        // 1
#define PID_OUTPUT_MAX 100.0    // 92.0
#define STEP_DURATION 120       // in seconds

#define VOLT_INIT 0           // initial voltage potentiometer setting
#define VOLT_MIN 40
#define VOLT_MAX 100
#define FREQ_INIT 94
#define FREQ_MIN 1
#define FREQ_MAX 100
#define FLOW_INIT 5
#define FLOW_MAX 25
#define FLOW_MIN 4

// for input step testing, use: setVals[], setVals2[], setVals3[]

// values for control variable 1
//int setVals[] = {40,45,50,55};
int setVals[] = {0,50,70,50,0};
//int setVals[] = {0,10,20,30,40,50,60,70,80,90,100};

// values for control variable 2
int counter2 = 0;
//int setVals2[] = {3,5,7};
int setVals2[] = {10,20};

// values for control variable 3
int counter3 = 0;
int setVals3[] = {80};

///////////////////////////////////////////////
////////////////////////////////////////////
// SHOULDN'T HAVE TO EDIT FROM HERE DOWN!
////////////////////////////////////////////
///////////////////////////////////////////////

// defines which potentiometer is actuating each variable
#define POT_VOLT 1
#define POT_FREQ 2
#define POT_FLOW 2
#define DEV_VOLT digitalPot1
#define DEV_FREQ digitalPot1
#define DEV_FLOW digitalPot2

// defines the initial values for each variable
float set_value = 0;
float set_volt = VOLT_INIT;
float set_freq = FREQ_INIT;
float set_flow = FLOW_INIT;

// Set the cycle time
long timeoutInterval = STEP_DURATION*1000; // this is in milliseconds
long previousMillis = 0;
int counter = 0;

// Watchdog Timer Setup
// NEW SINCE V009
// http://www.megunolink.com/articles/how-to-detect-lockups-using-the-arduino-watchdog/
#if WDT_ON
#include <avr/wdt.h>
#endif

// when SET1 IS COMPLETE...
// iterate SET2 and repeat!
void set_complete() {
  if( counter2 >= sizeof(setVals2) / sizeof(int) ) {
    set_complete2();
    counter2 = 0;
  }
  CTRL_VAR2 = setVals2[ counter2 ];
  counter2 += 1;
}

// when SET2 IS COMPLETE...
// iterate SET3 and repeat!
void set_complete2() {
  if( counter3 >= sizeof(setVals3) / sizeof(int) ) {
    set_complete3();
    counter3 = 0;
  }
  CTRL_VAR3 = setVals3[ counter3 ];
  counter3 += 1;
}

// when SET3 IS COMPLETE...
// shut everything down and HOLD
void set_complete3() {
  CTRL_VAR = 0;
  CTRL_VAR2 = 0;
  CTRL_VAR3 = 0;
  while ( true ) {
    delay(1);
  }
}

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

#if PID_ON
#include <PID_v1.h>
// Define Variables we'll be connecting the controller to
double Setpoint, Input, Output;
double Kp = 0.00;  // 0.0 // 25.0 // 14.89 // process value control 
double Ki = 0.50;  // 0.5 // 3.0  // 2.23  // integral control
double Kd = 0.00;  // 0.0 // 0.0  // 0.0   // derivative control
// Values that generally work well:
// 0.00, 0.50, 0.00 - frequency controller
// 0.10, 0.05, 0.02 - flow controller

// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); // DIRECT or REVERSE
#endif 


/**
 * //////////////////////////
 * /// SETUP Thermocouple ///
 * //////////////////////////
 */

float celcius3 = 0;

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
// each additional SPI device needs its own CS pin
#define MCP_POT1_CS 10  //arduino   <->   Chip Select      -> CS  (Pin 01 on McpDigitalPot DIP)
#define MCP_POT2_CS 9  // for the second MCP4261

// CALIBRATE ME: end-to-end potentiometer resistance, terminal A-terminal B
float rAB_ohms = 10000.00; // 10k Ohm 

// CALIBRATE ME: rW = wiper resistance (75-160 ohms)
//McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );
// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot1 = McpDigitalPot( MCP_POT1_CS, rAB_ohms );

// Adding a second MCP4261...
McpDigitalPot digitalPot2 = McpDigitalPot( MCP_POT2_CS, rAB_ohms );

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
#include <Wire.h>

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

#if RTC_ON
#include <TimeLib.h>
#include <DS1307RTC.h>
tmElements_t tm;

#if RTC_RESET
/**
 * ///////////////////
 * /// SET THE RTC ///
 * ///////////////////
 */

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void setrtc()
{
  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
}
#endif
#endif

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
  digitalPot1.scale = 100.0;
  digitalPot2.scale = 100.0;
  
  // set potentiometer initial positions
  DEV_VOLT.setResistance( POT_VOLT, VOLT_INIT );
  DEV_FREQ.setResistance( POT_FREQ, FREQ_INIT );
  DEV_FLOW.setResistance( POT_FLOW, FLOW_INIT );

#if PID_ON
  // controlled mode
  // turn the PID on
  //myPID.SetTunings(Kp,Ki,Kd) // set the tuning parameters
  Setpoint = setVals[0];                // controller drives toward this value
  myPID.SetControllerDirection(DIRECT); // DIRECT or REVERSE
  myPID.SetOutputLimits(PID_OUTPUT_MIN,PID_OUTPUT_MAX);         // PWM output range is 0-100 (off-on)
  myPID.SetSampleTime(50);              // in milliseconds
  myPID.SetMode(AUTOMATIC);             // AUTOMATIC = on
#endif

#if DEBUG
  Serial.println("timeout called...");
#endif
  timeout(); // set the initial voltage
#if DEBUG
  Serial.println("timeout complete");
#endif

#if RTC_ON
#if RTC_RESET
  if (!RTC.read(tm)) {
#if DEBUG
    Serial.println("setting rtc...");
#endif
    setrtc();
#if DEBUG
    Serial.println("rtc reset");
#endif
  }
#endif
#endif

#if WDT_ON
  // enable the watchdog timer
  // Watchdog Timeouts: WDTO_{1,2,4,8}s
  // WDTO_{15,30,60,120,250,500}MS
  wdt_enable(WDTO_1S);
#endif
}

/**
 * ////////////////////////
 * /// GET TEMPERATURES ///
 * //////////////////////
 */

float temperatureCelcius(int address) {
#if DEBUG
  Serial.println("getting temperatures...");
#endif
  int dev = address;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  // Write
#if DEBUG
  Serial.println("writing to i2c...");
#endif
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // Read
#if DEBUG
  Serial.println("reading from i2c...");
#endif
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck();       // Read 1 byte and then send ack.
  data_high = i2c_readAck();      // Read 1 byte and then send ack.
  pec = i2c_readNak();
  i2c_stop();

  // combines high and low bytes and converts temperature 
  // MSB is an error bit and is ignored for temperatures
#if DEBUG
  Serial.println("calculating temperature...");
#endif
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
#if DEBUG
  Serial.println("starting timeout loop...");
#endif

  if( counter >= sizeof(setVals) / sizeof(int) ) {
    set_complete();
    counter = 0;
  }

  // set the wiper position
  // pot0 uses pins 8,9,10; pot1 uses pins 5,6,7

  //digitalPot.setResistance(pot#, 0-100);
  //digitalPot.setResistance(1, 100-counter); // start high and go low
  //digitalPot.setResistance(1, counter); // start low and go high
  //digitalPot.setResistance(1, 100); // fixed value
  //int setVal = setVals[random(0,sizeof(setVals)/sizeof(int))];
  //digitalPot.setResistance(1,setVal); // value from selection                                                                   
  //counter += 10.0; // change in increments of 10

  // choose the setpoint for the next step
#if MANUAL_SETPOINT
  // get setpoint from potentiometer reading
  set_value = analogRead(A0) / 10.;
#else
  set_value = setVals[ counter ];
#endif

#if PID_ON // CLOSED LOOP
  Setpoint = set_value;
#elif RANDOM_INPUTS
  set_volt = random( VOLT_MIN, VOLT_MAX );
  set_freq = random( FREQ_MIN, FREQ_MAX );
  set_flow = random( FLOW_MIN, FLOW_MAX );
#else
  CTRL_VAR = set_value;
  
  // actually set the output values
  DEV_VOLT.setResistance( POT_VOLT, set_volt );
  DEV_FREQ.setResistance( POT_FREQ, set_freq );
  DEV_FLOW.setResistance( POT_FLOW, set_flow );
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
#if DEBUG
  Serial.println("starting main loop...");
#endif
  // reset the watchdog timer
  // if this doesn't occur within WDTO_X time
  // the system will reset
#if WDT_ON
#if DEBUG
  Serial.println("resetting wdt...");
#endif
  wdt_reset();
#if DEBUG
  Serial.println("wdt reset");
#endif
#endif
  
  celcius1 = temperatureCelcius(device1Address);// Reads data from MLX90614
  celcius2 = temperatureCelcius(device2Address);// with the given address,
                                                // converts to °C and stores
#if PID_ON
  // traversed only in controlled mode
  Input = celcius1; //celcius1 + celcius2;
  
  if ( myPID.Compute() ) {
    // digitalPot.setResistance(pot#, 0-100);
    CTRL_POT.setResistance( CTRL_VAR, Output ); // this is what the PID is controlling
  }
#endif

  // traversed in controlled AND uncontrolled mode
  // timeout loop for time-triggered adjustments
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }

  // Read the input from the potentiometer wiper at A1
  // voltage ctrl output value
  int volt_out = analogRead(A1);
  // Read the input from the potentiometer wiper at A2
  // flow ctrl output value
  int freq_out = analogRead(A2);
  // Read the input from the potentiometer wiper at A3
  // flow ctrl output value
  int flow_out = analogRead(A3);
  
// 2016-07-04 COMMENTED OUT UNTIL THIS IS CALIBRATED AND CONDITIONED PROPERLY
  // Read the analog input from the thermocouple amp
  //celcius3 = 21.5 + (analogRead(A2)*5/1024.-1.1035)/0.0148;

#if RTC_ON
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
    Serial.print(',');
    print3digits(millis()%1000);
    Serial.print(",");
    //int endtime = micros(); //END TIMING
  }
#endif

  // PRINT MEASURED CONTROL OUTPUTS
  Serial.print(volt_out);
  Serial.print(",");
  Serial.print(freq_out);
  Serial.print(",");
  Serial.print(flow_out);
  Serial.print(",");
  
  // PRINT CONTROL OUTPUTS
  Serial.print(set_volt);
  Serial.print(",");
  Serial.print(set_freq);
  Serial.print(",");
  Serial.print(set_flow);
  Serial.print(",");

  // PRINT MEASURED VALUES
  Serial.print(celcius1);
  Serial.print(",");
  Serial.print(celcius2);
  Serial.print(",");
  Serial.print(celcius3);

#if PID_ON
  Serial.print(",");
  Serial.print(Input);
  Serial.print(",");
  Serial.print(Setpoint);
  Serial.print(",");
  Serial.print(Output);

#endif
  Serial.println();
  
  //delay(1);                         // pause before looping
}
