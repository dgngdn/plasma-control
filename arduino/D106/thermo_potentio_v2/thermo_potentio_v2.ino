// Customized by Brandon Curtis, www.brandoncurtis.com
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <McpDigitalPot.h>

// Then choose any other free pin as the Slave Select (pin 10 if the default but doesn’t have to be)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10

// It’s recommended to measure the rated end-end resistance (terminal A to terminal B)
float rAB_ohms = 10090.00; // 10090.00; 5k Ohm

// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );

// Instantiate the thermopile
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;   // variable to store the value coming from the sensor
int tempValue = 0;     // variable to store the thermopile read temperature
int potSet = 0;        // variable to store the new potentiometer setting

void setup()
{
  Serial.begin(9600);
  // initialize SPI:
  SPI.begin(); 
  // Scale to 100.0 for a percentage, or 1.0 for a fraction
  digitalPot.scale = 100; 
  // initialize thermopile
  mlx.begin();  
}

void timeout()
{
  tempValue = mlx.readObjectTempF();
  potSet = 4*(tempValue-70);
  digitalPot.setResistance(0, potSet);
}

void loop()
{
  timeout();
}

