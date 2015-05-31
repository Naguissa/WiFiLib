/**
 * ESP8266 WiFi Library
 *
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa.com@gmail.com
 * @version 1.0
 * @created 2015-05-07
 */
#include <Arduino.h>
#include "WiFiLib.h"

WiFiLib::WiFiLib() {
}



void WiFiLib::reinit() {
	String m = "AT+CWMODE=";
	m += mode;
	send(F("AT+RST"), 2000);
	send(m, 1000); // configure as access point

	if (mode != '1') { // Configure AP
		sendPart(F("AT+CWSAP=\""));
		sendPart(ssid);
		sendPart(F("\",\""));
		sendPart(pass);
		send(F("\",1,0"), 1000); // Last param, ench: 0 OPEN; 2 WPA_PSK; 3 WPA2_PSK; 4 WPA_WPA2_PSK 
	}
	if (mode != '2') { // Configure STA
		sendPart(F("AT+CJSAP=\""));
		sendPart(ssid);
		sendPart(F("\",\""));
		sendPart(pass);
		send(F("\""), 1000);
	}


	send(F("AT+CIPMUX=1"), 1000); // configure for multiple connections
	send(F("AT+CIPSERVER=1,80"), 1000); // turn on server on port 80
}


String WiFiLib::getIPInfo() {
	return send(F("AT+CIFSR"), 1000); // get ip address
}

void WiFiLib::setMode(const char m) {
	mode = m;
}

void WiFiLib::setSSID(const String s) {
	ssid = s;
}

void WiFiLib::setPass(const String s) {
	pass = s;
}




void WiFiLib::sendPart(const String command) {
    WiFiLib_DEV.print(command);
// ToDo: Remove debug
	Serial.print(command);
}

String WiFiLib::send(const String command, const int timeout) {
    String response = "";
	char c;
    long int time;
    WiFiLib_DEV.print(command + F("\n\r"));
    time = millis();
    while ((time + timeout) > millis()) {
    	while (WiFiLib_DEV.available()) {
			c = WiFiLib_DEV.read();
			response += c;
		}
	}
// ToDo: Remove debug
Serial.println(command);
Serial.println(response);
    return response;
}

