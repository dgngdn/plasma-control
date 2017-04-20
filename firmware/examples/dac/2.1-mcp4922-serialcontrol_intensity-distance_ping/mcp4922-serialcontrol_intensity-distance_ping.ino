/* http://arduino.cc/en/Hacking/LibraryTutorial */
/* http://ww1.microchip.com/downloads/en/DeviceDoc/22250A.pdf */
// Brandon: TESTING 2017-04-19

/* This minimal example shows how to get single-shot range
measurements from the VL6180X.
The range readings are in units of mm. */

// DEFINES
#define DEBUG false

// IMPORTS
#include <MCP4922.h>
#include <SPI.h>
#include <Wire.h>
#include <VL6180X.h>


VL6180X sensor;

const int read_num = 512;
int read_data[read_num];
float read_average = 0;
int read_index = 0;
int read_total = 0;

const int PIN_DAC_A = 2;
const int PIN_DAC_B = 3;
const int DAC_STEPS = 4096;
int step_interval = 10;

MCP4922 DAC(51,52,53,5);    // (MOSI,SCK,CS,LDAC) define Connections for MEGA_board, 
//MCP4922 DAC(11,13,10,5);    // (MOSI,SCK,CS,LDAC) define Connections for UNO_board, 

namespace data {
  int DAC_A;
  int DAC_B;
  int DAC_SUM;
}

namespace setpoint {
  float voltage;
  float frequency;
  float flowrate;
}

void get_serial() {
  // gets manual input from the serial port
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
  }

  //return inputString.toInt();
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
      
    case 'p' :
      // you sent p
      print_data();
      #if DEBUG
        Serial.println("pinging for data");
      #endif
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

void print_data()
{
  Serial.print(data::DAC_A);
  Serial.print(",");
  Serial.print(data::DAC_B);
  Serial.print(",");
  Serial.print(read_average-60); // print distance
  Serial.print(',');
  Serial.print(analogRead(A0)); // print optical intensity
  Serial.println();
}




void setup()
{
  Serial.begin(9600);
  SPI.begin();
  Wire.begin();

  // set up VL6180X sensor
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(500);
}

void loop()
{
  if (Serial.available() > 0) {
    #if DEBUG
      Serial.println("bytes available: " + String(Serial.available()));
    #endif
    get_serial();
  }

  // read VL6180X distance and add to averaging array
  int sensor_val = sensor.readRangeSingleMillimeters();
  addRead(sensor_val);


  // read DAC values and print
  data::DAC_A = analogRead(PIN_DAC_A); // analogRead(A0); //analogRead(PIN_DAC_A);
  data::DAC_B = analogRead(PIN_DAC_B); // analogRead(A1); //analogRead(PIN_DAC_B);
  data::DAC_SUM = data::DAC_A + data::DAC_B;

  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  delay(50);
}

