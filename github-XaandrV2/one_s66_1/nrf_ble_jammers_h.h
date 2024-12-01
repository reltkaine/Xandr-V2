/* ___________;_________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   ________________________________________ */
   
#include <Arduino.h> 








const byte channels[] = {2, 26, 80};
byte currentChannelIndex = 0;

volatile bool modeChangeRequested = false;

unsigned long lastJammingTime = 0;
const unsigned long jammingInterval = 10;



void configureRadio( byte channel) {
  radio.powerDown();
  delay(1000);
  radio.powerUp();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  radio.setChannel(channel);
}

void initializeRadiosMultiMode() {
  if (radio.begin()) {
    Serial.println("Multi-Module Mode: Radio 1 started");
    configureRadio( 80);
  } else {
    Serial.println("Failed to initialize Radio 1");
  }



  Serial.println("All radios configured and ready in Multi-Module Mode");
}

void initializeRadios() {
    if (radio.begin()) {
      Serial.println("Single Module Mode: Radio 1 started");
      configureRadio( channels[currentChannelIndex]);
    } else {
      Serial.println("Failed to initialize Radio 1 in Single Module Mode");
    }

}

void jammer( int channel) {
  //const char text[] = "xxxxxxxxxxxxxxxx";
  const uint8_t text[] = {
  0x52, 0x75, 0x66, 0x69, 0x6E, 0x6F,
  0x56, 0x69, 0x63, 0x74, 0x6F, 0x72, 0x44,
  0x52, 0x53, 0x61, 0x62, 0x61,
  0x6C, 0x6C, 0x61, 0x58,
  0x61, 0x6E, 0x64, 0x65, 0x72, 0x4D, 0x69, 0x73, 0x73
};
  for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
    radio.setChannel(i);
    bool result = radio.write(&text, sizeof(text));
    if (result) {
      Serial.println("Transmission successful");
    } else {
      Serial.println("Transmission failed");
    }
    delay(10);
  }
}

void updateOLED() {
  display.clearBuffer();
  //display.setFont(u8g2_font_ncenB08_tr);

  display.setCursor(0, 10);
  display.print("Ble Jammer ");
  display.print(" ........ ");
 // display.setCursor(70, 10);
 // display.print("[");
  //display.print(currentMode == SINGLE_MODULE ? "Single" : currentMode == MULTI_MODULE ? "Multi" : "Deactive");
  //display.print("]");

  display.setCursor(0, 35);
  display.print("Hold Y-Button to Exit ");
  //display.setCursor(70, 35);
 // display.print(radio.isChipConnected() ? "Active" : "Inactive");


  display.sendBuffer();
}



void blejammerSetup(){
  
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2
 
 #else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster
 
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  #endif
  display.begin();
 
  initializeRadios();
  updateOLED();
 
}

void blejammerLoop(){  
  updateOLED();

    if (millis() - lastJammingTime >= jammingInterval) {
      jammer(channels[currentChannelIndex]);
      currentChannelIndex = (currentChannelIndex + 1) % 3;
      Serial.print("Single Module Mode: Jamming on channel range ");
      Serial.println(channels[currentChannelIndex]);
      
      lastJammingTime = millis();
    }
     
}
