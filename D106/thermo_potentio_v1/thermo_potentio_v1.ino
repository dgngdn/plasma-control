// Customized by Brandon Curtis, www.brandoncurtis.com
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <McpDigitalPot.h>

// Wire up the SPI Interface common lines:
// #define SPI_CLOCK            13 //arduino   <->   SPI Slave Clock Input     -> SCK (Pin 02 on McpDigitalPot DIP)
// #define SPI_MOSI             11 //arduino   <->   SPI Master Out Slave In   -> SDI (Pin 03 on McpDigitalPot DIP)
// #define SPI_MISO             12 //arduino   <->   SPI Master In Slave Out   -> SDO (Pin 13 on McpDigitalPot DIP)

// Then choose any other free pin as the Slave Select (pin 10 if the default but doesnt have to be)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select               -> CS  (Pin 01 on McpDigitalPot DIP)

// Its recommended to measure the rated end-end resistance (terminal A to terminal B)
// Because this can vary by a large margin, up to -+ 20%. And temperature variations.
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

// Change the potentiometer value every tenth of a second
long timeoutInterval = 100;
long previousMillis = 0;

void timeout()
{
  tempValue = mlx.readObjectTempF();
  potSet = 3*(tempValue-60);
  digitalPot.setResistance(0, potSet);
}

void loop()
{
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
    sensorValue = analogRead(sensorPin);   
    Serial.println(tempValue);
    Serial.println(potSet);
    Serial.println(sensorValue);
    Serial.println();
  }
}

