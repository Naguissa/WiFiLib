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



void WiFiLib::reinit() {
  	WiFiLib_DEV.begin(WiFiLib_BAUDS);
	send(F("AT+RST"), 2000);
	sendPart(F("AT+CWMODE="));
	send(String(mode), 100);
	if (mode != '1') { // Configure AP
		sendPart(F("AT+CWSAP=\""));
		sendPart(ssid);
		sendPart(F("\",\""));
		sendPart(pass);
		send(F("\",1,0"), 200); // Last param, ench: 0 OPEN; 2 WPA_PSK; 3 WPA2_PSK; 4 WPA_WPA2_PSK 
	}
	if (mode != '2') { // Configure STA
		sendPart(F("AT+CJSAP=\""));
		sendPart(ssid);
		sendPart(F("\",\""));
		sendPart(pass);
		send(F("\""), 200);
	}
	send(F("AT+CIPMUX=1"), 200); // configure for multiple connections
	send(F("AT+CIPSERVER=1,80"), 200); // turn on server on port 80
}


String WiFiLib::getIPInfo() {
	return send(F("AT+CIFSR"), 100); // get ip address
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



void WiFiLib::wifiLoop() {
    if(WiFiLib_DEV.available()) {
		char IPDSteps = 0, c;
		int ipd;
		String route = "";
		long int time = millis() + 1000;
		while (time > millis()) {
			while (WiFiLib_DEV.available()) {
				c = WiFiLib_DEV.read(); // read the next character.
				switch (IPDSteps) {
					case 0:
						if (c == '+') {
							IPDSteps++;
						} else {
							IPDSteps = 0;
						}
						break;

					case 1:
						if (c == 'I') {
							IPDSteps++;
						} else {
							IPDSteps = 0;
						}
						break;

					case 2:
						if (c == 'P') {
							IPDSteps++;
						} else {
							IPDSteps = 0;
						}
						break;

					case 3:
						if (c == 'D') {
							IPDSteps++;
						} else {
							IPDSteps = 0;
						}
						break;

					case 4:
						if (c == ',') {
							IPDSteps++;
							ipd = 0;
						} else {
							IPDSteps = 0;
						}
						break;

					case 5: // Reading IPD channel
						if (c == ',') {
							IPDSteps++;
						} else {
							ipd = ipd * 10 + c - 48;
						}
						break;

					case 6: // Length, ignored
						if (c == ':') {
							IPDSteps++;
						}
						break;

					case 7: // GET, post, etc
						if (c == ' ') {
							IPDSteps++;
						}
						break;

					case 8: // Route
						if (c == ' ') {
							IPDSteps++;
						} else {
							route += c;
						}
						break;
			
					case 9: // Ignore, request completed
						break;
			
					default:
						IPDSteps = 0;
				}
			}
		}
		if (IPDSteps == 9) { // Request fond, check routes
			IPDStruct * last = IPDs;
			bool found = false;
			while (last != NULL) {
				if (*last->route == "") {
					found = true;
				} else {
					switch (last->mode) {
						case 3:// 3 found in any position
							found = route.indexOf(*last->route) >= 0;
							break;

						case 2:// 2 ends with
							found = route.endsWith(*last->route);
							break;

						case 1:// 1 starts with
							found = route.startsWith(*last->route);
							break;

						case 0:// 0 same string
						default:
							found = route.equals(*last->route);
							break;
						}
					}
				if (found) {
					last->fp(route, ipd);
					sendPart(F("AT+CIPCLOSE="));
					send(String(ipd, DEC), 100);
					return;
				}
				last = last->next;
			}
		}			
	}
}

void WiFiLib::attachRoute(const String route, void (*fp)(const String, const int), const char mode) {
	IPDStruct * last;
	if (IPDs == NULL) {
		IPDs = (IPDStruct *) malloc(sizeof(IPDStruct));
		last = IPDs;
	} else {
		while (last->next != NULL) {
			last = last->next;
		}
		last->next = (IPDStruct *) malloc(sizeof(IPDStruct));
		last = last->next;
	}
	last->route = new String(route);
	last->fp = fp;
	last->mode = mode;
	last->next = NULL;
}


String WiFiLib::send(const String command, const int timeout, const bool removeNL) {
    String response = "";
	char c;
    long int time;
    WiFiLib_DEV.print(command);
	if (!removeNL) {
    	WiFiLib_DEV.print(F("\r\n"));
	}
    time = millis() + timeout;
    while (time > millis()) {
    	while (WiFiLib_DEV.available()) {
			c = WiFiLib_DEV.read();
			response += c;
		}
	}
    return response;
}



void WiFiLib::sendDataByIPD(const int ipd, const String data) {
	String str;
	sendPart(F("AT+CIPSEND="));
	str = String(ipd, DEC);
	sendPart(str);
	sendPart(F(","));
	str = String(data.length(), DEC);
	send(str, 100);
	send(data, 150, true);
}



// Used for setting-up Wifi Module to desired speed.
// Remember to change Arduino sketch speed when changing to adapt to new one.
void  WiFiLib::setBaudRate(const String br) {
	sendPart(F("AT+CIOBAUD="));
	send(br, 200);
}

