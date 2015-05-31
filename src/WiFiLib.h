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
#ifndef WIFILIB
	#define WIFILIB
	#include "Arduino.h"

	#ifndef WiFiLib_DEV
		#ifdef _VARIANT_ARDUINO_STM32_
			#define WiFiLib_DEV Serial2
		#else
			#define WiFiLib_DEV Serial
		#endif
	#endif

	class WiFiLib {
		public:
			WiFiLib();
			void reinit();
			void sendPart(const String);
			String send(const String, const int);
			void setMode(const char);
			void setSSID(const String);
			void setPass(const String);
			String getIPInfo();

		private:
			String buffer;
			String ssid = "WifiDefault";
			String pass = "WifiDefault";
			char mode = '2'; //1= Sta, 2= AP, 3=both. Sta is a device, AP is a router
	};
#endif
