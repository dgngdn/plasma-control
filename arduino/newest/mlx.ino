char junk;
#include <avr/wdt.h>   //#include "lib/avr/wdt.h" // #include <avr/wdt.h>
#include <Wire.h>      // #include "lib/Wire/src/Wire.h" // #include <Wire.h>
#include <McpDigitalPot.h> // "lib/mcpdigitalpot/McpDigitalPot.h" //#include <McpDigitalPot.h>

// GLOBAL CONSTANTS
const int THERMOPILE1_ADDRESS = 0x5A<<1;
const int THERMOPILE2_ADDRESS = 0x5B<<1;
const long SERIAL_BAUD = 9600; // 4800,9600,14400,19200,38400,57600,115200,0.5M,1.0M,2.0M
const int LOOP_DELAY = 10;      // milliseconds

// GLOBAL VARIABLES
float time_ms = 0;

// holds measured values
namespace data {
  float temp_amb_tube = 0;
  float temp_tube = 0;
  float temp_amb_surf = 0;
  float temp_surf = 0;
}

// set up Melexis thermopiles
void setup_mlx() {
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

  // Initialise the i2c bus
  i2c_init();
  // Enable pullup resistors
  PORTC = (1 << PORTC4) | (1 << PORTC5);
}

// retrieves temperatures from Melexis i2c thermopile
float get_tempDeg_mlx(int address) {
  #if DEBUG
    Serial.println("getting temperature from thermopile...");
  #endif
  int dev = address;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  // Write to the device to request the data
  #if DEBUG
    Serial.println("writing request to i2c...");
  #endif
  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // Read the data from the device
  #if DEBUG
    Serial.println("reading data from i2c...");
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

// set up watchdog timer
void setup_watchdog() {
  // enable the watchdog timer
  // if Arduino stops responding, auto-resets
  // Watchdog Timeouts: WDTO_{1,2,4,8}s
  // WDTO_{15,30,60,120,250,500}MS
  // http://www.megunolink.com/articles/how-to-detect-lockups-using-the-arduino-watchdog/
  wdt_enable(WDTO_1S);
}

// set up serial connection
void setup_serial() {
  // initialize serial communication at specified baudrate
  Serial.begin(SERIAL_BAUD);
}

void setup() {
  //analogReference(EXTERNAL);
  setup_serial();
  setup_watchdog();
  setup_mlx();
}

// the loop routine runs over and over again forever
void loop() {
  // reset the watchdog timer
  //if this doesn't occur within WDTO_X, system resets
  wdt_reset();

  // read the time in ms since Arduino startup
  time_ms = millis();

  // read the surface temperatures via Melexis thermocouples
  data::temp_surf = get_tempDeg_mlx(THERMOPILE1_ADDRESS);
  data::temp_tube = get_tempDeg_mlx(THERMOPILE2_ADDRESS);

  Serial.print(time_ms);
  Serial.print(',');
  Serial.print(temp_surf);
  Serial.print(',');
  Serial.print(temp_tube);
  Serial.println();

  // delay in-between reads
  delay(LOOP_DELAY);
}
