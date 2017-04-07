#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

void setup() {
  Serial.begin(9600);
  // opt: 9600,19200,38400,57600,115200,460800,230400,500000,1000000,2000000
  //while (!Serial) ; // wait for serial
  //delay(200);
  //Serial.println("DS1307RTC Read Test");
  //Serial.println("-------------------");
}

void loop() {
  tmElements_t tm;

  if (RTC.read(tm)) {
    int starttime = micros();
    RTC.read(tm);
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print('-');
    print2digits(tm.Month);
    Serial.print('-');
    print2digits(tm.Day);
    Serial.print('_');
    print2digits(tm.Hour);
    Serial.print(':');
    print2digits(tm.Minute);
    Serial.print(':');
    print2digits(tm.Second);
    Serial.print('.');
    print3digits(millis()%1000);
    int endtime = micros();


    //Serial.print("Ok, Time = ");
    //print2digits(tm.Hour);
    //Serial.write(':');
    //print2digits(tm.Minute);
    //Serial.write(':');
    //print2digits(tm.Second);
    //Serial.print(", Date (D/M/Y) = ");
    //Serial.print(tm.Day);
    //Serial.write('/');
    //Serial.print(tm.Month);
    //Serial.write('/');
    //Serial.print(tmYearToCalendar(tm.Year));
    Serial.print('_');
    Serial.print(endtime-starttime);    
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  //delay(1000);
}

void print3digits(int number) {
  if (number >= 0 && number < 100) {
    Serial.write('0');
  }
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
