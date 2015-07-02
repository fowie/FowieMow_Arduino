#ifndef DFROBOTGPS
#define DFROBOTGPS

#include "Arduino.h"
#include "GPSData.h"
#include "LogOutput.h"
#define onModulePin 5

void GPS_Setup(bool waitForLock);
void GSM_Board_Setup();
bool GPS_Lock();
int NextLength(String substr, int start);
int GetNextValue(String src, String* dst, int startIndex);
float ConvertToFloat(String str);
void ParseGPRMC(String gprmc);
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
int8_t sendATcommand(char* ATcommand, char* expected_answer,
        unsigned int timeout, String *response);


#endif
