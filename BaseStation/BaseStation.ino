#include "C:\Users\sfowers\OneDrive\Documents\Side Projects\Arduino\FowieMow\Shared\LogOutput.h";
#include "C:\Users\sfowers\OneDrive\Documents\Side Projects\Arduino\FowieMow\Shared\GPSData.h";
#include "C:\Users\sfowers\OneDrive\Documents\Side Projects\Arduino\FowieMow\Shared\DFRobotGPS.h";
#include <SoftwareSerial.h>

// Spark is on SoftwareSerial
SoftwareSerial sparkSerial(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:

  // GPS is on main serial
  Serial.begin(9600);
  sparkSerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (sparkSerial.available())
  {
  }
  
  if (Serial.available())
  {
  }
  
}
