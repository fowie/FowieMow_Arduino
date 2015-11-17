#ifndef GPSDATA
#define GPSDATA

typedef struct
{
float Latitude;
float Longitude;
float UTC;
float Speed;
float Course;
char LatDir;
char LonDir;
char Status;
char Mode;
String Date;
} GPSData;
#endif
