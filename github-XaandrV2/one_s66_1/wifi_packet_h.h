/* uncomment if the default 4 bit mode doesn't work */
/* ------------------------------------------------ */
// #define BOARD_HAS_1BIT_SDMMC true // forces 1bit mode for SD MMC
/* ------------------------------------------------ */
using namespace std;

/* ===== compile settings ===== */
#define MAX_CH 14      // 1 - 14 channels (1-11 for US, 1-13 for EU and 1-14 for Japan)
#define SNAP_LEN 2324  // max len of each recieved packet

//#define BUTTON_PIN 5  // button to change the channel

//#define SDA_PIN 33
//#define SCL_PIN 35
#define MAX_X 128
#define MAX_Y 51



#include "Buffer.h"

//TaskHandle_t wifi_task_handle = NULL;
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //////////////////



esp_err_t event_handler(void* ctx, system_event_t* event) {
  return ESP_OK;
}

/* ===== run-time variables ===== */
Buffer sdBuffer;
///#ifdef USE_DISPLAY
//U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
//#define fontName u8g2_font_6x12_tr  //u8g2_font_7x13_mf

//SH1106 display(0x3c, SDA_PIN, SCL_PIN);
//#endif


//SD CARD-------------------------------
//#define PIN_SPI_SD_CS 25
//#define PIN_SPI_SD_MOSI 13
//#define PIN_SPI_SD_MISO 12
//#define PIN_SPI_SD_CLK 14

//SPIClass sd_spi = SPIClass(HSPI);


bool useSD = false;
bool buttonPressed = false;
bool buttonEnabled = true;
uint32_t lastDrawTime;
uint32_t lastButtonTime;
uint32_t tmpPacketCounter;
uint32_t pkts[MAX_X];  // here the packets per second will be saved
uint32_t deauths = 0;  // deauth frames per second
unsigned int ch = 10;  // current 802.11 channel
int rssiSum;

//---------------------------------------------------------------

#define DATA_LENGTH 112
#define TYPE_MANAGEMENT 0x00
#define TYPE_CONTROL 0x01
#define TYPE_DATA 0x02
#define SUBTYPE_PROBE_REQUEST 0x04
#define SUBTYPE_PROBE_RESPONSE 0x05
#define SUBTYPE_BEACONS 0x08


#define PKTS_BUF_SIZE 320  // buffer size for "packets/s" graph
#define MAX_SSIDs 20    //1792     // buffer cache size (*32bits) for Beacon information, reduce this in case of memory problems

int graph_RSSI = 1;
int delta = 1;
int grid = 0;
int tcount = 0;

/*
   Global variables for storing beacons and clients
*/


/*
   Data structure for beacon information
*/
struct ssid_info {
  uint8_t mac[6];
  uint8_t ssid[33];
  uint8_t ssid_len;
  bool ssid_eapol;  // to mark if we already have the eapol.
};

ssid_info ssid_known[MAX_SSIDs];
uint32_t ssid_count = 0;        //
uint32_t ssid_eapol_count = 0;  // eapol frames per second

int8_t last_rssi;
char last_ssid[33] = { '[', 'n', 'o', 'n', 'e', ']', '\0' };
char last_ssid_mac[18] = { 0 };
char last_eapol_ssid[33] = { '[', 'n', 'o', 'n', 'e', ']', '\0' };
char last_eapol_mac[18] = { 0 };

uint32_t eapol = 0;        // eapol frames per second
uint32_t total_eapol = 0;  // eapol total frames
uint32_t graph_eapol = 0;  // eapol total frames
/*
   Convert Ethernet address to standard hex-digits-and-colons printable form.
   Re-entrant version (GNU extensions).
   Inspired from https://android.googlesource.com/platform/bionic/+/master/libc/bionic/ether_ntoa.c
*/
char* ether_ntoa_r(const uint8_t* addr, char* buf) {
  snprintf(buf, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
           addr[0], addr[1],
           addr[2], addr[3],
           addr[4], addr[5]);
  return buf;
}

/*
   Convert Ethernet address to standard hex-digits-and-colons printable form.
   Inspired from https://android.googlesource.com/platform/bionic/+/master/libc/bionic/ether_ntoa.c
*/
char* ether_ntoa(const uint8_t* addr) {
  static char buf[18];
  return ether_ntoa_r(addr, buf);
}


