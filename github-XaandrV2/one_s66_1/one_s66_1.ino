//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "printf.h"
#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>
#include "WiFi.h"
#include <AsyncTCP.h>
#include <SPIFFS.h>
//#include <AsyncTCP.h>
#include <DNSServer.h>
//#include <ESPAsyncWebServer.h>
#include <esp_wifi.h>  //Used for mpdu_rx_disable android workaround
#include <ESPmDNS.h>

#include <SensorReceiver.h>
#include <SensorTransmitter.h>
#include <NewRemoteReceiver.h>
#include <NewRemoteTransmitter.h>

#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#define SDA 33
#define SCL 35

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

#define SDA 33  //19;//standard
#define SCL 32  //23;//standard
#endif

#include <menu.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

#include <menuIO/PCF8574KeyIn.h>
//#include <menuIO/altKeyIn.h>
#include <menuIO/KeyIn.h>

#include <U8g2lib.h>
//#include <Adafruit_GFX.h>
//#include <menuIO/adafruitGfxOut.h>
#include "RF24.h"
#include <RadioLib.h>
#include <SPI.h>
#include "inita.h"

#include "keyin.h"
#include "keyboard1.h"
#include "keyboard_mouse_h.h"

#include "nimble_h.h"
#include "nrf24_h.h"
#include "nrf_ble_jammers_h.h"
#include "nrfbox_h.h"

#include "sdcard_h.h"
#include "cc11_h.h"
#include "cc1101_elec.h"
#include "cc1101_subanal.h"
#include "IR_h2.h"
#include "ir_tvbgone.h"
#include "wifi_h.h"
#include "Buffer.h"
#include "wifi_packet_h.h"
#include "wifi_evilportal4.h"

#include <Preferences.h>



using namespace Menu;


#define LEDPIN LED_BUILTIN
#define SOFT_DEBOUNCE_MS 100
#define MAX_DEPTH 3

// pins for buttons:
#define BTN_SEL 4  //3  //A Select button
#define BTN_A 4    //3    //A Select button

#define BTN_UP 0    //2    // Up
#define BTN_DOWN 1  // Down
#define BTN_ESC 6   //0   ///X
#define BTN_X 6     //0     ///X

#define BTN_LFT 2  //6
#define BTN_RGT 3  //5
#define BTN_B 5    //4  ////B
#define BTN_Y 7    ////y

#define fontName u8g2_font_6x12_tr  //u8g2_font_7x13_mf
#define fontX 6
#define fontY 12
#define offsetX 0
#define offsetY 2
#define U8_Width 128
#define U8_Height 64
#define USE_HWI2C

int haltmenu = 0;



// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[6] MEMMODE = {
  { { 0, 0 }, { 0, 1, 1 } },  //bgColor
  { { 1, 1 }, { 1, 0, 0 } },  //fgColor
  { { 1, 1 }, { 1, 0, 0 } },  //valColor
  { { 1, 1 }, { 1, 0, 0 } },  //unitColor
  { { 0, 1 }, { 0, 0, 1 } },  //cursorColor
  { { 1, 1 }, { 1, 0, 0 } },  //titleColor
};


int chooseTest = -1;
int ledCtrl = HIGH;
bool btle = LOW;
result myLedOn() {
  pinMode(LEDPIN, OUTPUT);
  ledCtrl = LOW;
  digitalWrite(LEDPIN, ledCtrl);
  return proceed;
}
result myLedOff() {
  pinMode(LEDPIN, OUTPUT);
  ledCtrl = HIGH;
  digitalWrite(LEDPIN, ledCtrl);
  return proceed;
}

CHOOSE(ir_reginal, ir_reginal_chooseMenu, "Choose", doNothing, noEvent, noStyle,
       VALUE("NA", 1, doNothing, noEvent),

       VALUE("EU", 0, doNothing, noEvent));
CHOOSE(chooseTest, chooseMenu, "Choose", doNothing, noEvent, noStyle,
       VALUE("First", 1, doNothing, noEvent),
       VALUE("Second", 2, doNothing, noEvent),
       VALUE("Third", 3, doNothing, noEvent),
       VALUE("Last", -1, doNothing, noEvent));

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, wrapStyle  //,doExit,enterEvent,noStyle
       ,
       VALUE("On", HIGH, doNothing, noEvent),
       VALUE("Off", LOW, doNothing, noEvent));

