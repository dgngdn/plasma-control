/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/

bool stuff = true;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.print(sensorValue*(5./1000.)*(1000/10));
  Serial.print(',');
  Serial.println(stuff);

  if (Serial.available() > 0) {
    get_serial();
  }
  
  delay(50);        // delay in between reads for stability
}

void get_serial() {
  #if DEBUG
    Serial.println("getting serial input...");
  #endif

  String inputString = "";         // a string to hold incoming data
  boolean stringComplete = false;  // whether the string is complete

  while (Serial.available() > 0) {

    // get the new byte:
    char inChar = (char)Serial.read();

    //if (isDigit(inChar)) {
      // add it to the inputString:
      inputString += inChar;
    //}
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      break;
    }
  }

    if ( stringComplete == true && inputString.length() > 0 ) {
      Serial.println(inputString);
      if (inputString.charAt(0) == '1') {
        digitalWrite(13, HIGH);
      }
      else if (inputString.charAt(0) == '2') {
        digitalWrite(13, LOW);
      }
    }
  }

