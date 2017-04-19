/* This minimal example shows how to get single-shot range
measurements from the VL6180X.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL6180X.h>

VL6180X sensor;

const int read_num = 512;
int read_data[read_num];
float read_average = 0;
int read_index = 0;
int read_total = 0;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(500);
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

void loop() 
{ 
  int sensor_val = sensor.readRangeSingleMillimeters();
  addRead(sensor_val);
  
  Serial.print(millis());
  Serial.print(',');
  //Serial.print(sensor_val);
  Serial.print(read_average-60);
  Serial.print(',');
  Serial.print(analogRead(A0));
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  
  Serial.println();
  //delay(100);
}
