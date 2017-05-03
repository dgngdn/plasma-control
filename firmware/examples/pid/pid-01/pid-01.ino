char junk;
/*  
 *  ## NOTEBOOK
 *  2017-05-03 created -- Brandon Curtis
 *  2017-05-03 tested, no plasma
 *  <not yet tested with plasma!>
 *  
 *  ## FEATURES
 *  
 *  
 *  
 *  
 *  
 *  
 *  
 *  
 *  
*/

// DEFINES
#define DEBUG false

// IMPORTS
#include <Wire.h>
#include <VL6180X.h>  // library: VL6180X proximity sensor
#include <avr/wdt.h>  // library: Atmel built-in watchdog timer
#include <MCP4922.h>  // library: MCP4922 DAC
#include <Adafruit_MLX90614.h>  // library: MLX90614 thermopile
#include <SPI.h>
#include <Wire.h>
#include <PID_v1.h>    //"lib/PID/PID_v1.h" // #include <PID_v1.h>

// NAMESPACES
namespace data {
  unsigned long ts;
  double dac_a;
  double dac_b;
  double dist;
  double photo_a;
  double photo_b;
  double tamb;
}

namespace setpoint {
  double voltage = 0;
  double frequency = 10;
  double flowrate = 0;
  double duty = 0;
}

namespace pid {
  double Setpoint = 50;
  double* Input = &data::photo_a;
  double Output;
  double Kp = 0.00;  // 0.0 // 25.0 // 14.89 // process value control
  double Ki = 5.00;   // 0.5 // 3.0  // 2.23  // integral control
  double Kd = 0.00;  // 0.0 // 0.0  // 0.0   // derivative control
  double* actuated = &setpoint::duty;
}

// GLOBAL VARIABLES
VL6180X proxsensor;
//MCP4922 DAC(51,52,53,5);    // (MOSI,SCK,CS,LDAC) define Connections for MEGA_board
MCP4922 DAC_FXN(11,13,10,5);  // (MOSI,SCK,CS,LDAC) define Connections for UNO_board
MCP4922 DAC_MFC(11,13,9,5);   // (MOSI,SCK,CS,LDAC) define Connections for UNO_board
Adafruit_MLX90614 mlx_5deg = Adafruit_MLX90614(0x5A);  // default address of MLX90614 thermopile
//PID myPID(&pid::Input, &pid::Output, &pid::Setpoint, pid::Kp, pid::Ki, pid::Kd, DIRECT); // DIRECT or REVERSE
PID myPID(pid::Input, &pid::Output, &pid::Setpoint, pid::Kp, pid::Ki, pid::Kd, DIRECT); // DIRECT or REVERSE
String inputString = "";         // a string to hold incoming Serial communications

// GLOBAL CONSTANTS
const unsigned long SERIAL_BAUD = 115200; // 4800,9600,14400,19200,38400,57600,115200,0.5M,1.0M,2.0M
const int PROXIMITY_TIMEOUT = 25; // milliseconds until proximity sensor times out
const int LOOPDELAY = 1; // milliseconds
const int DACSTEPS = 4096;

// HARDWARE PINOUT
const int PIN_PHOTO_A = 0;
const int PIN_PHOTO_B = 1;
const int PIN_DAC_A = 2; 
const int PIN_DAC_B = 3;
const int PIN_PWM = 3;


// CRC-8-MAXIM LOOKUP TABLE
static const uint8_t PROGMEM dscrc_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

uint8_t crc8(const uint8_t *addr, uint8_t len)
// generate CRC8 values; recipient can verify data integrity
{
  uint8_t crc = 0;
  while (len--) {
    crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
  }
  return crc;
}






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

void get_serial() {
  // gets manual input from the serial port
  #if DEBUG
    Serial.println("getting serial input...");
  #endif

  boolean stringComplete = false;  // whether the string is complete

  while (Serial.available() > 0) {

    // get the new byte:
    char inChar = (char)Serial.read();
    
    #if DEBUG
      Serial.println(String(inChar));
    #endif
    
    // add it to the inputString:
    inputString += inChar;
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      break;
    }
  }

  if ( stringComplete == true && inputString.length() > 0 ) {
    // if you have a non-blank input string and it's complete...
    #if DEBUG
      Serial.print("got serial input: ");
      Serial.print(inputString);
      Serial.println();
    #endif
    // process it with your manual_input function!
    manual_input(inputString);
    inputString = "";
  }
}

