#include <TM74HC595Display.h>

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

int SCLK = 4;
int RCLK = 3;
int DIO = 2;

TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];

void setup() {
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  LED_0F[10] = 0x88; //A
  LED_0F[11] = 0x83; //b
  LED_0F[12] = 0xC6; //C
  LED_0F[13] = 0xA1; //d
  LED_0F[14] = 0x86; //E
  LED_0F[15] = 0x8E; //F
  LED_0F[16] = 0xC2; //G
  LED_0F[17] = 0x89; //H
  LED_0F[18] = 0xF9; //I
  LED_0F[19] = 0xF1; //J
  LED_0F[20] = 0xC3; //L
  LED_0F[21] = 0xA9; //n
  LED_0F[22] = 0xC0; //O
  LED_0F[23] = 0x8C; //P
  LED_0F[24] = 0x98; //q
  LED_0F[25] = 0x92; //S
  LED_0F[26] = 0xC1; //U
  LED_0F[27] = 0x91; //Y
  LED_0F[28] = 0xFE; //hight -
  
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
}

void loop() {  
/*
  disp.send(LED_0F[0], 0b00000001);    //send digital "0" to 1st indicator
  delay(2000);
  disp.send(LED_0F[3], 0b00000110);    //send digital "3" to 2nd and 3rd indicator
  delay(2000);
  disp.send(LED_0F[10], 0b11111111);    //send simbol "A" to all indicators
  delay(2000);

  for(int i = 0; i<=99; i++){
  disp.digit2(i, 0b00000001, 50);    //send counter 0-99 with delay 50 cicles int 1st and 2nd view ports
  }

  for(int i = 0; i<=99; i++){
  disp.digit2(i, 0b00000100, 50);    //send counter 0-99 with delay 50 cicles int 1st and 2nd view ports
  }
  
  for(int i = 0; i<=99; i++){
  disp.digit2(i, 0b00010000, 50);    //send counter 0-99 with delay 50 cicles int 1st and 2nd view ports
  }

   for(int i = 0; i<=99; i++){
  disp.digit2(i, 0b01000000, 50);    //send counter 0-99 with delay 50 cicles int 3st and 4rd view ports
  }

  for(int i = 0; i<=100; i++){
  disp.digit4showZero(i, 50);        //send counter 0-100 with delay 50 cicles with zero
  }

  for(int i = 0; i<=1050; i++){
  disp.digit4(i, 50, true);          //send counter 0-9999 with delay 50 cicles and hide zero
  }

  for(int i = 0; i<=99999999; i++){
  disp.digit8(i, 25, true);          //send counter 0-9999 with delay 50 cicles and hide zero
  }
*/
  
  tmElements_t tm;

  if (RTC.read(tm)) {
    long timedisp = tm.Hour*10000L + tm.Minute*100L + tm.Second*1L;
    Serial.print(timedisp);
    disp.digit8(timedisp,10,false);
 /*
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
  */
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

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