TOGGLE(btle, setBTLE, "BTLE: ", doNothing, noEvent, wrapStyle  //,doExit,enterEvent,noStyle
       ,
       VALUE("On", HIGH, doNothing, noEvent),
       VALUE("Off", LOW, doNothing, noEvent));





const char* constMEM Digit MEMMODE = "0123456789";
const char* constMEM DigitPipe MEMMODE = "012345";
const char* constMEM Nr[] MEMMODE = { Digit, Digit, Digit, Digit, Digit, Digit };
const char* constMEM NrPipe[] MEMMODE = { DigitPipe };

const char* constMEM NrChannelScanMax[] MEMMODE = { Digit, Digit, Digit };
const char* constMEM NrChannelScanMin[] MEMMODE = { Digit, Digit, Digit };



const char* constMEM Cc[] MEMMODE = { Digit, Digit, Digit, ".", Digit, Digit };
const char* constMEM BR1[] MEMMODE = { Digit, Digit, ".", Digit };
const char* constMEM AR[] MEMMODE = { Digit, Digit };



SELECT(modul_mode, modul_mode_Menu, "Modulation", doNothing, noEvent, noStyle, VALUE("2-FSK", 0, doNothing, noEvent), VALUE("GFSK", 1, doNothing, noEvent), VALUE("ASK/OOK", 2, doNothing, noEvent), VALUE("4-FSK", 3, doNothing, noEvent), VALUE("MSK", 4, doNothing, noEvent)

      );

MENU(CC1101Menu, "CC1101-Menu", doNothing, anyEvent, noStyle,
     OP("Message", CC1101_action, enterEvent),
     EDIT("Frequency", FreqChar, Cc, doNothing, anyEvent, noStyle),
     EDIT("Bit Rate", BitRateChar, Cc, doNothing, anyEvent, noStyle),
     EDIT("Freq Deviation", FreqDevChar, Cc, doNothing, anyEvent, noStyle),
     EDIT("Rx Bandwidth", RXBandChar, Cc, doNothing, anyEvent, noStyle),
     EDIT("Output Power", OutPowChar, AR, doNothing, anyEvent, noStyle),
     EDIT("Preamble Length", PreambleLenChar, AR, doNothing, anyEvent, noStyle),
     SUBMENU(modul_mode_Menu),

     EXIT("<Back"));
//     OP("SHOWRAW", Cc1101_showraw_action_2, enterEvent),
MENU(SubGhzMenu, "SubGhzMenu", doNothing, anyEvent, noStyle,
     OP("SubGhz Anaylzer", Cc1101_scan_action, enterEvent),
     OP("SubGhz Anaylzer2", Cc1101_scan_action_2, enterEvent),
     OP("Replay Attack 1", Cc1101_recraw_action_3, enterEvent),

     OP("Replay Attack 2", Cc1101_recraw_action_2, enterEvent),


     SUBMENU(modul_mode_Menu),
     EDIT("Freq_Start", FreqStartChar, Cc, doNothing, anyEvent, noStyle),
     EDIT("Freq_End", FreqEndChar, Cc, doNothing, anyEvent, noStyle),



     EXIT("<Back"));
//OP("ReTrans", Cc1101_ReTrans, enterEvent),
MENU(Bluetooth, "Bluetooth", doNothing, anyEvent, noStyle,
     OP("Apple Juice", AppleJuice, enterEvent),
     OP("Windows Sour", Windows_Sour, enterEvent),
     OP("Android Sour", Android_Sour, enterEvent),
     OP("Samsung Sour", Samsung_Sour, enterEvent),
     OP("All Sour", All_Sour, enterEvent),
     OP("BLE_SCANNER", BLE_SCANNER, enterEvent),
     OP("BLE_BEACON_SCANNER", BLE_BEACON_SCANNER, enterEvent),
     EXIT("<Back"));




MENU(nrf24Menu, "NRF24-Menu", doNothing, anyEvent, noStyle,
     OP("Message", NRF24L01_action, enterEvent),
     EDIT("Device Address", bufADDd, Nr, doNothing, anyEvent, noStyle),
     EDIT("Paired Address", bufADDp, Nr, doNothing, anyEvent, noStyle),

     EXIT("<Back"));



