#include "LogOutput.h";
#include "GPSData.h";
#include "DFRobotGPS.h";

void setup() {
  // put your setup code here, to run once:

  // GPS is on main serial
  Serial.begin(9600);
  Serial.println("Serial connected.");
  GPS_Setup(false);
}

char buffer[500];

void loop() {
	////////////// GPS /////////////////////
	if (GPS_Lock())
	{
		String response = "";
		sendATcommand("AT+CGPSINF=32", "OK", 1000, &response);
		LogData(response.substring(response.indexOf('32,') + 1), GPSDataType);
		ParseGPRMC(response);  // don't really use this, mostly it's for updating last GPS so I could reduce number of TX of the GPS coords if no change
	}
	// That's it for the base station.  All the Arduino is doing is providing GPS data.  Heck, we could do this directly over usb....
	// But then I couldn't really use the Arduino later, and I'm forwarding the response anyway, this just gives me options to add more sensors to the Arduino later...
}