void manual_input(String input) {
  // process the manual request recieved via serial
  #if DEBUG
    Serial.println("processing serial input!");
  #endif

  switch( input.charAt(0) ) {
    case 'v' :
      // you sent v,###
      setpoint::voltage = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("voltage set!");
      #endif
      actuate_inputs();
      break;
  
    case 'f' :
      // you sent f,###
      setpoint::frequency = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("frequency set!");
      #endif
      actuate_inputs();
      break;
      
    case 'q' :
      // you sent q,###
      setpoint::flowrate = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("flowrate set!");
      #endif
      actuate_inputs();
      break;
      
    case 'd' :
      // you sent d,###
      setpoint::duty = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("duty cycle set!");
      #endif
      actuate_inputs();
      break;

    case 's' :
      // you sent s,###
      pid::Setpoint = input.substring(2).toFloat();
      #if DEBUG
        Serial.println("PID Setpoint set!");
      #endif
      actuate_inputs();
      break;

    case 'p' :
      // you sent p,###
      // PID controller settings
      
      if (input.charAt(2) == 'c') {
        // you sent p,c
        // FUNCTION: toggle controller state
        if (myPID.GetMode() != 0) { // if PID is currently on....
          myPID.SetMode(0);         // turn PID off
          #if DEBUG
          Serial.println("PID control turned off!");
          #endif
        }
        else {                     // but if PID controller currently off...
          pid::Output = *pid::actuated;  // reset PID output to current setpoint
          myPID.SetMode(1);        // turn PID on
          pid::Output = *pid::actuated;  // reset PID output to current setpoint
          #if DEBUG
          Serial.println("PID control turned on!");
          #endif
        }
      }
        
      if (input.charAt(2) == 'i') {
        // you sent p,i,
        // FUNCTION: set PID controller Input
        if (input.charAt(4) == 'i') {
          // you sent p,i,i,
          if (input.charAt(6) == '1') {
            // you sent p,i,i,1
            pid::Input = &data::photo_a;
            #if DEBUG
            Serial.println("PID input set: intensity1");
            #endif
        }
          if (input.charAt(6) == '2') {
            // you sent p,i,i,2
            pid::Input = &data::photo_b;
            #if DEBUG
            Serial.println("PID input set: intensity2");
            #endif
          }
        }
      }

      if (input.charAt(2) == 'o') {
        // you sent p,o,
        // FUNCTION: set PID controller Output (what it's actuating)
        if (input.charAt(4) == 'v') {
          // you sent p,o,v
          pid::actuated = &setpoint::voltage;
          myPID.SetOutputLimits(0,10);
          #if DEBUG
          Serial.println("PID output set: voltage");
          #endif
        }
        if (input.charAt(4) == 'f') {
          // you sent p,o,f
          pid::actuated = &setpoint::frequency;
          myPID.SetOutputLimits(10,20);
          #if DEBUG
          Serial.println("PID output set: frequency");
          #endif
        }
        if (input.charAt(6) == 'q') {
          // you sent p,o,q
          pid::actuated = &setpoint::flowrate;
          myPID.SetOutputLimits(0,3);
          #if DEBUG
          Serial.println("PID output set: flow rate");
          #endif
        }
        if (input.charAt(6) == 'd') {
          // you sent p,o,d
          pid::actuated = &setpoint::duty;
          myPID.SetOutputLimits(0,100);
          #if DEBUG
          Serial.println("PID output set: duty cycle");
          #endif
        }
      }

      if (input.charAt(2) == 't') {
        // you sent p,t,
        // FUNCTION: set PID tuning

        if (input.charAt(4) == 'p') {
          pid::Kp = input.substring(6).toFloat();
          myPID.SetTunings(pid::Kp,myPID.GetKi(),myPID.GetKd());
          #if DEBUG
            Serial.println("Kp set!");
          #endif
        }
  
        if (input.charAt(4) == 'i') {
          pid::Ki = input.substring(6).toFloat();
          myPID.SetTunings(myPID.GetKp(),pid::Ki,myPID.GetKd());
          #if DEBUG
            Serial.println("Ki set!");
          #endif
        }
  
        if (input.charAt(4) == 'd') {
          pid::Kd = input.substring(6).toFloat();
          myPID.SetTunings(myPID.GetKp(),myPID.GetKi(),pid::Kd);
          #if DEBUG
            Serial.println("Kd set!");
          #endif
        }
      }


      
      actuate_inputs();
      break;

    // INSERT CASE X
  }
}

