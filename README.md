# Arduino and STM32 WiFiLib

## What is this repository for? ##

ESP8266 library to manage WiFi operations


## How do I get set up? ##

 * Get the ZIP.
 * Rename to RTCLib.zip
 * Install library on Arduino

 * Optionally you can edit WiFiLib.h to change default values for essid, password or mode (not needed, you can change them using setters).


You can change device Serial and Baud rate using before #include:
 * #define WiFiLib_BAUDS 115200
 * #define WiFiLib_DEV Serial

Default values are:
 * WiFiLib_BAUDS: 115200
 * WiFiLib_DEV: Serial in Arduino, Serial2 on STM32


## Example ##

Included on example folder, available on Arduino IDE.



## Who do I talk to? ##

 * [Naguissa](https://github.com/Naguissa)
 * http://www.naguissa.com
