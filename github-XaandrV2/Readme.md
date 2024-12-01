# XandR

9/11/2023-UPDATE SKETCH AVAILABLE + EVIL PORTAL .....

https://creativecommons.org/licenses/by-nc-sa/4.0/


Diy Project(old version)
https://www.hackster.io/ruvic101/xandr-arduino-esp32-multi-tools-87774b

PCBWAY
https://www.pcbway.com/project/shareproject/Xandr_v2_Pentesting_Tool_29349d63.html


<a href="https://www.pcbway.com/project/shareproject/Xandr_v2_Pentesting_Tool_29349d63.html"><img src="https://www.pcbway.com/project/img/images/frompcbway-1220.png" alt="PCB from PCBWay" /></a>

Inside "Xandr Files" Folder copy "html" folder to SDCard this is for Captive Portal
Sample for BADUSB script also available like "string.txt"


check the picture for some Build Guide.....


Details for library needed check file "libraryinclude.txt" for the list


Install both Arduino 1.8 and Arduino 2 IDE

Install esp32 by Espressif System 2.0.17 not version 3.......


=>need to add in this file: C:\Users\\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.17\boards.txt
=>add this line somewhere in line 7098 or so

lolin32-lite.menu.PartitionScheme.huge_app=Huge APP (3MB No OTA/1MB SPIFFS)
lolin32-lite.menu.PartitionScheme.huge_app.build.partitions=huge_app
lolin32-lite.menu.PartitionScheme.huge_app.upload.maximum_size=3145728

=>to maximize the flash to fit the sketch......


Program using Arduino and Arduino Librar 

https://github.com/RobTillaart/PCF8575
https://github.com/me-no-dev/AsyncTCP

To compile or burn for Wemos Lite used Arduino 1.8 the added script is not being read by Arduino 2
 Board:WEMOS LOLIN32 LITE 
 Partition Sheme: Huge APP 

To compile or burn for Lolin S2 either used Arduino 1.8 or Arduino 2
 Board:LOLIN S2 Mini
 Partition Sheme: Huge APP


Hardware Specification

ESP32 S2 MINI/ESP32 LOLIN LITE

NRF24L01

CC1101

IR Reciever&Transmitter

OLED SH1106

PCF8574

SD Card

*PN522/PN532 (not yet implemented)

8 Tact Switch >>>recommended much more responsive(all palstic cover no metal cover)>>>https://www.amazon.in/Generic-Switch-Momentary-Tactile-Waterproof/dp/B07KJCCY99


	|UP|
LEFT|		|RIGHT
	|DOWN|

X|	|A|
	|B|
	|C|
X-Close/Exit
A-Enter/Select

IR Emitter DIODE

IR Reciever DIODE

10K OHM R1-R3-9-10-11-12-13-14-15

1300 OHM R4

410 OHM R5

200 OHM R2

47 uF

BC547	TRANSISTOR

NRF24-MENU - for sending and recieving message thru NRF24 ESP32 and ESP32S2 are already paired for testing
-sending message
	Press A open Virtual Key/Select charater
	B to delete character
	C to change/shifth to other group of character
	X to send message and exit to Virtual Key to recieve message
-recievibg message
	Stay at "COM NRF24"
******when exiting just press X

CC1101-MENU - for sending and recieving message thru CC1101 ESP32 and ESP32S2 are already paired for testing
-sending message
	Press A open Virtual Key/Select charater
	B to delete character
	C to change/shifth to other group of character
	X to send message and exit to Virtual Key to recieve message
-recievibg message
	Stay at "COM CC1101"
******when exiting just press X

WIFI-MENU - connecting to WIFI only(for testing WIFI Connection)

SubGhzMenu - Scanning specific SubGhz Frequency ranges using CC1101

2.4GhzMenu - Scanning channel using NRF24(need more tweak)

	    While for ESP32 it will use Bluetooth Keyboard(Bluetooth unstable connection).....

USB-Keyboard - connecting directly as Keyboard(ESP32S2)/BluetoothKeyboard(ESP32) and send key stroke "helllo"(for Testing)

WifiPacket - Spacehun Packet sniffer but config to only record handshake to SD Card

IR Remote - Scanning and Replicate IR signal(Unstable need more works)

  *A send Recieve any IR message (General to Protocol constraint)
  
  *B send NECRAW Recieve any IR message


BadUsb - Send Ducky Script thru Keyboard(ESP32S2)/BluetoothKeyboard(ESP32) from SDCard....

Evil Portal 

	    - Evil Portal Setup were it can sniff and save entry username/email and  password to SDCard once enter...

	    - portal ard readymade source out from "https://github.com/bigbrodude6119/flipper-zero-evil-portal/tree/main/portals"
     
     	    - copy the folder "html" to SDCARD user/password are save with an extension of ".evp"...





+++++++This is just the current form of this project need more work to be done and testing

******If you want to support project you can send donation directly..........

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.me/ruvics/2usd)

+++++++Initial Design for PCB Available here;;;

<a href="https://www.pcbway.com/project/shareproject/Xandr_v2_Pentesting_Tool_29349d63.html"><img src="https://www.pcbway.com/project/img/images/frompcbway-1220.png" alt="PCB from PCBWay" /></a>







/////

This is a human-readable summary of (and not a substitute for) the license. Disclaimer.
You are free to:

    Share — copy and redistribute the material in any medium or format
    Adapt — remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

Under the following terms:

    Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

    NonCommercial — You may not use the material for commercial purposes.

    ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.

    No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

Notices:

    You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
    No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.

