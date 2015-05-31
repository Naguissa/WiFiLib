/**
 * ESP8266 WiFi Library
 *
 * Attached routes ar functions in form:
 *     void function HANDLER(const string ROUTE, const int IPD);
 *
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa.com@gmail.com
 * @version 1.0
 * @created 2015-05-07
 */
#ifndef WIFILIB
	#define WIFILIB
	#include "Arduino.h"


	///// YOU NEED TO ADJUST THIS TO YOUR MODULE; try-and-error, each of my modules came at different speed
	//#define WIFILIB_BAUDS 19200
	//#define WIFILIB_BAUDS 9600
	#ifndef WiFiLib_BAUDS
		#define WiFiLib_BAUDS 115200
	#endif

	#ifndef WiFiLib_DEV
		#ifdef _VARIANT_ARDUINO_STM32_
			#define WiFiLib_DEV Serial2
		#else
			#define WiFiLib_DEV Serial
		#endif
	#endif

	class WiFiLib {
		public:
			void reinit();
			String send(const String, const int, const bool = false);
			#define sendPart(s) send(s, 0, true)
			void setMode(const char);
			void setSSID(const String);
			void setPass(const String);
			String getIPInfo();
			void attachRoute(const String, void (*)(const String, const int), const char = 0);
			void sendDataByIPD(const int, const String);
			void wifiLoop();
			// Used for setting-up Wifi Module to desired speed.
			// Remember to change Arduino sketch speed when changing to adapt to new one.
			void setBaudRate(const String);

		private:
			String buffer;
			String ssid = "WifiDefault";
			String pass = "WifiDefault";
			char mode = '2'; //1= Sta, 2= AP, 3=both. Sta is a device, AP is a router
			struct IPDStruct {
				String * route;
				void (* fp)(const String, const int);
				char mode; // 0 same string, 1 starts with, 2 ends with, 3 found in any position
				IPDStruct * next;
			};
		IPDStruct * IPDs = NULL;
	};
#endif