static void setLastSSID(uint16_t start, uint16_t size, uint8_t* data) {
  int u = 0;
  for (uint16_t i = start; i < DATA_LENGTH && i < start + size; i++) {
    //Serial.write(data[i]);
    last_ssid[u] = data[i];
    u++;
  }
  last_ssid[u] = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/* ===== functions ===== */
double getMultiplicator() {
  uint32_t maxVal = 1;
  for (int i = 0; i < MAX_X; i++) {
    if (pkts[i] > maxVal) maxVal = pkts[i];
  }
  if (maxVal > MAX_Y) return (double)MAX_Y / (double)maxVal;
  else return 1;
}

void wifi_promiscuous(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  if (type == WIFI_PKT_MISC) return;  // wrong packet type
  if (ctrl.sig_len > 293) return;     // packet too long
  uint32_t packetLength = ctrl.sig_len;
  if (type == WIFI_PKT_MGMT) packetLength -= 4;
  // fix for known bug in the IDF
  // https://github.com/espressif/esp-idf/issues/886
  //Serial.print(".");
  tmpPacketCounter++;
  rssiSum += ctrl.rssi;
  unsigned int u;

  if (type == WIFI_PKT_MGMT && (pkt->payload[0] == 0xA0 || pkt->payload[0] == 0xC0)) {
    deauths++;
    //      if (useSD) sdBuffer.addPacket(pkt->payload, packetLength);
    // deauth
  }

  if (((pkt->payload[30] == 0x88 && pkt->payload[31] == 0x8e) || (pkt->payload[32] == 0x88 && pkt->payload[33] == 0x8e))) {
    eapol++;  // new eapol packets :)


    log_d("Got EAPOL ...");

    memcpy(&ssid_known[MAX_SSIDs - 1].mac, pkt->payload + 16, 6);  // MAC source HW address

    for (u = 0; u < ssid_count; u++) {
      if (!memcmp(ssid_known[u].mac, ssid_known[MAX_SSIDs - 1].mac, 6)) {
        // only if is new print it
        if (!ssid_known[u].ssid_eapol) {
          ssid_eapol_count++;
          ssid_known[u].ssid_eapol = true;
          for (int i = 0; i < ssid_known[u].ssid_len; i++) {
            last_eapol_ssid[i] = ssid_known[u].ssid[i];
          }
          last_eapol_ssid[ssid_known[u].ssid_len + 1] = 0;
          snprintf(last_eapol_mac, 18, "%s", ether_ntoa(ssid_known[u].mac));

          Serial.printf("[EAPOL] Found new MAC: %s (SSID: %s)\n",
                        ether_ntoa(ssid_known[u].mac),
                        last_eapol_ssid);
        }
        break;
      }
    }
    //  uint8_t SSID_length = pkt->payload[25];
    //  Serial.println(" SSID: ");
    //  setLastSSID(26, SSID_length, pkt->payload);
    if (useSD) {
      sdBuffer.addPacket(pkt->payload, packetLength);
    }
  }

  //  if (type != WIFI_PKT_MGMT )  return;
  // We add the packets only for prove

  unsigned int frameControl = ((unsigned int)pkt->payload[1] << 8) + pkt->payload[0];

  uint8_t version = (frameControl & 0b0000000000000011) >> 0;
  uint8_t frameType = (frameControl & 0b0000000000001100) >> 2;
  uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  // uint8_t toDS = (frameControl & 0b0000000100000000) >> 8;
  // uint8_t fromDS = (frameControl & 0b0000001000000000) >> 9;

  /*
    // Only look for probe request packets
    if (frameType != TYPE_MANAGEMENT ||
    //  frameSubType != SUBTYPE_PROBE_REQUEST ||
    frameSubType != SUBTYPE_PROBE_RESPONSE ||
    frameSubType != SUBTYPE_BEACONS ||
    frameSubType != 0x0028  // QoS Data
    ) return;
    //if  (!((frameSubType == SUBTYPE_PROBE_RESPONSE) || (frameSubType == SUBTYPE_BEACONS ) || (frameSubType == 0x0028))) return;
    //if  ((frameSubType == SUBTYPE_PROBE_RESPONSE) || (frameSubType == SUBTYPE_BEACONS )) {
  */

  if ((frameSubType == SUBTYPE_BEACONS) && (version == 0)) {
    uint8_t SSID_length = pkt->payload[37];
    if (SSID_length > 32) return;

    bool ascii_error = false;
    for (u = 0; u < SSID_length; u++) {
      if (!isprint(pkt->payload[38 + u])) {
        log_d("NO IMPRI %02d - %02d", u, SSID_length);
        ascii_error = true;
      }
      if (!isAscii(pkt->payload[38 + u])) {
        log_d("NO ASCII %02d - %02d", u, SSID_length);
        ascii_error = true;
      }
    }

    if (ascii_error) return;

    memcpy(&ssid_known[MAX_SSIDs - 1].mac, pkt->payload + 16, 6);

    bool known = false;
    for (u = 0; u < ssid_count; u++) {
      if (!memcmp(ssid_known[u].mac, ssid_known[MAX_SSIDs - 1].mac, 6)) {
        known = true;
        break;
      }
    }

    if (!known) {
      // only write the beacon packet the first time that we see it, to reduce writing on the SD-CARD.
      if (useSD) sdBuffer.addPacket(pkt->payload, packetLength);

      memcpy(&ssid_known[ssid_count].mac, &ssid_known[MAX_SSIDs - 1].mac, 6);
      memcpy(&ssid_known[ssid_count].ssid, pkt->payload + 38, SSID_length);
      snprintf(last_ssid_mac, 18, "%s", ether_ntoa(ssid_known[ssid_count].mac));
      ssid_known[u].ssid_len = SSID_length;
      ssid_count++;
      setLastSSID(38, SSID_length, pkt->payload);

      last_rssi = pkt->rx_ctrl.rssi;

      Serial.printf("SSID count: %4d | Pack length: %4d  | SSID: %32s | RSSI: %4d\n",
                    ssid_count,
                    packetLength,
                    last_ssid,
                    last_rssi);
    }
  }

  //#define SUBTYPE_PROBE_REQUEST 0x04
  //#define SUBTYPE_PROBE_RESPONSE 0x05
  //#define SUBTYPE_BEACONS        0x08

  if (frameType != TYPE_MANAGEMENT) return;

  // Only look for probe request packets
  // if (frameType != TYPE_MANAGEMENT ||
  // frameSubType != SUBTYPE_PROBE_REQUEST
  //  )
  //        return;

  //  Serial.print(ctrl.rssi, DEC);
  //Serial.print(".");
  //Serial.println("");
  //Serial.printf("PACKET TYPE=%s CHAN=%02d, RSSI=%02d ",wifi_sniffer_packet_type2str(type),ctrl.channel,ctrl.rssi);
  //Serial.printf("PACKET TYPE=%s CHAN=%02d, RSSI=%02d ",wifi_sniffer_packet_type2str(type),pkt->rx_ctrl.channel,pkt->rx_ctrl.rssi);

  // Deprecated :
  // char addr[] = "00:00:00:00:00:00";
  // getMAC(addr, pkt->payload, 10);

  // log_d("MAC: %s", ether_ntoa( pkt->payload+10 ) );

  //  Serial.print(" Peer MAC: ");
  //  Serial.print(addr);

  //  uint8_t SSID_length = pkt->payload[25];
  //  Serial.print(" SSID: ");
  //  setLastSSID(26, SSID_length, pkt->payload);
}

void setChannel(int newChannel) {
  ch = newChannel;
  if (ch > MAX_CH || ch < 1) ch = 1;

  preferences.begin("packetmonitor32", false);
  preferences.putUInt("channel", ch);
  preferences.end();

  esp_wifi_set_promiscuous(false);
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous);
  esp_wifi_set_promiscuous(true);
}

