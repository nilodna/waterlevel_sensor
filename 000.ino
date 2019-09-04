
 // Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

Adafruit_BMP280 bmp; 

long duration;
double cm;
const int pingPin = 7;
const int chipSelect = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");


if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */


  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.write("Ano Mes Dia H M S Dist(cm) T(°C) P(Pa)");
    dataFile.println();
    dataFile.close();

  
}

void loop() {

    tmElements_t tm;

  
  // make a string for assembling the data to log:
  String dataString = "";

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH , 10000 );

    cm = duration*0.017;

    dataString += ' ';
    dataString += cm;

    dataString += ' ';
    
    dataString += bmp.readTemperature();
    dataString += ' ';
    
    dataString += bmp.readPressure();

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
      if (RTC.read(tm)) {
        dataFile.print(tmYearToCalendar(tm.Year));
        dataFile.print(' ');
        dataFile.print(tm.Month);
        dataFile.write(' ');
        dataFile.print(tm.Day);
        dataFile.write(' ');
        dataFile.print(tm.Hour);
        dataFile.write(' ');
        dataFile.print(tm.Minute);
        dataFile.write(' ');
        dataFile.print(tm.Second);
        dataFile.print(' ');
      } 
    
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

delay(1000);
  
}
