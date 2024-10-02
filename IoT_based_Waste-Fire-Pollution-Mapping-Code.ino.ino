// Master code for EDAI Project Sensors

//Integrating sensors MQ-7, GP2Y10, MQ-4, MQ-135
#include "MQ7.h"
int AQI;
#include <SPI.h>
#include <SD.h>
#define RLOAD 23.5      ////MQ135 code
#include "MQ135.h"
#define RLOAD 10
#include <SPI.h>



MQ135 gasSensor = MQ135(A3);
int val;
int sensorPin = A3;
int sensorValue = 0;
 /////MQ135 code finish

const int chipSelect = 28;

#define A_PIN 4
#define VOLTAGE 5


const int AO_Pin = 3; // Connect the AO of MQ-4 sensor with analog channel 0 pin (A0) of Arduino
const int DO_Pin = 6; // Connect the DO of MQ-4 sensor with digital pin 8 (D8) of Arduino

// init MQ7 device
MQ7 mq7(A_PIN, VOLTAGE);

int measurePin_GP2Y10 = A5;
int ledPower = 12;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  Serial.begin(9600);

  Serial.begin(9600);
  pinMode(sensorPin, INPUT); //MQ135

  pinMode(ledPower, OUTPUT);
  Serial.println("Calibrating MQ7");
  mq7.calibrate();    // calculates R0
  Serial.println("Calibration done!");
  pinMode(DO_Pin, INPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  

}

void loop()
{
  float dustDensity = optical_dust();
  String dataString = "";

  val = analogRead(A3);
  float AQI_ppm = gasSensor.getPPM();
  Serial.print ("AQI_ppm = ");
  Serial.println (AQI_ppm); //MQ135

  
  Serial.print("Dust Density:" );
  
  Serial.println(dustDensity);
  Serial.println("Carbon Monoxide concentration in PPM = "); Serial.println(mq7.readPpm());
  Serial.println("");   // blank new line
  Serial.print("Methane Conentration: "); // Print out the text "Methane Concentration: "
  Serial.println(AO_Pin); // Print out the methane value - the analog output - beteewn 0 and 1023
  

dataString += String(mq7.readPpm());
dataString += ",  "; 
dataString += String(AO_Pin);
dataString += ",  "; 
dataString += String(AQI_ppm);
dataString += ",  "; 
dataString += String(dustDensity);

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
  delay(1000);
}

float optical_dust()
{
  
  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin_GP2Y10);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (5.0 / 1024);
  dustDensity = 0.17 * calcVoltage - 0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  //  Serial.println("Raw Signal Value (0-1023):");
  //  Serial.println(voMeasured);
  //
  //  Serial.println("Voltage:");
  //  Serial.println(calcVoltage);
  return dustDensity;
  delay(1000);
}
