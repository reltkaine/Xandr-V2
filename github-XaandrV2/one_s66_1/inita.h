//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
#include "SD.h"
#include "FS.h"

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
//static const uint8_t LED_BUILTIN = 22;///////////////////////

#include "SPIFFS.h"
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
//#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <string>
#include <cstddef>
#include <Wire.h>
#include <Preferences.h>
#include "soc/rtc_wdt.h"





#include <RadioLib.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
//#include <IRsend.h>
//#include <IRrecv.h>
//#include <IRremoteESP8266.h>
//#include <IRutils.h>
//#include <IRremote.hpp>


Preferences preferences;

//evil portal
String html_content = "N";
//html_content.clear();
bool run_evilportal_start;
int run_evilportal_html;
char dns_evil_portal[] = { "DNSCaptivePortal" };



//U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
PCF8574 pcf8574(0x20);

bool wireless_DS = 0;  //////wirelesss duck script
int newline;
#define fontName u8g2_font_6x12_tr  //u8g2_font_7x13_mf
#define fontX 6
#define fontY 12
int fontw = 6;
int fonth = 12;



int button_up_clicked = 0;     // only perform action when button is clicked, and wait until another press
int button_a_clicked = 0;      // same as above
int button_b_clicked = 0;      // same as above
int button_x_clicked = 0;      // same as above
int button_y_clicked = 0;      // same as above
int button_down_clicked = 0;   // same as above
int button_left_clicked = 0;   // only perform action when button is clicked, and wait until another press
int button_right_clicked = 0;  // only perform action when button is clicked, and wait until another press


#define up_button 251     //p2
#define down_button 253   //p1
#define left_button 191   //p6
#define right_button 223  //p5
#define b_button 239      //p4
#define y_button 127      //p7
#define a_button 247      //p3
#define x_button 254      //p0



// pins for buttons:
#define BTN_SEL 4//3  //A Select button
#define BTN_A 4//3    //A Select button

#define BTN_UP 0//2    // Up
#define BTN_DOWN 1  // Down
#define BTN_ESC 6//0   ///X
#define BTN_X 6//0     ///X

#define BTN_LFT 2//6
#define BTN_RGT 3//5
#define BTN_B 5//4  ////B
#define BTN_Y 7  ////y
#define LED_PIN LED_BUILTIN

char FreqChar[] = "433.92";
char FreqStartChar[] = "432.99";
char FreqEndChar[] = "433.99";
char BitRateChar[] = "032.00";
char FreqDevChar[] = "047.60";
char RXBandChar[] = "812.50";
char OutPowChar[] = "10";
char PreambleLenChar[] = "32";

char NRFChannelStart[] = "000";
char NRFChannelEnd[] = "125";




//#define uint8_t byte
#ifdef CONFIG_IDF_TARGET_ESP32S2


#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"


USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;


char bufADDd[] = "00001";
char bufPiped[] = "1";

char bufADDp[] = "00002";
char bufPipep[] = "2";
char text[] = "Hello World S2";  /////text
#else

char bufADDd[] = "00002";
char bufPiped[] = "2";

char bufADDp[] = "00001";
char bufPipep[] = "1";
char text[] = "Hello World Lite";
#endif
uint8_t NRF24Add_d[6];   //={"00001"};
uint8_t NRF24Pipe_d[1];  //={'1'};
uint8_t NRF24Pipe_X;

uint8_t NRF24Add_p[6];   ///= {"00002"};
uint8_t NRF24Pipe_p[1];  //={'2'};
uint8_t NRF24Pipe_Xp;


#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

//static const uint8_t LED_BUILTIN = 15;
//SD CARD-------------------------------FFFFFFFFF
#define PIN_SPI_SD_CS 6//-1
#define PIN_SPI_SD_MOSI 13
#define PIN_SPI_SD_MISO 14
#define PIN_SPI_SD_CLK 16


byte sck = 7;    //  GPIO 7
byte miso = 9;   //  GPIO 9
byte mosi = 11;  // GPIO 11
int gdo0 = 4;    // GPIO 4
int gdo2 = 5;    // GPIO 5
int cc1101_CS = 3;





SPIClass sd_spi = SPIClass(HSPI);  //SPIClass sd_spi = SPIClass(FSPI);
//RF24 radio(1, 2);
#define CE 1
#define CSN 2
RF24 radio(CE, CSN);
CC1101 radio_c = new Module(3, 4, RADIOLIB_NC, 5);

#define IR_RECEIVE_PIN 10
#define IR_SEND_PIN 8

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

//static const uint8_t LED_BUILTIN = 22;
//#define USE_NIMBLE NimBLECharacteristic.h
#include <BleKeyboard.h>

BleKeyboard Keyboard("BLE XandR", "No Body", 100);


//SD CARD-------------------------------
#define PIN_SPI_SD_CS 25//-1
#define PIN_SPI_SD_MOSI 2//13
#define PIN_SPI_SD_MISO 12
#define PIN_SPI_SD_CLK 14

SPIClass sd_spi = SPIClass(HSPI);


byte sck = 18;   //  GPIO 18
byte miso = 19;  //  GPIO 19
byte mosi = 23;  // GPIO
int gdo0 = 13;//2;    // GPIO 2
int gdo2 = 35;//4;    // GPIO 4
int cc1101_CS = 5;




//RF24 radio(22, 15);
#define CE 22//32//22
#define CSN 15//33//15
RF24 radio(CE, CSN);

CC1101 radio_c = new Module(cc1101_CS, gdo0, RADIOLIB_NC, gdo2);
#define IR_RECEIVE_PIN 34//16//27
#define IR_SEND_PIN 26
#endif



////cc1101  scanner
float start_freq = 432;    //start scan frequency set.
float stop_freq = 434;     //stop scan frequency set.
int rssi_threshold = -75;  // set the threshold to react. the value should be between -20 and -90. recommended -75.

float freq = start_freq;
long compare_freq;
float mark_freq;
int rssi;
int mark_rssi = -100;

int modul_mode = 2;


void pin_reset() {
#ifdef CONFIG_IDF_TARGET_ESP32S2
pinMode(LED_BUILTIN, OUTPUT);
digitalWrite(LED_BUILTIN, LOW);

  //sd mosi 13 miso 12 sck 14 tx 26 rx 27
  //rfid 25 cs
  pinMode(6, OUTPUT);
  digitalWrite(6, 1);
  //CC1101  5 cs 2 gdo0 4 gdo2
  pinMode(cc1101_CS, OUTPUT);
  digitalWrite(cc1101_CS, 1);
  //RF24 15 cs, 22 ce
  pinMode(CSN, OUTPUT);
  digitalWrite(CSN, 0);

  digitalWrite(CSN, 1);
  //RF24 radio(1, 2);
#else
pinMode(LED_BUILTIN, OUTPUT);
digitalWrite(LED_BUILTIN, LOW);

  //-------------lite
  //rfid 25 cs
  pinMode(25, OUTPUT);
  digitalWrite(25, 1);
  //CC1101  5 cs 2 gdo0 4 gdo2
  pinMode(cc1101_CS, OUTPUT);
  digitalWrite(cc1101_CS, 1);
  //RF24 15 cs, 22 ce
  pinMode(CSN, OUTPUT);
  digitalWrite(CSN, 0);
  digitalWrite(CSN, 1);
  // RF24 radio(22, 15);
#endif
}
