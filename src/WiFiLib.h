/**
 * DS1307 and DS3231 RTCs with AT24C32 (and compatible) integrated EEPROM basic library
 *
 * Really tiny library to basic RTC and EEPROM (incorporated) functionality on Arduino.
 *
 * DS1307 and DS3231 RTCs are supported AT24C32 EEPROM supported (and compatibles)
 *
 *
 * @copyright Naguissa
 * @author Naguissa
 * @email naguissa.com@gmail.com
 * @version 1.0
 * @created 2015-05-07
 */
#ifndef WiFiLib
	#define WiFiLib
	#include "Arduino.h"

	#ifdef _VARIANT_ARDUINO_STM32_
		#define WiFiLib_DEV Serial2
	#else
		#define WiFiLib_DEV Serial
	#endif


	// Convert normal decimal numbers to binary coded decimal
	#define WiFiLib_decToBcd(val) ((uint8_t) ((val / 10 * 16) + (val % 10)))
	//#define WiFiLib_decToBcd(val) ((uint8_t) (val + 6 * (val / 10)))

	// Convert binary coded decimal to normal decimal numbers
	//#define WiFiLib_bcdToDec(val) ((uint8_t) (val - 6 * (val >> 4)))
	#define WiFiLib_bcdToDec(val) ((uint8_t) ((val / 16 * 10) + (val % 16)))


	#ifdef _VARIANT_ARDUINO_STM32_
		#define WiFiLib_INIT_WIRE() if (_do_init) { _do_init = false; Wire.begin(); }
	#endif


	class WiFiLib {
		public:
			bool reinit();
			void sendPart(const String);
			String send(const String, const int, bool);
			void setMode(const char);
			void setSSID(const String);
			void setPass(const String);

		private:
			String buffer = "";
			String ssid = F("WifiDefault");
			String pass = F("WifiDefault");
			char mode = '2'; //1= Sta, 2= AP, 3=both. Sta is a device, AP is a router
	};
#endif
