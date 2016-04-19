/**
 * control_v005, Brandon Curtis
 * this code is public domain
 * status: WORKS AS DESCRIBED
 * hardware requirements:
 * 2x IR thermal sensors (Melexis LX90614), I2C
 * 1x digital potentiometer (Microchip MCP4261), SPI
 * Thermal sensors must have unique addresses on the i2c bus
 * (this is accomplished with another program)
 * Temperature (degC) is read from both sensors.
 * DEFINE statements switch on and off a PID controller
 * 
 * if PID_CONTROL is true
 * Digital potentiometer is PID controlled
 * PID setpoint: 60C
 * PID inputs: _sum_ of the thermal readings
 * PID output: potentiometer wiper position
 * endif
 * 
 * if PID_CONTROL is false
 * Digital potentiometer is stepped through its range
 * 20% steps, 10 seconds per step
 * endif
 * 
 * potentiometer wiper voltage read at pin A3
 * print: thermal1_degC,thermal2_degC,A3
*/

#define PID_CONTROL false

#if PID_CONTROL
// controlled mode
// SETUP PID
// process-integral-derivative controller for Arduino
// http://playground.arduino.cc/Code/PIDLibary
// https://github.com/br3ttb/Arduino-PID-Library/
// http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
#include <PID_v1.h>
// Define Variables we'll be connecting the controller to
double Setpoint, Input, Output;
double Kp = 0.1; // 0.1, process value control 
double Ki = 1.0; // 1.0, integral control
double Kd = 0.0; // 0.0, derivative control
// Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); // DIRECT or REVERSE

#else
// uncontrolled mode
// Cycle the wipers around at 20% increments, changing every 10 seconds
long timeoutInterval = 10000;
long previousMillis = 0;
float counter = 0.0;
#endif

// SETUP MCP4261 
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

// Then choose any other free pin as the Slave Select (default pin 10)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select      -> CS  (Pin 01 on McpDigitalPot DIP)

// CALIBRATE ME: end-to-end potentiometer resistance, terminal A-terminal B
float rAB_ohms = 10090.00; // 10k Ohm 

// CALIBRATE ME: rW = wiper resistance (75-160 ohms)
//McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );
// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );

/**
 * Two Infrared Thermometers MLX906114
 * by Jaime Patarroyo
 * based on 'Is it hot? Arduino + MLX90614 IR Thermometer' by bildr.blog
 * 
 * Returns the temperature in Celcius and Fahrenheit from two MLX90614 
 * Infrared Thermometers, connected to the TWI/I²C pins (on the Wiring v1 
 * board 0 (SCL) and 1 (SDA) and on Wiring S board 8 (SCL) and 9 (SDA)).
 */
 
// SETUP MLX90614
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

float celcius1 = 0;             // Variable for degC, sensor 1
float celcius2 = 0;             // Variable for degC, sensor 2

void setup()
{
  Serial.begin(9600);           // Start serial communication at 9600bps.
  
  i2c_init();                               // Initialise the i2c bus.
  PORTC = (1 << PORTC4) | (1 << PORTC5);    // Enable pullups.
  
  // initialize SPI:
  SPI.begin();
  
  // (optional)
  // Set the scaling method, 100.0 for percentages, 1.0 for fractions
  digitalPot.scale = 100.0;

#if PID_CONTROL
  // controlled mode
  // turn the PID on
  //myPID.SetTunings(Kp,Ki,Kd) // set the tuning parameters
  Setpoint = 60;                        // controller drives toward this value
  myPID.SetControllerDirection(DIRECT); // DIRECT or REVERSE
  myPID.SetOutputLimits(0,100);         // PWM output range is 0-100 (off-on)
  myPID.SetSampleTime(50);              // in milliseconds
  myPID.SetMode(AUTOMATIC);             // AUTOMATIC = on
#endif
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
  
  // Returns temperature in Celcius.
  return celcius;
}

#if ! PID_CONTROL
void timeout()
{
  if(counter > 100.0)
    counter = 0.0;

  // set the wiper position
  // pot0 uses pins 8,9,10; pot1 uses pins 5,6,7
  // digitalPot.setResistance(pot#, 0-100);
  digitalPot.setResistance(1, 100-counter); // start high and go low
  //digitalPot.setResistance(1, counter); // start low and go high
  //digitalPot.setResistance(1, 100); // fixed value
  
  counter += 10.0; // change in increments of 10
}
#endif

void loop()
{
  celcius1 = temperatureCelcius(device1Address);// Reads data from MLX90614
  celcius2 = temperatureCelcius(device2Address);// with the given address,
                                                // transforms it into
                                                // temperature in Celcius and
                                                // stores it in celcius1/celcius2
#if PID_CONTROL
  // controlled mode
  Input = celcius1 + celcius2;
  
  if ( myPID.Compute() ) {
    // digitalPot.setResistance(pot#, 0-100);
    digitalPot.setResistance(1, Output); // this is what the PID is controlling
  }
#else
  // uncontrolled mode
  // timeout loop for adjusting the potentiometer setting
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }
#endif

  // Read the input from the potentiometer wiper at A3
  int sensorValue = analogRead(A3);
  
  Serial.print(celcius1);
  Serial.print(",");
  Serial.print(celcius2);
  Serial.print(",");
  Serial.print(sensorValue);
  Serial.println();

  delay(100);                         // Wait before lopping.
}