MENU(WifiMenu, "WIFI-Menu", doNothing, anyEvent, noStyle,
     OP("WIFI", wifi_action, enterEvent),
     EXIT("<Back"));

MENU(Nrf24_Scanner, "2.4Ghz Scanner", doNothing, anyEvent, noStyle,

     OP("2.4Ghz Scanner", NRF24L01_scanner, enterEvent),
     OP("2.4Ghz Analyzer", NRF24L01_analyzer, enterEvent),

     EXIT("<Back"));
//     EDIT("Channel_Start", NRFChannelStart, NrChannelScanMax, doNothing, anyEvent, noStyle),
//     EDIT("Channel_End", NRFChannelEnd, NrChannelScanMin, doNothing, anyEvent, noStyle),

MENU(BLE_NRF24_JAMMER, "NRF24 ATTACK", doNothing, anyEvent, noStyle,

     OP("NRF24 BLE JAMMER", NRF24_BLEJammer, enterEvent),



     OP("Nrf24 Jammer 2", Nrf24_Jammer1, enterEvent),


     EXIT("<Back"));


MENU(Evil_PortalMenu, "Evil_Portal_Menu", doNothing, anyEvent, noStyle,

     OP("Evil_Portal", Evil_Portal, enterEvent),
     OP("Evil_Portal_Name", EVIL_PORTAL_NAME, enterEvent),

     EXIT("<Back"));

MENU(Infrared_func, "Infrared", doNothing, anyEvent, noStyle,

     OP("IR Remote", IR_action, enterEvent),
     OP("IRTvBGone", IR_tvb_gone_action, enterEvent),
     SUBMENU(ir_reginal_chooseMenu),
     EXIT("<Back"));



MENU(mainMenu, "XANDR-MENU-RUVIC101", doNothing, noEvent, wrapStyle,
     SUBMENU(nrf24Menu),
     SUBMENU(CC1101Menu),
     SUBMENU(WifiMenu),
     SUBMENU(Bluetooth),
     SUBMENU(SubGhzMenu),
     SUBMENU(Nrf24_Scanner),
     SUBMENU(BLE_NRF24_JAMMER),
     SUBMENU(Evil_PortalMenu),
     SUBMENU(Infrared_func),
     OP("WifiPacket", WIFIPACKET_action, enterEvent),
     OP("BADUSB", BADUSB_action, enterEvent),

     SUBMENU(setLed),

     EXIT("<Back"));
//SUBMENU(chooseMenu),
//     OP("NRF24_USB", NRF24_USB_action, enterEvent),// experimental

///kklklklk=======OP("USB-KEYBOARD", KEYBOARD_action, enterEvent),

//SUBMENU(setBTLE),

MENU_OUTPUTS(out, MAX_DEPTH, SERIAL_OUT(Serial), U8G2_OUT(display, colors, fontX, fontY, offsetX, offsetY, { 0, 0, U8_Width / fontX, U8_Height / fontY }) /* x_offset, y_offset, x_width, y_width*/
            );


/*     OP("MouseJack", mj_action, enterEvent),*/

// define button list and functions:
keyMap btn_map[] = {
  { BTN_SEL, defaultNavCodes[enterCmd].ch },
  { BTN_UP, defaultNavCodes[upCmd].ch },
  { BTN_DOWN, defaultNavCodes[downCmd].ch },
  { BTN_ESC, defaultNavCodes[escCmd].ch },
  { BTN_LFT, defaultNavCodes[leftCmd].ch },
  { BTN_RGT, defaultNavCodes[rightCmd].ch }
};
PCF8574KeyIn<6> btns(btn_map, Wire);  // 3 buttons

//#define BTN_SELB   12// Select button
//#define BTN_UPB 4 // Down

//keyMap joystickBtn_map[]={
// {BTN_SELB, defaultNavCodes[enterCmd].ch,INPUT_PULLUP},
// {BTN_UPB, defaultNavCodes[upCmd].ch,INPUT_PULLUP}
// };
//keyIn<2> joystickBtns(joystickBtn_map);


serialIn serial(Serial);
menuIn* inputsList[] = { &serial, &btns };  //,&joystickBtns
chainStream<2> in(inputsList);              //2 is the number of inputs

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);



