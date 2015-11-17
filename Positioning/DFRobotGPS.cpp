#include "DFRobotGPS.h"

GPSData PreviousGPS;
GPSData CurrentGPS;

void GPS_Setup(bool waitForLock)
{
  int GPSLockRetryCount = 0;
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(onModulePin, OUTPUT);
  delay(500);
  
  digitalWrite(3,LOW);               //Enable  GSM UART
  digitalWrite(4,HIGH);              //Disable GPS UART
  GSM_Board_Setup();
  LogData((String)"GSM Board Online", StatusDataType);
  
  // Power on GPS
  sendATcommand("AT+CGPSPWR=1", "OK", 2000);
  // Set GPS to Autonomous mode
  sendATcommand("AT+CGPSRST=1", "OK", 2000);

  LogData((String)"GPS Chip Online", StatusDataType);
  
  while(!GPS_Lock() && waitForLock)
  {
    LogData((String)"Waiting for GPS Lock", StatusDataType);
    delay(1500);
  }
  //LogData((String)"GPS Locked", StatusDataType);
}

void GSM_Board_Setup()
{
  LogOutput("Powering up GSM.  If message repeats.  Check that S2 is set to Arduino, not USB");
  uint8_t answer=0;
  
  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  // waits for an answer from the module
  while(answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin,HIGH);
    delay(3000);
    digitalWrite(onModulePin,LOW);
    delay(3000);
    // Send AT every two seconds and wait for the answer
    answer = sendATcommand("AT", "OK", 2000);    
    LogOutput("No answer.  Resetting GSM chip.");
  }
}

bool GPS_Lock()
{  
  String response = "";
  // Wait for 3D or 2D Fix
  if( sendATcommand("AT+CGPSSTATUS?", "D Fix", 5000, &response) > 0 )
  {
    if(response.indexOf("3D") > 0)
    {
      LogOutput("3D FIX");
    }
    else if(response.indexOf("2D") > 0)
    {
      LogOutput("2D FIX");
    }
    return true;
  }
  return false;
} // end check for GPS lock

// Get the length from start to the next comma
int NextLength(String substr, int start)
{
  //LogOutput((String)"Substring is: "+substr.substring(start));
  return substr.substring(start).indexOf(',');
}

// Gets the next comma-separated value from src starting at startIndex and puts it in dst, returning the next startIndex (position after the comma)
int GetNextValue(String src, String* dst, int startIndex)
{
  int len = NextLength(src, startIndex);
  *dst += src.substring(startIndex, startIndex+len);
  //LogOutput((String)"Src:"+src+"\r\nDst:"+*dst+"\r\nIndex:"+startIndex);
  return startIndex+len+1;
}

// Converts the string to a float
float ConvertToFloat(String str)
{
  char floatbuf[32]; // make this at least big enough for the whole string
  str.toCharArray(floatbuf, sizeof(floatbuf));
  float f = atof(floatbuf);  
  return f;
}

//based on GPRMC mode (32) from the pdf at  http://www.dfrobot.com/image/data/TEL0051/3.0/SIM908_AT%20Command%20Manual_V1.01.pdf page 223
void ParseGPRMC(String gprmc)
{
  //LogOutput((String)"Original:"+gprmc);
  int start = gprmc.indexOf('32,')+1;
  
  String utcStr = "";
  start = GetNextValue(gprmc, &utcStr, start);
  float utc = ConvertToFloat(utcStr);
  
  String stat = "";
  start = GetNextValue(gprmc, &stat, start);

  String latStr = "";
  start = GetNextValue(gprmc, &latStr, start);
  float latitude = ConvertToFloat(latStr);
  
  String latDir = "";
  start = GetNextValue(gprmc, &latDir, start);

  String lonStr = "";
  start = GetNextValue(gprmc, &lonStr, start);
  float longitude = ConvertToFloat(lonStr);
  
  String lonDir = "";
  start = GetNextValue(gprmc, &lonDir, start);
  
  String speedStr = "";
  start = GetNextValue(gprmc, &speedStr, start);
  float speedOverGround = ConvertToFloat(speedStr);
  
  String courseStr = "";
  start = GetNextValue(gprmc, &courseStr, start);
  float course = ConvertToFloat(courseStr);
  
  String date = "";
  start = GetNextValue(gprmc, &date, start);
 
  // skip Magnetic Variation because it's always empty
  start += 1;
  // skip east/west indicator, it's always east because they don't use magnetic variation
  start += 3;
  
  String mode = "";
  start = GetNextValue(gprmc, &mode, start);
  
  CurrentGPS.Latitude = latitude;
  CurrentGPS.Longitude = longitude;
  CurrentGPS.UTC = utc;
  CurrentGPS.Speed = speedOverGround;
  CurrentGPS.Course = course;
  CurrentGPS.LatDir = latDir.charAt(0);
  CurrentGPS.LonDir = lonDir.charAt(0);
  CurrentGPS.Status = stat.charAt(0);
  CurrentGPS.Mode = mode.charAt(0);
  CurrentGPS.Date = date;
  
  if(CurrentGPS.Latitude != PreviousGPS.Latitude ||
    CurrentGPS.Longitude != PreviousGPS.Longitude ||
    CurrentGPS.Speed != PreviousGPS.Speed ||
    CurrentGPS.Course != PreviousGPS.Course)
  {
     LogData(gprmc.substring(gprmc.indexOf('32,')+1), GPSDataType);
  }
  PreviousGPS = CurrentGPS;  
  
  //LogOutput((String)"Got:\r\nUTC:"+CurrentGPS.UTC+"\r\nStatus:"+CurrentGPS.Status+"\r\nLatitude:"+CurrentGPS.Latitude+" "+CurrentGPS.LatDir+"\r\nLongitude:"+CurrentGPS.Longitude+" "+CurrentGPS.LonDir+"\r\nSpeed:"+CurrentGPS.Speed+"\r\nCourse:"+CurrentGPS.Course+"\r\nMode:"+CurrentGPS.Mode+"\r\nDate:"+CurrentGPS.Date);
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout)
{
  String discard = "";
  return sendATcommand(ATcommand, expected_answer, timeout, &discard);
}

int8_t sendATcommand(char* ATcommand, char* expected_answer,
        unsigned int timeout, String *response)
{
    uint8_t answer=0;
    unsigned long previous;

    // Clean the input buffer
    while( Serial.available() > 0) Serial.read();    
    
    previous = millis();
    Serial.println(ATcommand);    // Send the AT command 

    // this loop waits for the answer
    do{
        while(Serial.available() > 0)
            *response += (char)Serial.read();

        // check if the desired answer is in the response of the module
        if (response->lastIndexOf(expected_answer) != -1)
        {
            answer = 1;
        }
        // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    
    /*String output = "Received: ";
    output += *response;
    LogOutput(output);*/

    return answer;
}

