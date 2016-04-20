/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */
 
#define FASTADC true

// Define register bits for various ADC prescaler values
// see Table 23-4 in Atmega datasheet
// via http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 
int tstart;
int tend;
int tdelay;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200); // high speed serial connection
  //Serial.begin(9600);  // low speed serial connection
  
#if FASTADC
  // set up the ADC
  // remove bits set by Arduino library
  ADCSRA &= ~PS_128;  
  // set prescalers defined above: PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_16;
#endif
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  //int sensorValue = analogRead(A5);
  // print out the value you read:
  //Serial.print(millis());
  //tstart = micros();                 // TIMING START
  int sensorValue = analogRead(A5);
  Serial.print(sensorValue);
  //tend = micros();                   // TIMING END
  //tdelay = tend-tstart;
  //Serial.print(",");               // csv
  //Serial.print(tdelay);            // prints timing duration 
  //Serial.print(",");               // csv
  //Serial.print(sensorValue);         // prints analog sensor value
  Serial.println();                  // prints newline
  //delay(100);                        // delay in between reads for stability
}
