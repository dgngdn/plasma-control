/* http://arduino.cc/en/Hacking/LibraryTutorial */
/* http://ww1.microchip.com/downloads/en/DeviceDoc/22250A.pdf */
// Brandon: TESTING 2017-04-19

/* This minimal example shows how to get single-shot range
measurements from the VL6180X.
The range readings are in units of mm. */

// DEFINES
#define DEBUG false

// IMPORTS
#include <Wire.h>
#include <VL6180X.h>  // library: VL6180X proximity sensor
#include <avr/wdt.h>  // library: watchdog timer
#include <MCP4922.h>  // library: DAC
#include <SPI.h>
#include <Wire.h>

// GLOBAL VARS
VL6180X proxsensor;
String inputString = "";         // a string to hold incoming data

const int read_num = 100;
int read_data[read_num];
float read_average = 0;
int read_index = 0;
int read_total = 0;

// GLOBAL CONSTANTS
const double SERIAL_BAUD = 38400;
const int PROXIMITY_TIMEOUT = 25;
const int LOOPDELAY = 1; // milliseconds
const int PIN_DAC_A = 2; 
const int PIN_DAC_B = 3;

//MCP4922 DAC(51,52,53,5);    // (MOSI,SCK,CS,LDAC) define Connections for MEGA_board, 
MCP4922 DAC(11,13,10,5);    // (MOSI,SCK,CS,LDAC) define Connections for UNO_board, 

namespace data {
  int DAC_A;
  int DAC_B;
  int DIST;
}

namespace setpoint {
  float voltage;
  float frequency;
  float flowrate;
}

void setup_serial() {
  // set up serial connection
  // initialize serial communication at specified baudrate
  Serial.begin(SERIAL_BAUD);
  #if DEBUG
    Serial.println();
    Serial.println();
    Serial.println("Serial connection established!");
  #endif
}

void setup_watchdog() {
  // enable the watchdog timer
  // if Arduino stops responding, auto-resets
  // Watchdog Timeouts: WDTO_{1,2,4,8}s
  // WDTO_{15,30,60,120,250,500}MS
  // http://www.megunolink.com/articles/how-to-detect-lockups-using-the-arduino-watchdog/
  #if DEBUG
    Serial.println("enabling watchdog timer...");
  #endif
  wdt_enable(WDTO_250MS);
  #if DEBUG
    Serial.println("watchdog timer enabled!");
  #endif
}

void get_serial() {
  // gets manual input from the serial port
  #if DEBUG
    Serial.println("getting serial input...");
  #endif

  boolean stringComplete = false;  // whether the string is complete

  while (Serial.available() > 0) {

    // get the new byte:
    char inChar = (char)Serial.read();
    
    #if DEBUG
      Serial.println(String(inChar));
    #endif
    
    // add it to the inputString:
    inputString += inChar;
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      break;
    }
  }

  if ( stringComplete == true && inputString.length() > 0 ) {
    // if you have a non-blank input string and it's complete...
    #if DEBUG
      Serial.print("got serial input: ");
      Serial.print(inputString);
      Serial.println();
    #endif
    // process it with your manual_input function!
    manual_input(inputString);
    inputString = "";
  }
}

void manual_input(String input) {
  // process the manual request recieved via serial
  #if DEBUG
    Serial.println("processing serial input!");
  #endif

  switch( input.charAt(0) ) {
    case 'v' :
      // you sent v,###
      setpoint::voltage = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("voltage set!");
      #endif
      actuate_inputs();
      break;
  }
}

void actuate_inputs() {
  // actuates the system inputs via the DACs
  #if DEBUG
    Serial.println("actuating inputs...");
  #endif
  DAC.Set(map(setpoint::voltage,0,10,4095,0), 0);
}

void addRead(int value)
{
  read_total = read_total - read_data[read_index];
  read_data[read_index] = value;
  read_total = read_total + read_data[read_index];
  read_average = ((float)read_total) / read_num;
  read_index++;
  if (read_index >= read_num)
  {
    read_index = 0;
  }
}


void setup()
{
  setup_serial();
  setup_watchdog();
  SPI.begin();
  Wire.begin();

  // set up VL6180X sensor
  proxsensor.init();
  proxsensor.configureDefault();
  proxsensor.setTimeout(PROXIMITY_TIMEOUT);

  actuate_inputs();
}

void loop()
{
  // reset the watchdog timer
  // if this doesn't occur within WDTO_X, system resets
  wdt_reset();
  
  if (Serial.available() > 0) {
    #if DEBUG
      Serial.println("bytes available: " + String(Serial.available()));
    #endif
    get_serial();
  }

  // read VL6180X distance and add to averaging array
  data::DIST = proxsensor.readRangeSingleMillimeters();
  //addRead(data::DIST);


  // read DAC values and print
  data::DAC_A = analogRead(PIN_DAC_A); // analogRead(A0); //analogRead(PIN_DAC_A);
  data::DAC_B = analogRead(PIN_DAC_B); // analogRead(A1); //analogRead(PIN_DAC_B);
  Serial.print(millis());
  Serial.print(",\t");
  Serial.print(data::DAC_A);
  Serial.print(",\t");
  Serial.print(data::DAC_B);
  Serial.print(",\t");
  Serial.print(data::DIST); // print distance
  Serial.print(",\t");
  Serial.print(analogRead(A0)); // print optical intensity
  if (proxsensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  delay(LOOPDELAY);
}

