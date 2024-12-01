//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
void reset_nrf() {
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

  if (!radio.begin()) {
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 15, "radio hardware is not responding!!");
    display.sendBuffer();  // transfer internal memory to the display
    ///// while (1) {}  // hold in infinite loop
    //return proceed;
    while (1) {}
  }  ///connection error
}


uint8_t writeRegister_nrf24(uint8_t reg, uint8_t value) {
  uint8_t status;

  digitalWrite(CSN, LOW);
  status = SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
  SPI.transfer(value);
  digitalWrite(CSN, HIGH);
  return status;
}

uint8_t writeRegister_nrf24(uint8_t reg, const uint8_t* buf, uint8_t len) {
  uint8_t status;

  digitalWrite(CSN, LOW);
  status = SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
  while (len--)
    SPI.transfer(*buf++);
  digitalWrite(CSN, HIGH);

  return status;
}






int channel_detected = 0;
int nrf24_scan_channel[126];
char nrf24_textbb[32] = "";


void scanner_channel() {
  int channel_nrf = atoi(NRFChannelStart);
  int channel_nrfend = atoi(NRFChannelEnd);
  // the order of the following is VERY IMPORTANT
  radio.setAutoAck(false);
  // radio.setPALevel(RF24_PA_MIN);
  // radio.setDataRate(RF24_2MBPS);
  writeRegister_nrf24(RF_SETUP, 0x09);  // Disable PA, 2M rate, LNA enabled
  radio.setPayloadSize(32);
  radio.setChannel(channel_nrf);
  // RF24 doesn't ever fully set this -- only certain bits of it
  writeRegister_nrf24(EN_RXADDR, 0x00);
  // RF24 doesn't have a native way to change MAC...
  // 0x00 is "invalid" according to the datasheet, but Travis Goodspeed found it works :)
  writeRegister_nrf24(SETUP_AW, 0x00);
  radio.openReadingPipe(0, 0xAALL);
  radio.disableCRC();
  radio.startListening();
  radio.printDetails();
  int dagdag = channel_nrf;

  Serial.println(channel_nrf);
  Serial.println(channel_nrfend);
  bool exit_now = 1;

  while (exit_now) {
    if (readButtonStateY(BTN_Y)) {
      exit_now = 0;
    }
    if (channel_nrfend < channel_nrf) {

      nrf24_scan_channel[channel_nrf] = 999;
      channel_nrf = atoi(NRFChannelStart);
      dagdag = atoi(NRFChannelStart);
    }
    radio.setChannel(channel_nrf);
    bool goodSignal = radio.testRPD();  //testCarrier().
    //float time_c = millis();
    Serial.print("channel - ");
    Serial.print(channel_nrf);
    Serial.print(" - ");

    //   radio.startListening();
    //   delayMicroseconds(128);
    //   radio.stopListening();

    uint8_t sz = 0;

    float time_c = millis();
    while ((millis() - time_c < 100)) {
      if (radio.available() && !sz) {  ///
        if (radio.getDynamicPayloadSize() < 1) {
          // Corrupt payload has been flushed
          return;
        }
        radio.read(&nrf24_textbb, sizeof(nrf24_textbb));
        radio.flush_rx();
        if (nrf24_textbb[0] != '\0') {
          sz = radio.getDynamicPayloadSize();
          nrf24_scan_channel[channel_nrf] = channel_nrf;
          channel_detected = channel_nrf;
          Serial.println("XXXXXXXXXXXXXXXXXXXXXX");
          Serial.println(sz);
          Serial.println(goodSignal ? "Strong signal > 64dBm" : "Weak signal < 64dBm");
          // radio.read(&nrf24_textbb, sizeof(nrf24_textbb));
          Serial.print("Channel  : ");
          Serial.println(channel_detected);
          Serial.println(String(nrf24_textbb, HEX));

          for (int j = 0; j < sizeof(nrf24_textbb); j++) {
            Serial.print(nrf24_textbb[j], HEX);
            Serial.print(" ");
          }
          Serial.println();


          display.clearBuffer();
          display.setCursor(2, 15);
          display.print("2.4Ghz Analayzer");


          display.setCursor(2, 30);
          display.print("Channel : ");
          display.setCursor(70, 30);
          display.print(channel_detected);


          display.setCursor(2, 45);
          display.print("Data Size : ");
          display.setCursor(64, 45);
          display.print(sizeof(nrf24_textbb));

          display.setCursor(2, 55);
          display.print(channel_nrf);


          display.sendBuffer();  // transfer internal memory to the display
        }
      }

      /* empty channel
        else {
        Serial.println("M");
        Serial.println(sz);
        }
      */
    }




    channel_nrf += 1;
  }
}



void nrf24_c(char* b) {
  delay(5);
  //char texta[]="/0";
  //radio.openWritingPipe(NRF24Add_d);
  //radio.setAutoAck(true);
  radio.stopListening();
  //  radio.write(&texta, sizeof(texta));
  Serial.print("send >>>>>> ");
  Serial.println(String(Keybuffer));
  radio.write(&Keybuffer, sizeof(Keybuffer));
  Serial.print("send comple ");
  //radio.setAutoAck(false);
  radio.startListening();
  //delay(50);
}


