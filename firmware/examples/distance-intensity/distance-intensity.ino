/* This minimal example shows how to get single-shot range
measurements from the VL6180X.

The range readings are in units of mm. */

#include <Wire.h>
// library: VL6180X proximity sensor
#include <VL6180X.h>
// library: watchdog timer
#include <avr/wdt.h>   //#include "lib/avr/wdt.h" // #include <avr/wdt.h>

// GLOBAL CONSTANTS
const long SERIAL_BAUD = 9600;   // 4800,9600,14400,19200,38400,57600,115200,0.5M,1.0M,2.0M
const int LOOP_DELAY = 1;      // milliseconds

VL6180X sensor;

const int read_num = 512;
//const int read_num = 100;
int read_data[read_num];
float read_average = 0;
int read_index = 0;
int read_total = 0;

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
  Wire.begin();
  
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(50);
}

void loop() 
{
  // reset the watchdog timer
  //if this doesn't occur within WDTO_X, system resets
  wdt_reset();
  
  int sensor_val = sensor.readRangeSingleMillimeters();
  addRead(sensor_val);
  
  Serial.print(millis());
  Serial.print(',');
  Serial.print(sensor_val);
  //Serial.print(read_average-61+3);
  Serial.print(',');
  Serial.print(analogRead(A0));
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  
  Serial.println();
  delay(LOOP_DELAY);
}
