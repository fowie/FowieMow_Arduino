#include "LogOutput.h"

#define OutputSerial true

void LogOutput(String str)
{
  char buf[str.length()+1];
  str.toCharArray(buf, str.length()+1);
  LogOutput(buf);
}

void LogOutput(int val)
{
  char buf [4];
  sprintf (buf, "%03i", val);
  LogOutput(buf);
}

void LogOutput(long val)
{
  char buf[5];
  sprintf(buf, "%03f", val);
  LogOutput(buf);
}

void LogOutput(char val)
{
  char buf[2];
  sprintf(buf, "%c", val);
  LogOutput(buf);  
}

void LogOutput(char* logstr)
{
  if(OutputSerial)
  {
    // Turn off GSM and GPS UART
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    Serial.print("BS:");
    Serial.println(logstr);
    delay(500);
    // Turn GSM UART bac
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
  }
}

// Outputs with the special "data" character heading for the brain of the system to parse
void LogData(String data, DataType type)
{
  switch(type)
  {
    case GPSDataType:
      data = "~~GPRMC~~,"+data;
      break;
    case BatteryDataType:
      data = "~~BATT~~,"+data;
      break;
    case StatusDataType:
      data = "~~STATUS~~,"+data;
      break;
    case CommandAckType:
      data = "ACK";
      break;
    case CommandReadyType:
      data = "~~RDY~~,";
      break;
    default:
      break;
  }
  
  char buf[data.length()+1];
  data.toCharArray(buf, data.length()+1);
  
  // Turn off GSM and GPS UART
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  Serial.println(data);
  delay(500);
  // Turn GSM UART bac
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
}

