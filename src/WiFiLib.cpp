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
	setSSID("default");
	setPass("");
}

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
		send(F("\",1,0"), 500); // Last param, ench: 0 OPEN; 2 WPA_PSK; 3 WPA2_PSK; 4 WPA_WPA2_PSK 
	}
	if (mode != '2') { // Configure STA
		for(char i = 0; i < 5; i++) {
			sendPart(F("AT+CWJAP=\""));
			sendPart(ssid);
			sendPart(F("\",\""));
			sendPart(pass);
			String res = send(F("\""), 10000);
			if(res.indexOf("OK") >= 0) {
				break;
			}
		}
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
	if(ssid != NULL) {
		free(ssid);
	}
	ssid = (char *) malloc((s.length() + 1) * sizeof(char));
	s.toCharArray(ssid, s.length());
	ssid[s.length()] = '\0';
}

void WiFiLib::setSSID(const char s[]) {
	if(ssid != NULL) {
		free(ssid);
	}
	ssid = (char *) malloc((strlen(s) + 1) * sizeof(char));
	strcpy(ssid, s);
	ssid[strlen(s)] = '\0';
}

void WiFiLib::setPass(const String s) {
	if(pass != NULL) {
		free(pass);
	}
	pass = (char *) malloc((s.length() + 1) * sizeof(char));
	s.toCharArray(pass, s.length());
	pass[s.length()] = '\0';
}

void WiFiLib::setPass(const char s[]) {
	if(pass != NULL) {
		free(pass);
	}
	pass = (char *) malloc((strlen(s) + 1) * sizeof(char));
	strcpy(pass, s);
	pass[strlen(s)] = '\0';
}



void WiFiLib::wifiLoop() {
    if(WiFiLib_DEV.available()) {
		char IPDSteps = 0, c;
		int ipd;
		String route = "";
		long int time = millis() + 500;
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
			char *tmpstr = NULL;
			while (last != NULL) {
				tmpstr = NULL;
				if (last->mode == 4) {// 4 Default route
						found = true;
				} else if (strlen(last->route) <= route.length()) {
					switch (last->mode) {
						case 3:// 3 found in any position
							found = route.indexOf(*last->route) >= 0;
							break;

						case 2:// 2 ends with
							found = route.endsWith(last->route);
							break;

						case 1:// 1 starts with
							found = route.startsWith(last->route);
							break;

						case 0:// 0 same string
						default:
							found = route.equals(last->route);
							break;
					}
					if (tmpstr != NULL) {
						free(tmpstr);
					}
				}
				if (found) {
					last->fp(route, ipd);
					sendPart(F("AT+CIPCLOSE="));
					send(String(ipd, DEC), 100);
					return;
				}
				last = (IPDStruct *) last->next;
			}
		sendDataByIPD(ipd, F("404 - Not found"));
		send(String(ipd, DEC), 100);
		last->fp(route, ipd);
		sendPart(F("AT+CIPCLOSE="));
		send(String(ipd, DEC), 100);
		}			
	}
}

void WiFiLib::attachRoute(const String route, void (*fp)(const String, const int), const char mode) {
	IPDStruct * last = (IPDStruct *) _attachRoute_common();
	last->route = (char *) malloc(sizeof(char) * (route.length() + 1));
	route.toCharArray(last->route, route.length());
	last->route[route.length() + 1] = '\0';
	last->fp = fp;
	last->mode = mode;
}

void WiFiLib::attachRoute(const char route[], void (*fp)(const String, const int), const char mode) {
	IPDStruct * last = (IPDStruct *) _attachRoute_common();
	last->route = (char *) malloc(sizeof(char) * (strlen(route) + 1));
	strcpy(last->route, route);
	last->route[strlen(route) + 1] = '\0';
	last->fp = fp;
	last->mode = mode;
}

void * WiFiLib::_attachRoute_common() {
	IPDStruct * last;
	if (IPDs != NULL) {
		last = IPDs;
		while (last->next != NULL) {
			last = (IPDStruct *) last->next;
		}
		last->next = (IPDStruct *) malloc(sizeof(IPDStruct));
		last = (IPDStruct *) last->next;
	} else {
		IPDs = (IPDStruct *) malloc(sizeof(IPDStruct));
		last = IPDs;
	}
	last->next = NULL;
	return last;
}


String WiFiLib::send(const String command, const int timeout, const bool removeNL) {
    WiFiLib_DEV.print(command);
    return _send_common(timeout, removeNL);
}

String WiFiLib::send(const char command[], const int timeout, const bool removeNL) {
    WiFiLib_DEV.print(command);
    return _send_common(timeout, removeNL);
}

String WiFiLib::send(const char command, const int timeout, const bool removeNL) {
    WiFiLib_DEV.print(command);
    return _send_common(timeout, removeNL);
}

String WiFiLib::send(const int command, const int timeout, const bool removeNL) {
    WiFiLib_DEV.print(command);
    return _send_common(timeout, removeNL);
}

String WiFiLib::_send_common(const int timeout, const bool removeNL) {
    String response = "";
	char c;
    long int time;
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


void WiFiLib::sendDataByIPD(const int ipd, const String data, const int timeout) {
	_sendDataByIPD_common(ipd);
	send((int) data.length(), 100);
	send(data, timeout, true);
}

void WiFiLib::sendDataByIPD(const int ipd, const char data[], const int timeout) {
	_sendDataByIPD_common(ipd);
	send((int) strlen(data), 100);
	send(data, timeout, true);
}

void WiFiLib::sendDataByIPD(const int ipd, const char data, const int timeout) {
	_sendDataByIPD_common(ipd);
	send((int) sizeof(data), 100);
	send(data, timeout, true);
}

void WiFiLib::sendDataByIPD(const int ipd, const int data, const int timeout) {
	String str = String(data);
	sendDataByIPD(ipd, str, timeout);
}


void WiFiLib::_sendDataByIPD_common(const int ipd) {
	sendPart(F("AT+CIPSEND="));
	sendPart(ipd);
	sendPart(F(","));
}




// Used for setting-up Wifi Module to desired speed.
// Remember to change Arduino sketch speed when changing to adapt to new one.
void WiFiLib::setBaudRate(const String br) {
	sendPart(F("AT+CIOBAUD="));
	send(br, 200);
}

void WiFiLib::_clearRoutes(IPDStruct * act) {
	if (act->next != NULL) {
		_clearRoutes((IPDStruct *) act->next);
		free(act->next);
		act->next = NULL;
	}

}

void WiFiLib::clearRoutes() {
	if (IPDs != NULL) {
		_clearRoutes(IPDs);
		free(IPDs);
		IPDs = NULL;
	}
}

