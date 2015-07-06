//Step 0: Insert SIM Card to Module GPS GPRS
//Step 1: S1 to Prog
//Step 2: S2 to Arduino
//Step 3: Change Phone Receiber and Upload Sketch
//Step 4: S1 to Comm
//Step 5: Plug the long side of LED into pin 8 and short side into GND
//Step 6: Wait for LEDs Stat and Network OFF, PWR On Red
//Step 7: Open Arduino Monitor Serial and Press RST Board
#include <Time.h>
#include "LogOutput.h"
#include "GPSData.h"
#include "DFRobotGPS.h"

/*** GLOBAL VARIABLES ***/
float CurrentVoltage;
float PreviousVoltage;

enum Command
{
  Move,
  COUNT  //Must be the last item
};

/*** MAIN FUNCTIONS ***/
void setup()
{
    Serial.begin(9600);                 //Set Baud Rate
    Serial.println("Starting");
	GPS_Setup(false);
}

void loop(){
  LogOutput("Loop start.");
  while( Serial.available() > 0) Serial.read();    
  delay(100);
  
  ////////////// Battery Monitoring ///////////
  CurrentVoltage = (float)readVcc() / (float)1000;
  LogOutput((String)"VCC is: "+CurrentVoltage+"V");
  if(CurrentVoltage != PreviousVoltage)
    LogData((String)CurrentVoltage, BatteryDataType);
  PreviousVoltage = CurrentVoltage;

  ////////////// GPS /////////////////////
  if(GPS_Lock())
  {
    String response = "";
    sendATcommand("AT+CGPSINF=32", "OK", 1000, &response);
    LogData(response.substring(response.indexOf('32,')+1), GPSDataType);
    ParseGPRMC(response);
  }
  
  // Do other work here
  WaitForCommand();
}

/* Waits for 500ms to receive a command and processes it if received */
void WaitForCommand()
{
  LogOutput("Waiting for command");
  String commandLine = "";

  int answer = 0;
  // Turn off GSM and GPS UART
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  answer = sendATcommand("~~RDY~~,", ":CMDEND", 1000, &commandLine);
  // Turn GSM UART back on
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);

  LogOutput("GOt here");
  if(answer <= 0)    
  {
    return;
  }
  //LogOutput((String)"Got Command Line: "+commandLine);   
  LogOutput((String)"Command: "+commandLine.substring(4,6));
  // Get the command value
  int cmdInt = commandLine.substring(4,6).toInt();
  LogOutput((String)"Command: "+(String)cmdInt);
  
  if((Command)cmdInt < COUNT)
  {
    LogOutput((String)"Got valid command: "+commandLine.substring(6));
    LogData((String)"", CommandAckType); 
  }
}

/**  BATTERY FUNCTIONS **/
/* from http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/ */
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