void nrf24_string(String b) {
  delay(5);
  int len = b.length();

  char texta[len + 1] = "\0";
  b.toCharArray(texta, len + 1);
  //radio.openWritingPipe(NRF24Add_d);
  //radio.setAutoAck(true);
  radio.stopListening();
  //  radio.write(&texta, sizeof(texta));
  Serial.print("send nrf str>>>>>> ");
  Serial.println(String(texta));
  Serial.println(String(sizeof(texta)));

  radio.write(&texta, sizeof(texta));
  Serial.println("send nrf str comple ");
  //radio.setAutoAck(false);
  radio.startListening();
  //delay(50);
}


char textbb[32] = "";
char textbbuffer[32] = "";

int nrf24_r() {
  delay(5);
  bool r_ok = 0;
  radio.startListening();
  if (radio.available()) {
    textbbuffer[0] = '\0';
    textbb[0] = '\0';
    radio.read(&textbb, sizeof(textbb));
    display.setFont(fontName);
    strcpy(textbbuffer, textbb);
    Serial.print("recieve >>>>>> ");
    Serial.println(String(textbb));
    Serial.println(textbb);
    Serial.println(char(textbb[0]));
    r_ok = 1;
  } else {
    r_ok = 0;
  }
  radio.flush_rx();  ///////////////////////////////////////
  display.drawStr(2, 12, "COM NRF24 ");
  display.drawStr(2, 30, textbbuffer);
  return r_ok;
  //} while (display.nextPage());
}




void nrf24_i() {
  pin_reset();

  while (!radio.begin()) {

    radio.powerDown();
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 2, "NRF24 is not responding!!");
    display.sendBuffer();  // transfer internal memory to the display
    ///// while (1) {}  // hold in infinite loop
    //return proceed;
    delay(10);
    radio.powerUp();
    pin_reset();
  }  ///connection error

  String s1;
  String s2;
  String s3;
  s1 = bufADDd;
  s1.getBytes(NRF24Add_d, 6);
  s3 = bufADDp;
  s3.getBytes(NRF24Add_p, 6);
  s2 = bufPiped;
  s2.getBytes(NRF24Pipe_d, 1);
  NRF24Pipe_X = NRF24Pipe_d[0];

  radio.setPALevel(RF24_PA_LOW);
  //radio.setAutoAck(1);
  //radio.setRetries(0,15);

  radio.openWritingPipe(NRF24Add_d);  // 00002

  radio.openReadingPipe(NRF24Pipe_X, NRF24Add_p);  // 00001
  //  radio.openReadingPipe(NRF24Pipe, 00001); // 00001
  display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
  display.drawStr(25, 15, "NRF24 detected!!");
  display.sendBuffer();  // transfer internal memory to the display
}

void nrf24_jam_init() {
  pin_reset();
  display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
  display.drawStr(25, 15, "Initialized 2.4 Jammer");
  display.sendBuffer();  // transfer internal memory to the display

  while (!radio.begin()) {

    radio.powerDown();
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 15, "radio hardware is not responding!!");
    display.sendBuffer();  // transfer internal memory to the display
    ///// while (1) {}  // hold in infinite loop
    //return proceed;
    delay(10);
    radio.powerUp();
    pin_reset();
  }  ///connection error
  radio.startListening();
  radio.stopListening();
  writeRegister_nrf24(0x01, 0x00);
  writeRegister_nrf24(0x06, 0x0F);
  display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
  display.drawStr(25, 15, "Start 2.4GHZ Jamming");
  display.sendBuffer();  // transfer internal memory to the display

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
}
const uint8_t noise[31] = {
  0x52, 0x75, 0x66, 0x69, 0x6E, 0x6F,
  0x56, 0x69, 0x63, 0x74, 0x6F, 0x72, 0x44,
  0x52, 0x53, 0x61, 0x62, 0x61,
  0x6C, 0x6C, 0x61, 0x58,
  0x61, 0x6E, 0x64, 0x65, 0x72, 0x4D, 0x69, 0x73, 0x73
};  // send random noise

int nrf24_jam_start() {

  for (int i = 0; i <= 125; i++) {
    




    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 15, "JAMMING!!");
    display.drawStr(25, 24, "Channel: ");
    display.setCursor(40, 35);
    display.print(i);
    //display.drawStr(40, 24, char(i));

    display.sendBuffer();  // transfer internal memory to the display
    radio.setChannel(i);
    radio.write(noise, 31);
    Serial.print("Jamming: ");
    Serial.println(i);
      
    if (readButtonStateY(BTN_Y)) {
      return 1;
    }
  //  for (int iab = 0; iab <= 31; iab++) {
   //   Serial.print(noise[iab]);
    //  Serial.print(" ");
   // }
    Serial.println();


    delay(10);
  }
}
