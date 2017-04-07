char junk;

// IMPLEMENTATION SCHEDULE
//
// OPEN LOOP
// implement: clean up function generator
//
// CLOSED LOOP
// implement: read setpoints remotely
// implement: read setpoints locally from PID
// implement: PID calculations
//
// WILL NOT IMPLEMENT
// some other software will set the RTC
// not dealing with thermocouples at the moment

// IMPORT LIBRARIES
#include <avr/wdt.h>   //#include "lib/avr/wdt.h" // #include <avr/wdt.h>
#include <PID_v1.h>    //"lib/PID/PID_v1.h" // #include <PID_v1.h>
#include <SPI.h>       // #include "lib/SPI/src/SPI.h" // #include <SPI.h>
#include <McpDigitalPot.h> // "lib/mcpdigitalpot/McpDigitalPot.h" //#include <McpDigitalPot.h>
#include <Wire.h>      // #include "lib/Wire/src/Wire.h" // #include <Wire.h>
#include <i2cmaster.h> // "lib/i2cmaster/i2cmaster.h" // #include <i2cmaster.h>
                       //#include "lib/time/TimeLib.h" // #include <TimeLib.h>
#include <DS1307RTC.h> // "lib/DS1307RTC/DS1307RTC.h" // #include <DS1307RTC.h>

// SYSTEM RUN SETTINGS
#define DEBUG false
const unsigned long STEP_DURATION_SEC = 30; // in seconds
char ctrl_measured = 'p';
#define CTRL_VAR 'v'



// DATA STORAGE VARIABLES
namespace data {
  float temp_amb = 0;
  float temp_tube = 0;
  float temp_surf = 0;
  float photo_i = 0;
}

// system setpoints with physical meaning
namespace setpoint {
  int index = 0;

  float volt = 0;
  float freq = 0;
  float flow = 0;

  float traj_volt[] = {0.0}; // {0}; // in kilovolts
  float traj_freq[] = {0.0}; // {0}; // in kilohertz
  float traj_flow[] = {0.0}; // {0}; // in slm

  //float pid = 0;
  float traj_pid[] = {0.00,0.05,0.10,0.15,0.20,0.25};
}

// internal representation of the setpoints
namespace setpoint_internal {
  float volt = 0;
  float freq = 0;
  float flow = 0;
}

// measured values from the setpoint potentiometers
namespace setpoint_meas {
  float volt = 0;
  float freq = 0;
  float flow = 0;
}

float time_ms = 0;
//float tstamp = 0;
unsigned long previousMillis = 0;
float *mydata[] = {&data::photo_i,
  &data::temp_tube, &data::temp_surf,
  &setpoint::volt, &setpoint::freq, &setpoint::flow,
  // &setpoint_internal::volt, &setpoint_internal::freq, &setpoint_internal::flow,
  // &setpoint_meas::volt, &setpoint_meas::freq, &setpoint_meas::flow,
  &data::temp_amb, &setpoint_meas::flow, &setpoint_internal::flow };

// GLOBAL CONSTANTS
const int THERMOPILE1_ADDRESS = 0x5A<<1;
const int THERMOPILE2_ADDRESS = 0x5B<<1;
const int R_PHOTO = 2200;       // ohms
const int LOOP_DELAY = 10;      // milliseconds
const long SERIAL_BAUD = 38400; // 4800,9600,14400,19200,38400,57600,115200,0.5M,1.0M,2.0M
unsigned long timeout_interval = STEP_DURATION_SEC*1000.;
bool timed_advance = false;
#define PIN_PHOTO A0
#define PIN_TEMPAMB A1
//#define PIN_VOLT_MEAS A1
#define PIN_FREQ_MEAS A2
#define PIN_FLOW_MEAS A3

// PIN DESCRIPTION
// // Read the input from the potentiometer wiper at A1
// // voltage ctrl output value
// int volt_out = analogRead(A1);
// // Read the input from the potentiometer wiper at A2
// // flow ctrl output value
// int freq_out = analogRead(A2);
// // Read the input from the potentiometer wiper at A3
// // flow ctrl output value
// int flow_out = analogRead(A3);
// * DISCONNECTED - analog potentiometer output - manual control setpoint
// * DISCONNECTED - analog ambient temperature, #define PIN_TEMPAMB
// * A0  photodarlington
// * A1  digital potentiometer output - voltage control
// * A2  digital potentiometer output - frequency control
// * A3  digital potentiometer output - flow control
// * A4  I2C data line
// * A5  I2C data line

