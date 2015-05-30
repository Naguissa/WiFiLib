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



WiFiLib:reinit() {
	send(F("AT+RST"), 2000,true);
	send(F("AT+CWMODE=") + mode, 1000, true); // configure as access point

	if (mode != '2') { // Configure AP
		sendPart(F("AT+CWSAP="));
		sendPart(ssid);
		sendPart(",");
		sendPart(pass);
		sendPart(",1,0");
	}
	send(F("AT+CIPMUX=1"), 1000, true); // configure for multiple connections
	send(F("AT+CIPSERVER=1,80"), 1000, true); // turn on server on port 80
}


String getIPInfo() {
	return send(F("AT+CIFSR"), 1000, true); // get ip address
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
    return response;
}

