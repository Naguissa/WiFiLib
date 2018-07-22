#include <Arduino.h>
#include <WiFiLib.h>




WiFiLib ESP;


void defaultRoute(const String route, const int ipd) {
  ESP.sendDataByIPD(ipd, F("Catch-all functionality\n"));
  ESP.sendDataByIPD(ipd, F("Current request: "));
  ESP.sendDataByIPD(ipd, route);
  ESP.sendDataByIPD(ipd, F("; IPD: "));
  ESP.sendDataByIPD(ipd, String(ipd, DEC));
}

void test(const String route, const int ipd) {
  ESP.sendDataByIPD(ipd, F("Test specific route: "));
  ESP.sendDataByIPD(ipd, route);
  ESP.sendDataByIPD(ipd, F("; IPD: "));
  ESP.sendDataByIPD(ipd, String(ipd, DEC));
}

void setup() {
  // Added to 
  #ifdef _VARIANT_ARDUINO_STM32_
    	Serial.begin(9600);
  #endif
  delay(4000);
 
   // Uncomment to change ESP baud rate; remember to change WIFILIB_BAUDS after.
//  ESP.setBaudRate("115200");
//  return;

  ESP.reinit();
  ESP.attachRoute("/test", test, 0);
  ESP.attachRoute("", defaultRoute);
}

void loop() {
   ESP.wifiLoop();
}
 