// HARDWARE CONFIGURATION DESCRIPTIONS
// defines which potentiometer is actuating each variable
#define POT_VOLT 1
#define POT_FREQ 2
#define POT_FLOW 1
#define DEV_VOLT digitalPot1
#define DEV_FREQ digitalPot1
#define DEV_FLOW digitalPot2
// declaring stuff so it's global (bad constructors)
McpDigitalPot digitalPot1 = McpDigitalPot( 10, 10000 );
McpDigitalPot digitalPot2 = McpDigitalPot( 9, 10000 );



// THIS STUFF MUST BE SWITCHED AROUND SO THAT
// CTRL_VAR CAN BE CHANGED ON THE FLY!

#if CTRL_VAR == 'v'
  #define CTRL_SETPOINT setpoint::volt
  #define CTRL_SETPOINT_INTERNAL setpoint_internal::volt
  #define CTRL_POT POT_VOLT
  #define CTRL_DEV DEV_VOLT
  #define CTRL_OUT_MIN 0
  #define CTRL_OUT_MAX 10
#endif

#if CTRL_VAR == 'f'
  #define CTRL_SETPOINT setpoint::freq
  #define CTRL_SETPOINT_INTERNAL setpoint_internal::freq
  #define CTRL_POT POT_FREQ
  #define CTRL_DEV DEV_FREQ
  #define CTRL_OUT_MIN 10
  #define CTRL_OUT_MAX 20
#endif

#if CTRL_VAR == 'q'
  #define CTRL_SETPOINT setpoint::flow
  #define CTRL_SETPOINT_INTERNAL setpoint_internal::flow
  #define CTRL_POT POT_FLOW
  #define CTRL_DEV DEV_FLOW
  #define CTRL_OUT_MIN 0
  #define CTRL_OUT_MAX 3.5
#endif




// FUNCTIONS
// prints the data via serial
template<typename T, size_t N>
void dataprint(T (&arr)[N]) {
  // https://stackoverflow.com/q/4839626/element-count-of-an-array
  // https://stackoverflow.com/q/16505376/pass-array-to-function-template
  // https://arduino.land/FAQ/content/6/32/en/passing-arrays-to-functions.html
  // https://stackoverflow.com/q/22083676/array-of-variables-arduino
  size_t items = N;
  int i;
  for (i = 0; i < items; i++) {
    Serial.print(*arr[i]);
    Serial.print(',');
  }
  Serial.println();
}

// set up serial connection
void setup_serial() {
  // initialize serial communication at specified baudrate
  Serial.begin(SERIAL_BAUD);
}

// set up watchdog timer
void setup_watchdog() {
  // enable the watchdog timer
  // if Arduino stops responding, auto-resets
  // Watchdog Timeouts: WDTO_{1,2,4,8}s
  // WDTO_{15,30,60,120,250,500}MS
  // http://www.megunolink.com/articles/how-to-detect-lockups-using-the-arduino-watchdog/
  wdt_enable(WDTO_8S);
}

// set up the realtime clock
void setup_rtc() {
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
 }

// set up the PID controller
//void setup_pid() {
//}
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

// Define Variables we'll be connecting the controller to
double SetpointPID, Input, Output;
double Kp = 0.00;  // 0.0 // 25.0 // 14.89 // process value control
double Ki = 0.05;   // 0.5 // 3.0  // 2.23  // integral control
double Kd = 0.00;  // 0.0 // 0.0  // 0.0   // derivative control
// Values that generally work well:
// 0.00, 0.50, 0.00 - frequency controller
// 0.10, 0.05, 0.02 - flow controller

// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &SetpointPID, Kp, Ki, Kd, DIRECT); // DIRECT or REVERSE

// set up Melexis thermopiles
void setup_mlx() {
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

  // Initialise the i2c bus
  i2c_init();
  // Enable pullup resistors
  PORTC = (1 << PORTC4) | (1 << PORTC5);
}

