#ifndef LOGOUTPUT
#define LOGOUTPUT

#include "Arduino.h"
#include "GPSData.h"

typedef enum DataType
{
  GPSDataType,
  BatteryDataType,
  StatusDataType,
  CommandAckType,
  CommandReadyType,
  
} DataType;


void LogData(String data, DataType type);
void LogOutput(String str);
void LogOutput(int val);
void LogOutput(long val);
void LogOutput(char val);
void LogOutput(char* log);

#endif