bool setupSD() {
  if (!SD.begin(PIN_SPI_SD_CS, sd_spi)) {
    Serial.println("Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  return true;
}

void draw() {
  double multiplicator = getMultiplicator();
  int len;
  int rssi;

  if (pkts[MAX_X - 1] > 0) rssi = rssiSum / (int)pkts[MAX_X - 1];
  else rssi = rssiSum;

  display.clearBuffer();

  ////display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setCursor(5, 11);
  display.print(ch);
  display.drawStr(13, 11, ("|"));

  display.setCursor(18, 11);
  display.print(rssi);

  display.drawStr(35, 11, ("|"));
  display.setCursor(72, 11);
  display.print(tmpPacketCounter);

  display.drawStr(87, 11, ("["));

  display.setCursor(90, 11);
  display.print(deauths);

  display.drawStr(97, 11, ("]"));
  display.drawStr(105, 11, ("|"));
  display.setCursor(110, 11);
  display.print((useSD ? "SD" : ""));

  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStr(40, 12, ("Pkts:"));

  display.drawLine(0, 63 - MAX_Y, MAX_X, 63 - MAX_Y);
  for (int i = 0; i < MAX_X; i++) {
    len = pkts[i] * multiplicator;
    display.drawLine(i, 63, i, 63 - (len > MAX_Y ? MAX_Y : len));
    if (i < MAX_X - 1) pkts[i] = pkts[i + 1];
  }
  display.sendBuffer();
}

void coreTask() {


  uint32_t currentTime;
  int wexit = 1;

  while (wexit) {




    currentTime = millis();

    /* bit of spaghetti code, have to clean this up later :D */


    if (readButtonStateY(BTN_SEL)) {
      if (useSD) {
        useSD = false;
        sdBuffer.close(&SD);
        draw();
      } else {
        if (setupSD())
          sdBuffer.open(&SD);
        draw();
      }
    }

    if (readButtonStateY(BTN_B)) {

      setChannel(ch + 1);
      draw();
    }
    if (readButtonStateY(BTN_X)) {

      sdBuffer.save(&SD);
    }
    if (readButtonStateY(BTN_Y)) {
      esp_wifi_set_promiscuous(false);
      esp_wifi_stop();
      esp_wifi_deinit();
      Serial.println("pass null");
      wexit = 0;
      sdBuffer.save(&SD);
      sdBuffer.close(&SD);
    }




    if (wexit)


    {
      // save buffer to SD
      if (useSD)
        sdBuffer.save(&SD);

      // draw Display
      if (currentTime - lastDrawTime > 1000) {
        lastDrawTime = currentTime;
        // Serial.printf("\nFree RAM %u %u\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT), heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT));// for debug purposes

        pkts[MAX_X - 1] = tmpPacketCounter;

        draw();

        Serial.println((String)pkts[MAX_X - 1]);

        tmpPacketCounter = 0;
        deauths = 0;
        rssiSum = 0;
      }

      // Serial input
      if (Serial.available()) {
        ch = Serial.readString().toInt();
        if (ch < 1 || ch > 14) ch = 1;
        setChannel(ch);
      }
    }
  }
  return;
}

/* ===== main program ===== */
void wifi_packet_init() {

    /* show start screen */
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(6, 10, "PacketMonitor32");
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(24, 34, "Made with <3 by");
  display.drawStr(29, 44, "@Spacehuhn");
  //display.displ/ay();
  display.sendBuffer();

//  delay(1000);


  // Serial
  //Serial.begin(115200);

  // Settings
  preferences.begin("packetmonitor32", false);
  ch = preferences.getUInt("channel", 10);
  preferences.end();

  // System & WiFi
  nvs_flash_init();
  tcpip_adapter_init();
/*
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  //ESP_ERROR_CHECK(esp_wifi_set_country(WIFI_COUNTRY_EU));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

*/

  esp_event_loop_init(event_handler, NULL);
  esp_wifi_init(&cfg);
  //ESP_ERROR_CHECK(esp_wifi_set_country(WIFI_COUNTRY_EU));
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();


  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);

  // SD card
  sdBuffer = Buffer();

  sd_spi.begin(PIN_SPI_SD_CLK, PIN_SPI_SD_MISO, PIN_SPI_SD_MOSI, PIN_SPI_SD_CS);

  if (setupSD())
    sdBuffer.open(&SD);

  // I/O


  // pinMode(BUTTON_PIN, INPUT_PULLUP);

  // display
  /*
    #ifdef USE_DISPLAY
    display.begin();
    //display.init();

    #ifdef FLIP_DISPLAY
    // display.flipScreenVertically();
    display.setFlipMode(0);
    #endif
  */


  // start Wifi sniffer
  esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous);
  esp_wifi_set_promiscuous(true);
}