// set up the MCP4261 digital potentiometers
void setup_pots() {
  /**
   * /////////////////////
   * /// SETUP MCP4261 ///
   * /////////////////////
   * Configured the Microchip digital potentiometers
   * Uses the Spi library by Cam Thompson, originally part of FPU library (micromegacorp.com)
   * from http://arduino.cc/playground/Code/Fpu or http://www.arduino.cc/playground/Code/Spi
   * Including SPI.h vv below initializea the MOSI, MISO, and SPI_CLK pins as per ATMEGA 328P
   */

   // McpDigitalPot library available from https://github.com/teabot/McpDigitalPot
   // Adapted from https://github.com/dreamcat4/McpDigitalPot
   // Wire up the SPI Interface common lines:
   // #define SPI_CLOCK            13 //arduino <-> SPI Slave Clock Input   -> SCK (Pin 02 on McpDigitalPot DIP)
   // #define SPI_MOSI             11 //arduino <-> SPI Master Out Slave In -> SDI (Pin 03 on McpDigitalPot DIP)
   // #define SPI_MISO             12 //arduino <-> SPI Master In Slave Out -> SDO (Pin 13 on McpDigitalPot DIP)

   // Then choose any other free pin as the Slave Select (default pin 10)
   // each additional SPI device needs its own CS pin
   #define MCP_POT1_CS 10  //arduino   <->   Chip Select      -> CS  (Pin 01 on McpDigitalPot DIP)
   #define MCP_POT2_CS 9   // for the second MCP4261

   // CALIBRATE ME: end-to-end potentiometer resistance, terminal A-terminal B
   float rAB_ohms = 10000.00; // 10k Ohm

   // CALIBRATE ME: rW = wiper resistance (75-160 ohms)
   //McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );
   // Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
   digitalPot1 = McpDigitalPot( MCP_POT1_CS, rAB_ohms );

   // Adding a second MCP4261...
   digitalPot2 = McpDigitalPot( MCP_POT2_CS, rAB_ohms );

   // initialize SPI:
   SPI.begin();

   // (optional)
   // Set the scaling method, 100.0 for percentages, 1.0 for fractions
   digitalPot1.scale = 100.0;
   digitalPot2.scale = 100.0;

   // set potentiometer initial positions
   //DEV_VOLT.setResistance( POT_VOLT, VOLT_INIT );
   //DEV_FREQ.setResistance( POT_FREQ, FREQ_INIT );
   //DEV_FLOW.setResistance( POT_FLOW, FLOW_INIT );
}

// gets manual input from the serial port
void get_serial() {
  #if DEBUG
    Serial.println("getting serial input...");
  #endif

  String inputString = "";         // a string to hold incoming data
  boolean stringComplete = false;  // whether the string is complete

  while (Serial.available() > 0) {

    // get the new byte:
    char inChar = (char)Serial.read();

    #if DEBUG
      Serial.println(String(inChar));
    #endif

    //if (isDigit(inChar)) {
      // add it to the inputString:
      inputString += inChar;
    //}
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      break;
    }
  }

  //Serial.println(inputString);
  //inputString = "";
  //stringComplete = false;
  //Serial.println(inputString);
  //Serial.println(inputString.toInt());

  if ( stringComplete == true && inputString.length() > 0 ) {
    #if DEBUG
      Serial.println("got serial input!");
    #endif
    manual_input(inputString);
  }

  //return inputString.toInt();
}

