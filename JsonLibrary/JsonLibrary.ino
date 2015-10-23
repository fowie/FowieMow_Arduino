/*
 Name:		JsonLibrary.ino
 Created:	10/22/2015 8:07:35 PM
 Author:	Spencer
*/

// the setup function runs once when you press reset or power the board
#include <ArduinoJson.h>
void setup() {
	Serial.begin(115200);
}

// the loop function runs over and over again until power down or reset
void loop() {
	StaticJsonBuffer<500> jsonBuffer;

	//char jsonstr[500] = "{\"city\":{\"id\":5792990,\"name\":\"Duvall\",\"coord\":{\"lon\":-121.98568,\"lat\":47.742321},\"country\":\"US\",\"population\":0},\"cod\":\"200\",\"message\":0.0288,\"cnt\":1,\"list\":[{\"dt\":1445540400,\"temp\":{\"day\":42.89,\"min\":32.18,\"max\":42.89,\"night\":32.18,\"eve\":42.89,\"morn\":42.89},\"pressure\":977.15,\"humidity\":79,\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"sky is clear\",\"icon\":\"02n\"}],\"speed\":2.31,\"deg\":314,\"clouds\":8}]}";
	const char* json_src = "{\"city\":{\"id\":5792990,\"name\":\"Duvall\",\"coord\":{\"lon\":-121.98568,\"lat\":47.742321},\"country\":\"US\",\"population\":0},\"cod\":\"200\",\"message\":0.0288,\"cnt\":1,\"list\":[{\"dt\":1445540400,\"temp\":{\"day\":42.89,\"min\":32.18,\"max\":42.89,\"night\":32.18,\"eve\":42.89,\"morn\":42.89},\"pressure\":977.15,\"humidity\":79,\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"sky is clear\",\"icon\":\"02n\"}],\"speed\":2.31,\"deg\":314,\"clouds\":8}]}";
	char json[500];
	strcpy(json, json_src);
	Serial.print("JsonStr length: ");Serial.println(strlen(json));
	Serial.print("Jsonstr=[");Serial.print(json);Serial.println("]");
	//jsonstr.getBytes(buffer, jsonstr.length(), 0);
	//Serial.print("Buffer=[");Serial.print((char*)jsonstr);Serial.println("]");

	JsonObject& root = jsonBuffer.parseObject(json);

	if (!root.success())
	{
		Serial.println("Parse failed!");
	}
	else
	{
		Serial.println("Parse succeeded!");
		JsonObject& list = root["list"];
		JsonObject& temperatures = list["temp"];
		JsonObject& weather = list["weather"];
		Serial.print("City:");Serial.println((const char*)root["city"]["name"]);
		Serial.print("Temps\r\nDay: ");Serial.print((double)temperatures["day"]);Serial.print(" Min:");Serial.print((double)temperatures["min"]);Serial.print("Max: ");Serial.println((double)temperatures["max"]);
	}

	delay(1000);
}
