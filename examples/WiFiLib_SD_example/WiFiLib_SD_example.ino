/**
Config this to fit your board.
 - Bigger buffer means more speed, but also you have to incrase SDTIME
*/
#define WIFI_SSID "Write your wifi SSID"
#define WIFI_PASS "Write your wifi Password"

#define SDBUFFER 256
#define SDTIME 250
// SDCard CS line
#define SD_CS 10
// Led powers down when everything is prepared
#define LED_PIN 33







#include <Arduino.h>


#include <SPI.h>
#include <SD.h>

#include <WiFiLib.h>


WiFiLib ESP;


void _web_header(const int ipd) {
  ESP.sendDataByIPD(ipd, F("<html><head><title>WiFiLib SD Example</title></head><body>"));
}

void _web_footer(const int ipd) {
  ESP.sendDataByIPD(ipd, F("</body></html>"));
}

void nonSDRoute(const String route, const int ipd) {
  _web_header(ipd);
  ESP.sendDataByIPD(ipd, F("<h2>Warning</h2>"));
  ESP.sendDataByIPD(ipd, F("<p>SD card failed to start.</p>"));
  ESP.sendDataByIPD(ipd, F("<p><a href=\"/resetSD\">Insert SD and click here.</a></p>"));
  _web_footer(ipd);
}

void filesRoute(const String route, const int ipd) {
  char charbuff[SDBUFFER > route.length() ? SDBUFFER : route.length() + 1];
  int pos = 0;
  route.substring(1).toCharArray(charbuff, route.length());
  File myFile = SD.open(charbuff);
  if (myFile) {
    // read from the file until there's nothing else in it:
    char ch;
    charbuff[0] = '\0';
    while (myFile.available()) {
      ch = myFile.read();
      charbuff[pos] = ch;
      charbuff[pos + 1] = '\0';
      if (pos >= SDBUFFER) {
        ESP.sendDataByIPD(ipd, charbuff, SDTIME);
        pos = 0;
        charbuff[0] = '\0';
      } else {
        pos++;
      }
    }
    if (pos > 0) {
      ESP.sendDataByIPD(ipd, charbuff, pos > 10 ? SDTIME : 100);
    }
    // close the file:
    myFile.close();
  } else {
      _web_header(ipd);
      ESP.sendDataByIPD(ipd, F("<h2>404</h2>"));
      ESP.sendDataByIPD(ipd, F("<h3>File not found</h3>"));
     _web_footer(ipd);
  }
}




void resetSDRoute(const String route, const int ipd) {
  _setupSD();
  ESP.sendDataByIPD(ipd, F("Done."));
}  

void _setupSD() {
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS)) {
    ESP.clearRoutes();
    ESP.attachRoute("/resetSD", resetSDRoute, 0);
    ESP.attachRoute("", nonSDRoute, 4);
  } else {
    ESP.clearRoutes();
    ESP.attachRoute("/", filesRoute, 4);
  }
}

void _setupESP() {
  ESP.setMode('1');
  ESP.setSSID(WIFI_SSID);
  ESP.setPass(WIFI_PASS);
  ESP.reinit();
}

void setup() {
  pinMode(SD_CS, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(2000);
  _setupESP();
  _setupSD();
  digitalWrite(LED_PIN, LOW);
}

void loop() {
   ESP.wifiLoop();
}
 