result BLE_BEACON_SCANNER(idleEvent e) {
  nav.idleOn();
  mainMenu[0].disable();  ///wala
  //bool n_exit = 1;
  //  bool a_exit = 1;
  ble_beacon_scanner_init();
  // while (n_exit) {
  //    if (readButtonStateY(BTN_Y)) {
  //     n_exit = 0;
  //   }


  ble_beacon_scanner();
  //  }
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
#endif
haltmenu = 0;
mainMenu[0].enable();
nav.idleOff();
return proceed;
}




  result BLE_SCANNER(idleEvent e) {
    nav.idleOn();
    mainMenu[0].disable();  ///wala
    ble_scanner_init();
    ble_scanner();
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
#endif
haltmenu = 0;
mainMenu[0].enable();
nav.idleOff();
return proceed;
}
//==============================================
result All_Sour(idleEvent e) {
nav.idleOn();
mainMenu[0].disable();  ///wala
bool n_exit = 1;

    init_nim();  //xxx
    while (n_exit) {
      BTLE_JAM_nim();  //xx
      if (readButtonStateY(BTN_Y)) {
        n_exit = 0;
      }
    }
    destroy();
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
#endif

    haltmenu = 0;
    mainMenu[0].enable();
    nav.idleOff();
    return proceed;
  }



  result Samsung_Sour(idleEvent e) {
    nav.idleOn();
    mainMenu[0].disable();  ///wala
    bool n_exit = 1;

    init_nim();  //xxx
    while (n_exit) {
      advertiseSamsung();  //xx
      if (readButtonStateY(BTN_Y)) {
        n_exit = 0;
      }
    }
    destroy();
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
#endif

    haltmenu = 0;
    mainMenu[0].enable();
    nav.idleOff();
    return proceed;
  }

  result Windows_Sour(idleEvent e) {
    nav.idleOn();
    mainMenu[0].disable();  ///wala
    bool n_exit = 1;

    init_nim();  //xxx
    while (n_exit) {
      advertiseMicrosoft();  //xx
      if (readButtonStateY(BTN_Y)) {
        n_exit = 0;
      }
    }
    destroy();
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
#endif

    haltmenu = 0;
    mainMenu[0].enable();
    nav.idleOff();
    return proceed;
  }

  result Android_Sour(idleEvent e) {
    nav.idleOn();
    mainMenu[0].disable();  ///wala
    bool nrf24_exit = 1;
    //nrf24_i();
    //nrf24_jam_init();
    // nrf24_jam_start();
    //aj_adv(1);
    init_nim();
    while (nrf24_exit) {
      advertiseAndroid();

      //executeSpam(Google);
      if (readButtonStateY(BTN_Y)) {
        nrf24_exit = 0;
      }
    }


#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
#endif
haltmenu = 0;
mainMenu[0].enable();
nav.idleOff();
return proceed;
}



    result AppleJuice(idleEvent e) {
      nav.idleOn();
      mainMenu[0].disable();  ///wala

      bool nrf24_exit = 1;

      // spooferSetup();
      init_nim();
      while (nrf24_exit) {
        //          executeSpam(Apple);
        advertiseAndroid();

        if (readButtonStateY(BTN_Y)) {
          nrf24_exit = 0;
        }
      }




#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

      esp_bt_controller_deinit();
      esp_wifi_stop();
      esp_wifi_deinit();
#endif
haltmenu = 0;
mainMenu[0].enable();
nav.idleOff();
return proceed;
}


      result EVIL_PORTAL_NAME(idleEvent e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala
        bool n_exit = 1;
        //  bool a_exit = 1;
        while (n_exit) {
          if (readButtonStateY(BTN_Y)) {
            n_exit = 0;
          }

          run_key();
          n_exit = 0;
        }
        strcpy(dns_evil_portal, Keybuffer);
        //dns_evil_portal = &Keybuffer;


        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();
        return proceed;
      }



      result Evil_Portal(idleEvent e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala
        shutwifi();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "Evil Portal");
        display.sendBuffer();  // transfer internal memory to the display

        digitalWrite(LEDPIN, 1);

        sdcard_r();

        delay(100);

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "SD Start");
        display.drawStr(25, 24, "LOADING FILES");

        display.sendBuffer();  // transfer internal memory to the display
        sdcard_s_ex(".html", "html");

        esp_event_loop_init(event_handler, NULL);  ///initialized esp event loop.....
        int sd_exit = 1;
        while (sd_exit) {
          if (readButtonStateY(BTN_Y)) {
            sd_exit = 0;
          }

          while (sdcard_sel_Folder("html")) {

            read_string_file(SD, fileo);
            //     run_payload(SD, p);
            if (html_content != "N") {
              digitalWrite(LEDPIN, 0);
              evil_portal_start();
              html_content = "N";
              display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
              display.drawStr(25, 15, "STOP EVIL PORTAL");
              display.sendBuffer();  // transfer internal memory to the display
              delay(100);
              sd_exit = 0;
            }
          }
        }








        shutwifi();

        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();
        return proceed;
      }



      result Nrf24_Jammer1(idleEvent e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala
        bool nrf24_exit = 1;
        nrf24_i();
        jammerSetup();
        while (nrf24_exit) {

          // nrf24_jam_start();
          if (readButtonStateY(BTN_Y)) {
            nrf24_exit = 0;
          }
          jammerLoop();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();
        return proceed;
      }




      /* Experimental  sending payload tru NRF24 to ble usb / usb
                    result NRF24_USB_action(idleEvent e) {


                    nav.idleOn();
                    mainMenu[0].disable();  ///wala
                    nrf24_i();
                    keyboard_mouse_i();

                    int nrf24_exit = 1;
                    int nrf24_ter = 0;
                    while (nrf24_exit) {
                      display.firstPage();
                      do {

                        if (readButtonStateY(BTN_Y)) {
                          nrf24_exit = 0;
                        }
                        if (readButtonStateY(BTN_SEL)) {
                          nrf24_ter = 1;
                        }



                        if (nrf24_r()) {

                          if (textbb[0] == 'D' && textbb[1] == 'S') {
                            Serial.println("DUCKY SCRIPT START");
                            textbb[0] = '\0';
                            process_wireless_rec();
                            Serial.println("DUCKY SCRIPT END");
                          }


                          if (textbb[0] != '\0') {
                            kyboard_print_r(String(textbb));
                            textbb[0] = '\0';
                          }
                        }



                        //nrf24_c(text);
                        while (nrf24_ter) {
                          run_key();
                          nrf24_c(Keybuffer);
                          nrf24_ter = 0;
                          //Keybuffer[] = "/0";
                        }



                      } while (display.nextPage());
                    }
                    keyboard_mouse_u();

                    haltmenu = 0;
                    mainMenu[0].enable();
                    nav.idleOff();

                    return proceed;
                    }
      */

      result KEYBOARD_action(eventMask e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala

        keyboard_mouse_i();
        delay(200);

        int keyboard_exit = 1;
        while (keyboard_exit) {
          if (readButtonStateY(BTN_SEL)) {
            kyboard_print_r("helo typemaster");
          }
          if (readButtonStateY(BTN_Y)) {
            keyboard_exit = 0;
          }
        }
        keyboard_mouse_u();



        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();



        return proceed;
      }




      result WIFIPACKET_action(idleEvent e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala
        //wifi_stop();
        shutwifi();
        sdcard_r();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "WIFI Start");
        display.sendBuffer();  // transfer internal memory to the display
        restartwifi();
        wifi_i();
        wifi_scan();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.sendBuffer();   // transfer internal memory to the display

        wifi_packet_init();



        setChannel(wifi_ch);
        delay(10);



        coreTask();
        /*
                      while (wifi_packet_exit) {
                      Serial.println("hellow");


                      if (readButtonStateY(BTN_Y)) {
                        Serial.println("Killing the wifi");
                        sdBuffer.close(&SD);
                        esp_wifi_set_promiscuous(false);
                        esp_wifi_stop();
                        esp_wifi_deinit();
                        wifi_packet_exit = 0;
                      }
                      vTaskDelay(portMAX_DELAY);


                      // vTaskDelay(500 / portTICK_PERIOD_MS);
                      // wifi_packet_exit=wifi_packet_run();
                      }
        */
        //wifi_stop();
        //  esp_wifi_stop();

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "Packet Sniff----STOP");
        display.sendBuffer();  // transfer internal memory to the display

        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }

      result BADUSB_action(eventMask e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala

        //nrf24_i();


        sdcard_r();

        delay(100);

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "SD Start");
        display.drawStr(25, 24, "LOADING FILES");

        display.sendBuffer();  // transfer internal memory to the display
        sdcard_s();
        //init_sd_display();
        int sd_exit = 1;
        while (sd_exit) {
          //  char* p;
          //char* p=nullptr;
          if (readButtonStateY(BTN_Y)) {
            sd_exit = 0;
          }
          while (sdcard_sel()) {
            run_payload(SD, fileo);

            // run_ducky_payload(SD, fileo);
            //     run_payload(SD, p);
          }

          // while (loop_sd_display()) {
        }

        //listDir(SD, "/", 0);
        //run_payload(SD,"/payload.txt");
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result wifi_action(idleEvent e) {
        nav.idleOn();
        mainMenu[0].disable();  ///wala
        //wifi_stop();
        shutwifi();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "WIFI Start");
        display.sendBuffer();  // transfer internal memory to the display
        restartwifi();
        wifi_i();
        wifi_scan();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "Enter_Password");
        display.sendBuffer();  // transfer internal memory to the display
        delay(100);
        run_key();
        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "Wifi Connecting!!");
        display.sendBuffer();  // transfer internal memory to the display
        wifi_connect(strtok(Keybuffer, ""));


        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }
      /*
                    result mj_action(idleEvent e) {

                    nav.idleOn();
                    mainMenu[0].disable();  ///wala
                    mj_i();
                    int mj_exit = 1;
                    while (mj_exit) {

                      if (readButtonStateY(BTN_Y)) {
                        mj_exit = 0;
                      }
                      mj_r();
                    }
                    haltmenu = 0;
                    mainMenu[0].enable();
                    nav.idleOff();

                    return proceed;
                    }

      */






      result Cc1101_scan_action(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        //  cc1101_scan_init();

        //initprotocol();
        int Cc1101_scan_exit = 1;
        start_c1101();
        while (Cc1101_scan_exit) {

          if (readButtonStateY(BTN_Y)) {
            Cc1101_scan_exit = 0;
          }

          //    cc1101_scan();float start_ma = start_freq;

          //analyze_cc1101(start_freq);
          sub_anal();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }



      result Cc1101_scan_action_2(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        //  cc1101_scan_init();

        //initprotocol();
        int Cc1101_scan_exit = 1;
        cc1101initialize2();
        cc1101_scan_elec();
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result Cc1101_recraw_action_2(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        cc1101initialize2();
        rpattackSetup();
        int Cc1101_scan_exit = 1;

        while (Cc1101_scan_exit) {

          if (readButtonStateY(BTN_Y)) {
            Cc1101_scan_exit = 0;
          }
          rpattackLoop();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result Cc1101_recraw_action_3(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        cc1101initialize2();
        cc1101_scan_raw();
        //cc1101_recraw();

        //cc1101_rxraw();
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result Cc1101_ReTrans(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        cc1101initialize2();


        int Cc1101_scan_exit = 1;
        //start_c1101();

        while (Cc1101_scan_exit) {

          if (readButtonStateY(BTN_Y)) {
            Cc1101_scan_exit = 0;
          }

          //    cc1101_scan();float start_ma = start_freq;

          //analyze_cc1101(start_freq);
          ReTrans_setup();
        }


        //cc1101_recraw();

        //cc1101_rxraw();
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }




      result Cc1101_showraw_action_2(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala

        int Cc1101_scan_exit = 1;
        while (Cc1101_scan_exit) {

          if (readButtonStateY(BTN_Y)) {
            Cc1101_scan_exit = 0;
          }

          cc1101_showraw();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }



      result IR_tvb_gone_action(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        ir_tvb_gone_setup();
        int IR_exit = 1;
        while (IR_exit) {
          display.firstPage();
          do {
            if (readButtonStateY(BTN_Y) || sendAllCodes() == 1) {
              IR_exit = 0;
            }

          } while (display.nextPage());
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result IR_action(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        IR_i();
        int IR_exit = 1;
        while (IR_exit) {
          display.firstPage();
          do {
            if (readButtonStateY(BTN_Y)) {
              IR_exit = 0;
            }
            IR_r();
          } while (display.nextPage());
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result CC1101_action(idleEvent e) {

        nav.idleOn();
        mainMenu[0].disable();  ///wala
        cc1101_i();
        int cc1101_exit = 1;
        int cc1101_ter = 0;
        int cc1101_interval = 0;



        while (cc1101_exit) {
          display.firstPage();
          do {

            if (readButtonStateY(BTN_Y)) {
              cc1101_exit = 0;
            }
            if (readButtonStateY(BTN_SEL)) {
              cc1101_ter = 1;
            }
            cc1101_r();
            //nrf24_c(text);
            while (cc1101_ter) {
              run_key();
              cc1101_c();
              cc1101_ter = 0;
              //Keybuffer[] = "/0";
            }
            int cc1101_beacon = 0;

            if (readButtonStateY(BTN_B)) {
              cc1101_beacon = 1;
            }
            //nrf beacon
            int cc1101_beacon_interval = 1;

            while (cc1101_beacon) {
              display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
              display.drawStr(25, 15, "Sending Beacon");
              display.sendBuffer();  // transfer internal memory to the display

              char spaceship[2] = "";
              spaceship[0] = ' ';
              char interv[2] = "";
              char beacon_buffer[20] = "";
              int len = 2;
              //String(nrf24_beacon_interval).length;
              //interv[0]=nrf24_beacon_interval++;
              /*
                             int b=9999;
                             int len=String(b).length;
                             char cStr[len+1];
                             itoa(b,cStr,10);
                              String(b).toCharArray(cStr,len);
              */
              //itoa(nrf24_beacon_interval++,interv,2);
              //strcat(Keybuffer, spaceship);
              //strcat(spaceship, interv);
              String(cc1101_beacon_interval).toCharArray(interv, len);
              //itoa(nrf24_beacon_interval,interv,2);
              strcat(beacon_buffer, Keybuffer);
              strcat(beacon_buffer, spaceship);
              strcat(beacon_buffer, interv);
              cc1101_beacon_interval++;

              int tapos = 1;

              while (tapos) {
                cc1101_string(String(beacon_buffer));
                delay(100);
                tapos = 0;
              }

              if (cc1101_interval <= 9) {
                cc1101_interval = 0;
              }

              if (readButtonStateY(BTN_Y)) {
                display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
                display.drawStr(25, 15, "STOP Beacon");
                display.sendBuffer();  // transfer internal memory to the display

                cc1101_beacon = 0;
              }

              // Serial.println(String(beacon_buffer));
            }

          } while (display.nextPage());
        }

        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }



      result NRF24_BLEJammer(idleEvent e) {


        nav.idleOn();
        mainMenu[0].disable();  ///wala

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "NRF24 BLE Jammer");
        display.sendBuffer();  // transfer internal memory to the display

        nrf24_i();
        reset_nrf();

        blejammerSetup();
        int sd_exit = 1;
        while (sd_exit) {
          if (readButtonStateY(BTN_Y)) {
            sd_exit = 0;
          }

          blejammerLoop();
        }

        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }

      result NRF24L01_analyzer(idleEvent e) {


        nav.idleOn();
        mainMenu[0].disable();  ///wala

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "NRF24 Analyzer");
        display.sendBuffer();  // transfer internal memory to the display

        /*
                      reset_nrf();

                      scanner_channel();
        */
        int sd_exit = 1;
        nrf24_i();
        analyzerSetup();
        //scannerSetup();
        while (sd_exit) {
          if (readButtonStateY(BTN_Y)) {
            sd_exit = 0;
          }

          analyzerLoop();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result NRF24L01_scanner(idleEvent e) {


        nav.idleOn();
        mainMenu[0].disable();  ///wala

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
        display.drawStr(25, 15, "NRF24 Scanner");
        display.sendBuffer();  // transfer internal memory to the display

        /*
                      reset_nrf();

                      scanner_channel();
        */
        int sd_exit = 1;
        nrf24_i();
        scannerSetup();
        while (sd_exit) {
          if (readButtonStateY(BTN_Y)) {
            sd_exit = 0;
          }

          scannerLoop();
        }
        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }


      result NRF24L01_action(idleEvent e) {


        nav.idleOn();
        mainMenu[0].disable();  ///wala
        nrf24_i();
        int nrf24_exit = 1;
        int nrf24_ter = 0;
        //int nrf24_interval = 0;


        while (nrf24_exit) {
          display.firstPage();
          do {

            if (readButtonStateY(BTN_Y)) {
              nrf24_exit = 0;
            }
            if (readButtonStateY(BTN_SEL)) {
              nrf24_ter = 1;
            }
            nrf24_r();
            //nrf24_c(text);
            while (nrf24_ter) {
              run_key();
              nrf24_c(Keybuffer);
              nrf24_ter = 0;
              //Keybuffer[] = "/0";
            }

            int nrf24_beacon = 0;

            if (readButtonStateY(BTN_B)) {
              nrf24_beacon = 1;
            }
            //nrf beacon
            int nrf24_beacon_interval = 1;

            while (nrf24_beacon) {
              display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
              display.drawStr(25, 15, "Sending Beacon");
              display.sendBuffer();  // transfer internal memory to the display

              char spaceship[2] = "";
              spaceship[0] = ' ';
              char interv[2] = "";
              char beacon_buffer[20] = "";
              int len = 2;
              //String(nrf24_beacon_interval).length;
              //interv[0]=nrf24_beacon_interval++;
              /*
                             int b=9999;
                             int len=String(b).length;
                             char cStr[len+1];
                             itoa(b,cStr,10);
                              String(b).toCharArray(cStr,len);
              */
              //itoa(nrf24_beacon_interval++,interv,2);
              //strcat(Keybuffer, spaceship);
              //strcat(spaceship, interv);
              String(nrf24_beacon_interval).toCharArray(interv, len);
              //itoa(nrf24_beacon_interval,interv,2);
              strcat(beacon_buffer, Keybuffer);
              strcat(beacon_buffer, spaceship);
              strcat(beacon_buffer, interv);
              nrf24_beacon_interval++;

              int tapos = 1;

              while (tapos) {
                nrf24_string(String(beacon_buffer));
                delay(100);
                tapos = 0;
              }

              if (nrf24_beacon_interval >= 9) {
                nrf24_beacon_interval = 0;
              }

              if (readButtonStateY(BTN_Y)) {
                display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
                display.drawStr(25, 15, "STOP Beacon");
                display.sendBuffer();  // transfer internal memory to the display

                nrf24_beacon = 0;
              }

              // Serial.println(String(beacon_buffer));
            }


          } while (display.nextPage());
        }

        haltmenu = 0;
        mainMenu[0].enable();
        nav.idleOff();

        return proceed;
      }



      void setup() {
        pinMode(LEDPIN, OUTPUT);
        //joystickBtns.begin();
        Wire.begin(SDA, SCL);
        Serial.begin(115200);
        display.begin();
        display.setFont(fontName);

        display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);1723237

        display.setCursor(25, 15);
        display.print(String(ESP.getChipModel()));
        display.print("-");
        display.print(String(ESP.getChipRevision()));
        display.setCursor(25, 24);
        display.print(String(__TIME__));
        display.setCursor(25, 34);
        display.print(String(__DATE__));


        display.sendBuffer();  // transfer internal memory to the display



        delay(1000);
        shutwifi();
        // while (!Serial)
        //   ;
        // nvs_flash_erase(); // erase the NVS partition and...
        //nvs_flash_init(); // initialize the NVS partition.
        preferences.begin("evil_portal", false);

        //

        run_evilportal_start = preferences.getBool("run_evilportal", 0);
        run_evilportal_html = preferences.getInt("html", 0);
        Serial.println("checking_evilportal_start");
        Serial.print("run_evilportal_start :");
        Serial.println(run_evilportal_start);

        Serial.print("run_evilportal_html :");
        Serial.println(run_evilportal_html);

        //while(!Serial);
        //shutwifi();

        //    esp_wifi_stop();
        //  esp_wifi_deinit();


        //ESP.restart();


        Serial.println("Menu test");
        // display.begin();
        // display.setFont(fontName);
        display.drawStr(0, 0, "Menu test");
        newline = display.getMaxCharHeight();
        delay(1000);
      }


      bool btl_power = 0;

      void loop() {


        nav.doInput();
        if (nav.changed(0)) {  //only draw if menu changed for gfx device
          //change checking leaves more time for other tasks
          display.firstPage();
          do nav.doOutput();
          while (display.nextPage());
        }

        digitalWrite(LEDPIN, ledCtrl);
        delay(100);
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2


#else
if (btle) {

        if (btl_power == 0) {

          btStart();
          btl_power = 1;
        }
      } else {

        if (btl_power == 1)

        {

          btStop();
          btl_power = 0;
        }
      }
#endif
///   }---------------------

        //  nav.poll();
      }