// process the manual request recieved via serial
void manual_input(String input) {
  #if DEBUG
    Serial.println("processing serial input!");
  #endif

  switch( input.charAt(0) ) {
    case 'p' :
      // you sent PAUSE
      timed_advance = false;
      #if DEBUG
        Serial.println("paused!");
      #endif
      break;

    case 'r' :
      // you sent RESUME
      timed_advance = true;
      #if DEBUG
        Serial.println("resumed!");
      #endif
      break;

    case 'd' :
      // you sent d,###
      timeout_interval = input.substring(2).toInt()*1000;
      #if DEBUG
        Serial.println("delay set!");
      #endif
      break;

    case 'b' :
      // you sent BEGIN
      setpoint::index=0;
      #if DEBUG
        Serial.println("begin!");
      #endif
      break;

    case 'v' :
      // you sent v,###
      setpoint::volt = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("voltage set!");
      #endif
      actuate_inputs();
      break;

    case 'f' :
      // you sent f,###
      setpoint::freq = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("frequency set!");
      #endif
      actuate_inputs();
      break;

    case 'q' :
      // you sent q,###
      setpoint::flow = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("flowrate set!");
      #endif
      actuate_inputs();
      break;

    case 's' :
      // you sent s,<char>,###
      if (input.charAt(2) == 's') {
        SetpointPID = input.substring(4).toFloat();
        #if DEBUG
          Serial.println("setpoint set!");
        #endif
      }

      if (input.charAt(2) == 'p') {
        Kp = input.substring(4).toFloat();
        myPID.SetTunings(Kp,myPID.GetKi(),myPID.GetKd());
        #if DEBUG
          Serial.println("Kp set!");
        #endif
      }

      if (input.charAt(2) == 'i') {
        Ki = input.substring(4).toFloat();
        myPID.SetTunings(myPID.GetKp(),Ki,myPID.GetKd());
        #if DEBUG
          Serial.println("Ki set!");
        #endif
      }

      if (input.charAt(2) == 'd') {
        Kd = input.substring(4).toFloat();
        myPID.SetTunings(myPID.GetKp(),myPID.GetKi(),Kd);
        #if DEBUG
          Serial.println("Kd set!");
        #endif
      }

      if (input.charAt(2) == 'm') {
        ctrl_measured = input.charAt(4);
        #if DEBUG
          Serial.println("control_measured set!");
        #endif
      }
      //
      // if (input.charAt(2) == 'c') {
      //   Kd = input.substring(4);
      //   #if DEBUG
      //     Serial.println("control_changed set!");
      //   #endif
      // }

      break;

    case 'o' :
      // you sent o,###
      Output = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("output set!");
      #endif
      break;

    case 'c' :
      // you sent c
      if (myPID.GetMode() != 0) { // if PID is currently on....
        myPID.SetMode(0);         // turn PID off
      }
      else {                     // but if PID controller currently off...
        Output = CTRL_SETPOINT;  // reset PID output to current setpoint
        myPID.SetMode(1);        // turn PID on
        Output = CTRL_SETPOINT;  // reset PID output to current setpoint
      }
      //myPID.SetMode(!myPID.GetMode());

      #if DEBUG
        Serial.println("control toggled!");
      #endif
      break;
    }
}

// UTILITY: ZERO-PAD TO TWO DIGITS AND PRINT
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

// get the timestamp (seconds since the epoch)
void print_tstamp() {
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
    Serial.print(millis());
    Serial.print(",");
    Serial.print(myPID.GetMode());
    Serial.print(",");
    //int endtime = micros(); //END TIMING
  }

  //Serial.print(RTC.get());
  //Serial.print(',');
  //Serial.print(millis());
  //Serial.print(',');
}

