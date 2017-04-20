/**
 * Two Infrared Thermometers MLX906114
 * by Jaime Patarroyo
 * based on 'Is it hot? Arduino + MLX90614 IR Thermometer' by bildr.blog
 * 
 * Returns the temperature in Celcius and Fahrenheit from two MLX90614 
 * Infrared Thermometers, connected to the TWI/I²C pins (on the Wiring v1 
 * board 0 (SCL) and 1 (SDA) and on Wiring S board 8 (SCL) and 9 (SDA)).
 */

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
}

void loop()
{
  celcius1 = temperatureCelcius(device1Address);// Read's data from MLX90614
  celcius2 = temperatureCelcius(device2Address);// with the given address,
                                                // transform's it into
                                                // temperature in Celcius and
                                                // store's it in the celcius1
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
  Serial.print(celcius1);
  Serial.print(",");
  Serial.print(celcius2);
  Serial.println();

  delay(100);                         // Wait a second before printing again.
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
