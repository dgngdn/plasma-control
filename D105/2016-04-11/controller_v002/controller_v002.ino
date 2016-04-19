
/**
 * control_v002, Brandon Curtis
 * this code is public domain
 * status: WORKS AS DESCRIBED
 * hardware requirements:
 * 2x IR thermal sensors (Melexis LX90614), I2C
 * 1x digital potentiometer (Microchip MCP4261), SPI
 * Thermal sensors must have unique addresses on the i2c bus
 * (this is accomplished with another program)
 * Temperature (degC) is read from both sensors.
 * Digital potentiometer is stepped through its range
 * 20% steps, 2 seconds per step
 * potentiometer wiper voltage read at pin A3
 * print: thermal1_degC,thermal2_degC,A3
*/

/**
 * Two Infrared Thermometers MLX906114
 * by Jaime Patarroyo
 * based on 'Is it hot? Arduino + MLX90614 IR Thermometer' by bildr.blog
 * 
 * Returns the temperature in Celcius and Fahrenheit from two MLX90614 
 * Infrared Thermometers, connected to the TWI/I²C pins (on the Wiring v1 
 * board 0 (SCL) and 1 (SDA) and on Wiring S board 8 (SCL) and 9 (SDA)).
 */
 
// The Spi library by Cam Thompson. It was originally part of FPU library (micromegacorp.com)
// Available from http://arduino.cc/playground/Code/Fpu or http://www.arduino.cc/playground/Code/Spi
// Including SPI.h vv below initializea the MOSI, MISO, and SPI_CLK pins as per ATMEGA 328P
#include <SPI.h>


// McpDigitalPot library available from https://github.com/teabot/McpDigitalPot
// Adapted from https://github.com/dreamcat4/McpDigitalPot
#include <McpDigitalPot.h>

// Wire up the SPI Interface common lines:
// #define SPI_CLOCK            13 //arduino   <->   SPI Slave Clock Input     -> SCK (Pin 02 on McpDigitalPot DIP)
// #define SPI_MOSI             11 //arduino   <->   SPI Master Out Slave In   -> SDI (Pin 03 on McpDigitalPot DIP)
// #define SPI_MISO             12 //arduino   <->   SPI Master In Slave Out   -> SDO (Pin 13 on McpDigitalPot DIP)

// Then choose any other free pin as the Slave Select (pin 10 if the default but doesnt have to be)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select               -> CS  (Pin 01 on McpDigitalPot DIP)

// Its recommended to measure the rated end-end resistance (terminal A to terminal B)
// Because this can vary by a large margin, up to -+ 20%. And temperature variations.
float rAB_ohms = 10090.00; // 10k Ohm

// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );

// rW - Wiper resistance. This is a small additional constant. To measure it
// use the example, setup(). Required for accurate calculations (to nearest ohm)
// Datasheet Page 5, gives typical values MIN=75ohm, MAX @5v=160ohm,@2.7v=300ohm
// Usually rW should be somewhere between 100 and 150 ohms.
// Instantiate McpDigitalPot object, after measuring the real rW wiper resistance
// McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );

// Cycle the wipers around at 20% increments, changing every 2 seconds
long timeoutInterval = 2000;
long previousMillis = 0;
float counter = 0.0;


// Configuration stuff for the MLX IR thermal sensors
#include <i2cmaster.h>

int device1Address = 0x5A<<1;   // 0x50 is the assigned address for I²C 
                                // communication for sensor 1.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.
int device2Address = 0x5B<<1;   // 0x55 is the assigned address for I²C 
                                // communication for sensor 2.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.

float celcius1 = 0;             // Variable to hold temperature in Celcius
                                // for sensor 1.
//float fahrenheit1 = 0;          // Variable to hold temperature in Fahrenheit
                                // for sensor 1.
float celcius2 = 0;             // Variable to hold temperature in Celcius
                                // for sensor 2.
//float fahrenheit2 = 0;          // Variable to hold temperature in Fahrenheit
                                // for sensor 2.

void setup()
{
  Serial.begin(9600);           // Start serial communication at 9600bps.
  
  i2c_init();                               // Initialise the i2c bus.
  PORTC = (1 << PORTC4) | (1 << PORTC5);    // Enable pullups.
  
  // initialize SPI:
  SPI.begin();
  
  // (optional)
  // measure the the wiper resistance, called rW
  //digitalPot.setPosition(0, 0); // rAW = rW_ohms
  //digitalPot.setPosition(1, 0); // rAW = rW_ohms
  
  // (optional)
  // Scale to 100.0 for a percentage, or 1.0 for a fraction
  // Eg if scale=100, then setResistance(0, 100) = max rAW resistance
  // Eg    scale=1.0, then setResistance(0, 1.0) = max rAW resistance
  // digitalPot.scale = 1.0;
  digitalPot.scale = 100.0; // For the timeout example, below
}

void loop()
{
  celcius1 = temperatureCelcius(device1Address);// Read's data from MLX90614
  celcius2 = temperatureCelcius(device2Address);// with the given address,
                                                // transform's it into
                                                // temperature in Celcius and
                                                // stores it in the celcius1
                                                // or celcius2 variables.
 
  //fahrenheit1 = (celcius1*1.8) + 32;     // Converts celcius into Fahrenheit 
  //fahrenheit2 = (celcius2*1.8) + 32;     // and stores in Fahrenheit1 or 
                                         // Fahrenheit2 variables.

  //Serial.print("Sensor 1: Celcius: ");   // Prints all readings in the Serial 
  //Serial.print(celcius1);                // port.
  //Serial.print("  Fahrenheit: ");
  //Serial.println(fahrenheit1);
  //Serial.print("Sensor 2: Celcius: ");
  //Serial.print(celcius2);
  //Serial.print("  Fahrenheit: ");
  //Serial.println(fahrenheit2);
  
  // Read the input from the potentiometer wiper at A3
  int sensorValue = analogRead(A3);
  
  Serial.print(celcius1);
  Serial.print(",");
  Serial.print(celcius2);
  Serial.print(",");
  Serial.print(sensorValue);
  Serial.println();

  delay(100);                         // Wait a second before printing again.
  
  // timeout loop for adjusting the potentiometer setting
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }
  // Loop.
}

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
  
  // Returns temperature un Celcius.
  return celcius;
}

void timeout()
{
  if(counter > 100.0)
    counter = 0.0;

  // These resistances are just percentages of 100
  // pot0 uses pins 8,9,10; pot1 uses pins 5,6,7
  // digitalPot.setResistance(pot#, 0-100);
  digitalPot.setResistance(1, counter);

  counter += 20.0;
}