// retrieves temperatures from Melexis i2c thermopile
float get_tempDeg_mlx(int address) {
  #if DEBUG
    Serial.println("getting temperature from thermopile...");
  #endif
  int dev = address;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  // Write to the device to request the data
  #if DEBUG
    Serial.println("writing request to i2c...");
  #endif
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // Read the data from the device
  #if DEBUG
    Serial.println("reading data from i2c...");
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

// converts a2d_temp for lm35 to degC
float a2d_to_tempDeg_lm35(int a2d_temp) {
  float temp_c = a2d_temp*(3.3/1024.)*(1000./10.);
  return temp_c;
  //return a2d_temp;
}

// converts a2d_photo to photoI, uA
float a2d_to_photoI(int a2d_photo) {
  // V = IR; I = V/R
  //float photocurrent = a2d_photo*(5.0/1024.)*(1000000./R_PHOTO);
  return a2d_photo;
}

// converts real-world setpoint to internal setpoint
float setpoint_to_internal(float setpoint, char var) {
  float setpoint_internal;
  if (var == 'v') {
    setpoint_internal = min( 100.0, (7.90 * setpoint + 1.98) );
  }

  if (var == 'f') {
    setpoint_internal = sqrt( max( 0.0, (setpoint - 6.7) / (0.00159) ) );
  }

  if (var == 'q') {
    //setpoint_internal = max( 1.0, (10.43 * setpoint + 0.023 ));
    setpoint_internal = max( 1.0, (10.43 * setpoint + 0.023 ));
  }
  return setpoint_internal;
}

// returns whether it's time to move to the next state
bool its_time() {
  bool is_it_time = ( (millis() - previousMillis) > timeout_interval );
  return is_it_time;
}

// sets the system setpoints (by various means)
void step_setpoints() {
  #if DEBUG
    Serial.println("stepping setpoints...");
  #endif

    SetpointPID = setpoint::traj_pid[setpoint::index];
    setpoint::volt = setpoint::traj_volt[setpoint::index];
    setpoint::freq = setpoint::traj_freq[setpoint::index];
    setpoint::flow = setpoint::traj_flow[setpoint::index];
}

// actuates the system inputs via the potentiometers
void actuate_inputs() {
  #if DEBUG
    Serial.println("actuating inputs...");
  #endif

  setpoint_internal::volt = setpoint_to_internal(setpoint::volt, 'v');
  setpoint_internal::freq = setpoint_to_internal(setpoint::freq, 'f');
  setpoint_internal::flow = setpoint_to_internal(setpoint::flow, 'q');

  DEV_VOLT.setResistance( POT_VOLT, setpoint_internal::volt );
  DEV_FREQ.setResistance( POT_FREQ, setpoint_internal::freq );
  DEV_FLOW.setResistance( POT_FLOW, setpoint_internal::flow );
}

// move to the next state
void timeout() {
  #if DEBUG
    Serial.println("timeout triggered!");
  #endif

  // reset the counter if it's run the entire way through
  if( setpoint::index >= sizeof(setpoint::traj_volt) / sizeof(setpoint::traj_volt[0]) ) {
    setpoint::index = 0;
  }

  // set the setpoints
  // if open loop, looked up from the provided input trajectories
  // if closed loop PID, calculated by PID controller
  // if closed loop, queried from elsewhere
  step_setpoints();

  // actuate the system inputs via the digital potentiometers
  actuate_inputs();

  // increment the counters
  setpoint::index += 1;
  previousMillis = millis();
}

// the setup routine runs once when you press reset
void setup() {
  analogReference(EXTERNAL);
  setup_serial();
  setup_watchdog();
  setup_rtc();
  //#if PID_ON
  //  setup_pid();
  //#endif
  setup_mlx();
  setup_pots();

  // controlled mode - turn the PID on
  //myPID.SetTunings(Kp,Ki,Kd) // set the tuning parameters
  SetpointPID = setpoint::traj_pid[0];            // controller drives toward this value
  myPID.SetControllerDirection(DIRECT); // DIRECT or REVERSE
  //myPID.SetOutputLimits(PID_OUTPUT_MIN,PID_OUTPUT_MAX);         // PWM output range is 0-100 (off-on)
  myPID.SetOutputLimits(CTRL_OUT_MIN,CTRL_OUT_MAX);         // PWM output range is 0-100 (off-on)
  myPID.SetSampleTime(50);              // in milliseconds
  myPID.SetMode(0);             // AUTOMATIC or nonzero = on

  step_setpoints();
  actuate_inputs();
}

// the loop routine runs over and over again forever
void loop() {
  // reset the watchdog timer
  //if this doesn't occur within WDTO_X, system resets
  wdt_reset();

  if (Serial.available() > 0) {
    #if DEBUG
      Serial.println("bytes available: " + String(Serial.available()));
    #endif
    get_serial();
  }

  // timeout loop for time-triggered adjustments
  if ( timed_advance && its_time() ) {
    timeout();
  }

  // traversed only in controlled mode
  switch( ctrl_measured ) {
    case 'p' :
      //photodiode
      Input = data::photo_i;
      break;
    case 's' :
      //surface temperature
      Input = data::temp_surf;
      break;
    case 't' :
      //tube temperature
      Input = data::temp_tube;
      break;
    }


  if ( myPID.Compute() ) {
    CTRL_SETPOINT = Output;
    // actuate the system inputs via the digital potentiometers
    actuate_inputs();
  }

  // read the time in ms since Arduino startup
  time_ms = millis();

  // get the timestamp from the RTC
  //tstamp = float(get_tstamp());
  print_tstamp();

  Serial.print(Input);
  Serial.print(",");
  Serial.print(SetpointPID);
  Serial.print(",");
  Serial.print(Output);
  Serial.print(",");

  // read the ambient temperature via LM35
  analogRead(PIN_TEMPAMB);
  data::temp_amb = a2d_to_tempDeg_lm35(analogRead(PIN_TEMPAMB));
  delay(1);

  // read the light intensity via photodetector
  analogRead(PIN_PHOTO);
  data::photo_i = a2d_to_photoI(analogRead(PIN_PHOTO));
  delay(1);

  // read the voltages actually applied by the potentiometers
  //setpoint_meas::volt = analogRead(PIN_VOLT_MEAS);
  //setpoint_meas::freq = analogRead(PIN_FREQ_MEAS);
  setpoint_meas::flow = analogRead(PIN_FLOW_MEAS);
  delay(1);

  // read the surface temperatures via Melexis thermocouples
  data::temp_surf = get_tempDeg_mlx(THERMOPILE1_ADDRESS);
  data::temp_tube = get_tempDeg_mlx(THERMOPILE2_ADDRESS);

  // print the data
  dataprint(mydata);

  // delay in-between reads
  delay(LOOP_DELAY);
}
