#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

byte ATuneModeRemember=2;
double input=80, output=50, setpoint=40;
double kp=14.89,ki=2.23,kd=0;

double kpmodel=1.5, taup=100, theta[50];
double outputStart=5;
double aTuneStep=5, aTuneNoise=1, aTuneStartValue=75;
unsigned int aTuneLookBack=20;

boolean tuning = true;
unsigned long  modelTime, serialTime;

PID myPID(&input, &output, &setpoint,kp,ki,kd, DIRECT);
PID_ATune aTune(&input, &output);

//set to false to connect to the real world
boolean useSimulation = false;

/**
 * /////////////////////
 * /// SETUP MCP4261 ///
 * /////////////////////
 * Configured the Microchip digital potentiometers
 * Uses the Spi library by Cam Thompson, originally part of FPU library (micromegacorp.com)
 * from http://arduino.cc/playground/Code/Fpu or http://www.arduino.cc/playground/Code/Spi
 * Including SPI.h vv below initializea the MOSI, MISO, and SPI_CLK pins as per ATMEGA 328P
 */
#include <SPI.h>

// McpDigitalPot library available from https://github.com/teabot/McpDigitalPot
// Adapted from https://github.com/dreamcat4/McpDigitalPot
#include <McpDigitalPot.h>

// Wire up the SPI Interface common lines:
// #define SPI_CLOCK            13 //arduino <-> SPI Slave Clock Input   -> SCK (Pin 02 on McpDigitalPot DIP)
// #define SPI_MOSI             11 //arduino <-> SPI Master Out Slave In -> SDI (Pin 03 on McpDigitalPot DIP)
// #define SPI_MISO             12 //arduino <-> SPI Master In Slave Out -> SDO (Pin 13 on McpDigitalPot DIP)

// Then choose any other free pin as the Slave Select (default pin 10)
#define MCP_DIGITAL_POT_SLAVE_SELECT_PIN 10 //arduino   <->   Chip Select      -> CS  (Pin 01 on McpDigitalPot DIP)

// CALIBRATE ME: end-to-end potentiometer resistance, terminal A-terminal B
float rAB_ohms = 10000.00; // 10k Ohm 

// CALIBRATE ME: rW = wiper resistance (75-160 ohms)
//McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms, rW_ohms );
// Instantiate McpDigitalPot object, with default rW (=117.5 ohm, its typical resistance)
McpDigitalPot digitalPot = McpDigitalPot( MCP_DIGITAL_POT_SLAVE_SELECT_PIN, rAB_ohms );

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

#include <i2cmaster.h>
#include <Wire.h>

int device1Address = 0x5A<<1;   // 0x5A is the assigned address for I²C 
                                // communication for sensor 1.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.
int device2Address = 0x5B<<1;   // 0x5B is the assigned address for I²C 
                                // communication for sensor 2.
                                // Shift the address 1 bit right, the 
                                // I²Cmaster library only needs the 7 most 
                                // significant bits for the address.

float celcius1 = 0;             // Variable for degC, sensor 1
float celcius2 = 0;             // Variable for degC, sensor 2

/**
 * ////////////////////////
 * /// GET TEMPERATURES ///
 * //////////////////////
 */

float temperatureCelcius(int address) {
#if DEBUG
  Serial.println("getting temperatures...");
#endif
  int dev = address;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  // Write
#if DEBUG
  Serial.println("writing to i2c...");
#endif
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // Read
#if DEBUG
  Serial.println("reading from i2c...");
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

void setup()
{
  if(useSimulation)
  {
    for(byte i=0;i<50;i++)
    {
      theta[i]=outputStart;
    }
    modelTime = 0;
  }
  //Setup the pid 
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(1,100);         // PWM output range is 0-100 (off-on)

  if(tuning)
  {
    tuning=false;
    changeAutoTune();
    tuning=true;
  }
  
  serialTime = 0;
  Serial.begin(57600);
  
  //setup potentiometer
  // initialize SPI:
  SPI.begin();
  
  // (optional)
  // Set the scaling method, 100.0 for percentages, 1.0 for fractions
  digitalPot.scale = 100.0;
  
  // SETUP MELEXIS
  i2c_init();                               // Initialise the i2c bus.
  PORTC = (1 << PORTC4) | (1 << PORTC5);    // Enable pullups.

}

void loop()
{

  unsigned long now = millis();

  if(!useSimulation)
  { //pull the input in from the real world
    //input = analogRead(0);
    input = temperatureCelcius(device2Address);
  }
  
  if(tuning)
  {
    byte val = (aTune.Runtime());
    if (val!=0)
    {
      tuning = false;
    }
    if(!tuning)
    { //we're done, set the tuning parameters
      kp = aTune.GetKp();
      ki = aTune.GetKi();
      kd = aTune.GetKd();
      myPID.SetTunings(kp,ki,kd);
      AutoTuneHelper(false);
    }
  }
  else myPID.Compute();
  
  if(useSimulation)
  {
    theta[30]=output;
    if(now>=modelTime)
    {
      modelTime +=100; 
      DoModel();
    }
  }
  else
  {
     //analogWrite(0,output); 
     digitalPot.setResistance(1, output);
  }
  
  //send-receive with processing if it's time
  if(millis()>serialTime)
  {
    SerialReceive();
    SerialSend();
    serialTime+=500;
  }
}

void changeAutoTune()
{
 if(!tuning)
  {
    //Set the output to the desired starting frequency.
    output=aTuneStartValue;
    aTune.SetNoiseBand(aTuneNoise);
    aTune.SetOutputStep(aTuneStep);
    aTune.SetLookbackSec((int)aTuneLookBack);
    AutoTuneHelper(true);
    tuning = true;
  }
  else
  { //cancel autotune
    aTune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}

void AutoTuneHelper(boolean start)
{
  if(start)
    ATuneModeRemember = myPID.GetMode();
  else
    myPID.SetMode(ATuneModeRemember);
}


void SerialSend()
{
  Serial.print("setpoint: ");Serial.print(setpoint); Serial.print(" ");
  Serial.print("input: ");Serial.print(input); Serial.print(" ");
  Serial.print("output: ");Serial.print(output); Serial.print(" ");
  if(tuning){
    Serial.println("tuning mode");
  } else {
    Serial.print("kp: ");Serial.print(myPID.GetKp());Serial.print(" ");
    Serial.print("ki: ");Serial.print(myPID.GetKi());Serial.print(" ");
    Serial.print("kd: ");Serial.print(myPID.GetKd());Serial.println();
  }
}

void SerialReceive()
{
  if(Serial.available())
  {
   char b = Serial.read(); 
   Serial.flush(); 
   if((b=='1' && !tuning) || (b!='1' && tuning))changeAutoTune();
  }
}

void DoModel()
{
  //cycle the dead time
  for(byte i=0;i<49;i++)
  {
    theta[i] = theta[i+1];
  }
  //compute the input
  input = (kpmodel / taup) *(theta[0]-outputStart) + input*(1-1/taup) + ((float)random(-10,10))/100;

}