void wifi_stop() {


  esp_wifi_set_promiscuous(false);
  esp_wifi_stop();
  //esp_wifi_deinit();

  // Wifi stop
  //WiFi.disconnect(true);  // Disconnect from the network
  //WiFi.mode(WIFI_OFF);    // Switch WiFi off
  //esp_wifi_restore();
  //WiFi.eraseAP();
  //      WiFi.mode(WIFI_STA);
  //  WiFi.disconnect();
  //esp_wifi_restore();
  //WiFi.eraseAP();

  // System & WiFi
  //nvs_flash_erase();      // erase the NVS partition and...
  nvs_flash_init();  // initialize the NVS partition.
  tcpip_adapter_init();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  //ESP_ERROR_CHECK(esp_wifi_set_country(WIFI_COUNTRY_EU));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

  esp_wifi_set_channel(0, WIFI_SECOND_CHAN_NONE);
}
void disableWiFi(){
     //   adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
}

void shutwifi(){

    esp_wifi_set_promiscuous(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);

    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_stop();
    esp_wifi_deinit();

}
void restartwifi(){
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  //esp_wifi_set_promiscuous(true);
  //esp_wifi_set_promiscuous_rx_cb(&wifi_promiscuous);
  //esp_wifi_set_channel(0, WIFI_SECOND_CHAN_NONE);


}