void actuate_inputs() {
  // actuates the system inputs via the DACs
  #if DEBUG
    Serial.println("actuating inputs...");
  #endif
  //DAC_FXN.Set(mapfloat(setpoint::voltage,0,10,0,DACSTEPS-1), mapfloat(setpoint::frequency,0,10,DACSTEPS-1,0));
  //DAC_FXN.Set(mapfloat(setpoint::voltage,0,10,((float) 6 / 10)*DACSTEPS,((float) 2 / 10)*DACSTEPS), mapfloat(setpoint::frequency,0,10,DACSTEPS-1,0));
  
  // set V,F DAC
  // tuned for Brandon's development setup on 24V supply:
  //DAC_FXN.Set(mapfloat(setpoint::voltage,0,10,0.96*(DACSTEPS-1),0.55*(DACSTEPS-1)), 
  //        mapfloat(setpoint::frequency,10,20,0.92*(DACSTEPS-1),0.1*(DACSTEPS-1)));

  // tuned for the control jet setup on 24V supply:
  DAC_FXN.Set(mapfloat(setpoint::voltage,0,10,0.96*(DACSTEPS-1),0.55*(DACSTEPS-1)), 
          mapfloat(setpoint::frequency,10,20,0.90*(DACSTEPS-1),0.085*(DACSTEPS-1)));
  
  // set MFC DAC     
  DAC_MFC.Set(mapfloat(setpoint::flowrate,0,10,0,4095),0);
  
  // set duty cycle
  analogWrite(PIN_PWM, mapfloat(setpoint::duty,0,100,0,255));
}

int get_crc(String mystring) {
  // calculate the CRC8 of the string
  // first, convert string to array of chars (signed ints)
  char mychars[mystring.length()+1];
  mystring.toCharArray(mychars,sizeof(mychars));
  // then, convert chars into unsigned ints
  uint8_t myuints[sizeof(mychars)];
  for (int i=0; i < sizeof(mychars); i++) {
    myuints[i] = (uint8_t) mychars[i];
  }
  // finally, calculate the crc of the unsigned ints
  int mycrc = crc8(myuints,sizeof(myuints));
  return mycrc;
}

String get_datastring() {
  // build the string from the data
  String mystring = "";
  mystring += data::ts;
  mystring += ",\t";
  mystring += setpoint::voltage;
  mystring += ",\t";
  mystring += setpoint::frequency;
  mystring += ",\t";
  mystring += setpoint::flowrate;
  mystring += ",\t";
  mystring += setpoint::duty;
  mystring += ",\t";
  mystring += data::dist;
  mystring += ",\t";
  mystring += data::photo_a;
  mystring += ",\t";
  mystring += data::photo_b;
  mystring += ",\t";
  mystring += data::tamb;
  mystring += ",\t";
  mystring += myPID.GetMode();
  mystring += ",\t";
  mystring += pid::Setpoint;
  mystring += ",\t";
  mystring += *pid::Input;
  mystring += ",\t";
  mystring += pid::Output;
  return mystring;
}

float mapfloat(float x, long in_min, long in_max, long out_min, long out_max)
{
 return (x - in_min) * ((float)out_max - out_min) / ((float)in_max - in_min) + out_min;
}

void setup_pid() {
  //turn the PID on
  //pid::Input = &data::photo_a;
  //pid::Setpoint = 0;
  //pid::actuated = &setpoint::duty;
  myPID.SetOutputLimits(0,100);
  myPID.SetSampleTime(100); // in milliseconds
  myPID.SetMode(AUTOMATIC);
  actuate_inputs();
}

void setup_proximity() {
  // set up VL6180X sensor
  proxsensor.init();
  proxsensor.configureDefault();
  proxsensor.setTimeout(PROXIMITY_TIMEOUT);
}

void setup()
{
  SPI.begin();
  Wire.begin();
  setup_serial();
  setup_watchdog();
  setup_proximity();
  setup_pid();
}

void loop()
{
  // reset the watchdog timer
  // if this doesn't occur within WDTO_X, system resets
  wdt_reset();

  if ( myPID.Compute() ) {
    *pid::actuated = pid::Output;
    // actuate the system inputs via the interfaced hardware
    actuate_inputs();
  }
  
  if (Serial.available() > 0) {
    #if DEBUG
      Serial.println("bytes available: " + String(Serial.available()));
    #endif
    get_serial();
  }

  // get the timestamp
  data::ts = millis();
  
  // read and save data from sensors
  data::dac_a = analogRead(PIN_DAC_A);
  data::dac_b = analogRead(PIN_DAC_B);
  data::photo_a = analogRead(PIN_PHOTO_A);
  data::photo_b = analogRead(PIN_PHOTO_B);
  data::dist = proxsensor.readRangeSingleMillimeters(); // read VL6180X distance
  data::tamb = mlx_5deg.readAmbientTempC();

  // build the string from the data
  String mystring = get_datastring();

  // generate the CRC8 of the string
  int mycrc = get_crc(mystring);

  // printing the data string
  Serial.print(mystring);
  Serial.print(",\t");

  // printing the crc
  Serial.print(mycrc);
  Serial.println();
  delay(LOOPDELAY);
}
