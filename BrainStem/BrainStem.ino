//Step 0: Insert SIM Card to Module GPS GPRS
//Step 1: S1 to Prog
//Step 2: S2 to Arduino
//Step 3: Change Phone Receiber and Upload Sketch
//Step 4: S1 to Comm
//Step 5: Plug the long side of LED into pin 8 and short side into GND
//Step 6: Wait for LEDs Stat and Network OFF, PWR On Red
//Step 7: Open Arduino Monitor Serial and Press RST Board
#include <Encoder.h>
#include <Time.h>
#include "LogOutput.h"

/*** GLOBAL VARIABLES ***/
float CurrentVoltage;
float PreviousVoltage;
#define encoder0PinA  11
#define encoder0PinB  12
#define encoder1PinA  9
#define encoder1PinB  10
Encoder rightMotorEncoder(encoder0PinA, encoder0PinB);
Encoder leftMotorEncoder(encoder1PinA, encoder1PinB);
long rightMotorPosition = -999;
long leftMotorPosition = -999;

// Actual motor power command values [0,255]?
unsigned int leftMotor = 0;
unsigned int rightMotor = 0;
String* response = new String("");

enum Command
{
  Move,
  EncoderQuery,
  BatteryQuery,
  COUNT  //Must be the last item
};

/*** MAIN FUNCTIONS ***/
void setup()
{
    Serial.begin(115200);                 //Set Baud Rate
    Serial.println("Starting");
	while (Serial.available() > 0) Serial.read();
}

void loop(){
  //LogOutput("Loop start.");
  //delay(100);

  ////////////// Battery Monitoring ///////////
  CurrentVoltage = (float)readVcc() / (float)1000;
  //if(CurrentVoltage != PreviousVoltage)
  //	  Serial.println((String)"~~BATT~~" + CurrentVoltage);
  PreviousVoltage = CurrentVoltage;
   
  // Read encoders
  long newLeft, newRight;
  newLeft = leftMotorEncoder.read();
  newRight = rightMotorEncoder.read();
  if (newLeft != leftMotorPosition || newRight != rightMotorPosition) {
	  //Serial.println((String)"~~ENC~~" + newLeft + "|" + newRight);
	  leftMotorPosition = newLeft;
	  rightMotorPosition = newRight;
  }

  WaitForCommand();
}

/* Waits for 500ms to receive a command and processes it if received */
void WaitForCommand()
{
  int answer = 0;

  while (Serial.available() > 0)
	*response += (char)Serial.read();

	// check if the desired answer is in the response of the module
	if (response->lastIndexOf(":CMDEND:") != -1)
	{
		answer = 1;
	}

  if(answer <= 0)    
  {
	// Not valid (yet), return to do other work 
	// maybe next time I come back I can finish the command
	// Serial.println((String)"Not valid, got" + *response);
    return;
  }
  //LogOutput((String)"Got Command Line: "+commandLine);   
  //Serial.println((String)"Command: "+response->substring(4,6));
  // Get the command value
  int cmdInt = response->substring(4,6).toInt();
  Serial.println((String)"Command: "+(String)cmdInt);
  response = new String("");

  if((Command)cmdInt < COUNT)
  {
    //Serial.println((String)"Got valid command: "+response->substring(6));
	switch (cmdInt)
	{
	case Move:
		// Set motor commands 
		
		// echo back values
		Serial.println((String)"~~MOVE~~" + leftMotor + "|" + rightMotor);
		break;
	case EncoderQuery:
		// return encoder values
		Serial.println((String)"~~ENC~~" + leftMotorPosition + "|" + rightMotorPosition);
		break;
	case BatteryQuery:
		// return battery value
		Serial.println((String)"~~BATT~~" + CurrentVoltage);
		break;
	}
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
