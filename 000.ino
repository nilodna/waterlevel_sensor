#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

Adafruit_BMP280 bmp; 

long duration;              // duracao do eco do sensor de ultrassom
double cm;                  // distancia em cm sensor ultrassom
const int pingPin = 7;      // pino do ultrassom
const int chipSelect = 10;  // chipselect do shield SD
int sharp;                  // leitura analogica do sensor otico (pino A0)
float distance;             // distancia calculada com o sensor otico

void setup() {

  Serial.begin(9600);
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  else{
    Serial.println("card initialized.");
  }

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


  File dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.write("Ano Mes Dia H M S Dist(cm) Dist2(cm) T(°C) P(Pa)"); //cabecalho de dados
    dataFile.println();
    dataFile.close();

  
}

void loop() {

  tmElements_t tm;    //objeto para RTC
  
  // make a string for assembling the data to log:
  String dataString = "";

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH , 30000 );

  sharp = analogRead(A0);


  if (RTC.read(tm)) {
    dataString += tmYearToCalendar(tm.Year);
    dataString += ' ';
    dataString += tm.Month;
    dataString += ' ';
    dataString += tm.Day;
    dataString += ' ';
    dataString += tm.Hour;
    dataString += ' ';
    dataString += tm.Minute;
    dataString += ' ';
    dataString += tm.Second;
    dataString += ' ';

  } 

  cm = duration*0.017;
  dataString += cm;
  dataString += ' ';

  distance = 9462.0/(sharp - 16.92);
  dataString += distance;
  dataString += ' ';
  
  dataString += bmp.readTemperature();
  dataString += ' ';
  
  dataString += bmp.readPressure();

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
    Serial.println(dataString);
delay(500);
  
}
