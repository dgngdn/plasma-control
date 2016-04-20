#include <PID_v1.h>

#include <SPI.h>
#include <McpDigitalPot.h>

#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10
float rAB_ohms = 10090.00;
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );

int sensorPin = A0;
int sensorValue = 0;



void setup()
{
  Serial.begin(9600);

  // initialize SPI:
  SPI.begin(); 
  
  digitalPot.scale = 100; 
}
  
  




