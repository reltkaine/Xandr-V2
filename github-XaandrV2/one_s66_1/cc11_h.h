//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
String str;
byte buffer_cc1101[61] = { 0 };


void initprotocol() {
#ifdef CONFIG_IDF_TARGET_ESP32S2
  //rfid  lite 25
  pinMode(6, OUTPUT);
  digitalWrite(6, 1);
  //CC1101  lite 5 gdo2 4  gdo0
  pinMode(3, OUTPUT);
  digitalWrite(3, 1);
  //RF24 lite 15
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
#else
  //rfid  lite 25  s2 6
  pinMode(25, OUTPUT);
  digitalWrite(25, 1);
  //CC1101  lite 5 gdo2 4  gdo0 2  s2 3
  pinMode(5, OUTPUT);
  digitalWrite(5, 1);
  //RF24 lite 15 s2 2
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
#endif
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, cc1101_CS);
  ELECHOUSE_cc1101.setGDO(gdo0, gdo2);
  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  } else {
    Serial.println("Connection Error");
  }
  //CC1101 Settings:                (Settings with "//" are optional!)
  ELECHOUSE_cc1101.Init();  // must be set to initialize the cc1101!
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  //ELECHOUSE_cc1101.setPA(10);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
  ELECHOUSE_cc1101.setMHZ(433.92);  // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setRxBW(58);
  mySwitch.enableReceive(gdo2);      // Receiver on
  ELECHOUSE_cc1101.SetRx();          // set Receive on
  start_freq = atof(FreqStartChar);  //start scan frequency set.
  stop_freq = atof(FreqEndChar);     //stop scan frequency set.
  Serial.println("Frequency Scanner Ready");
}
unsigned long found_value = 0;



void analyze_cc1101(float start_ma) {

  unsigned long nrf_startMillis;  //some global variables available anywhere in the program
  unsigned long nrf_currentMillis;
  const unsigned long nrf_period = 20;  //the value is a number of milliseconds




  Serial.println(start_ma);
  nrf_startMillis = millis();
  ELECHOUSE_cc1101.setMHZ(start_ma);



  while (millis() - nrf_startMillis <= nrf_period) {

    if (mySwitch.available()) {
      display.setCursor(2, 30);
      display.print("DATA:");
      display.setCursor(70, 30);
      found_value = mySwitch.getReceivedValue();
      display.print(found_value);

      display.setCursor(2, 45);
      display.print("Protocol:");
      display.setCursor(70, 45);
      display.print(mySwitch.getReceivedProtocol());

      display.sendBuffer();  // transfer internal memory to the display

      Serial.print("Received ");
      Serial.print(mySwitch.getReceivedValue());
      Serial.print(" / ");
      Serial.print(mySwitch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.print(mySwitch.getReceivedProtocol());
      Serial.print(" Delay: ");
      Serial.println(mySwitch.getReceivedDelay());


      mySwitch.disableReceive();  // Receiver off
    }

    if (found_value == 0) {
      display.clearBuffer();
      display.setCursor(2, 1);
      display.print("Subghz Analayzer");

      display.setCursor(2, 15);
      display.print("Frequency:");
      display.setCursor(70, 15);
      display.print(start_ma);

      display.setCursor(2, 30);
      display.print("DATA:");

      display.setCursor(2, 45);
      display.print("Protocol:");

      display.sendBuffer();  // transfer internal memory to the display
    }
  }






  start_ma += 0.01;




  if (readButtonStateY(BTN_X)) {
    start_ma = start_freq;
  }


  if (readButtonStateY(BTN_A)) {

    display.clearBuffer();
    display.setCursor(2, 1);
    display.print("Sending");

    display.setCursor(2, 15);
    display.print("Frequency:");
    display.setCursor(70, 15);
    display.print(start_ma);

    display.setCursor(2, 30);
    display.print("DATA:");
    display.setCursor(70, 30);
    display.print(mySwitch.getReceivedValue());

    display.setCursor(2, 45);
    display.print("Protocol:");
    display.setCursor(70, 45);
    display.print(mySwitch.getReceivedProtocol());

    display.sendBuffer();  // transfer internal memory to the display


    mySwitch.enableTransmit(gdo0);  // Transmit on
    ELECHOUSE_cc1101.SetTx();       // set Transmit on

    Serial.println("Transmit");

    mySwitch.setRepeatTransmit(15);                                               // transmission repetitions.
    mySwitch.setProtocol(mySwitch.getReceivedProtocol());                         // send Received Protocol
    mySwitch.setPulseLength(mySwitch.getReceivedDelay());                         // send Received Delay
    mySwitch.send(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength());  // send Received value/bits

    ELECHOUSE_cc1101.SetRx();      // set Receive on
    mySwitch.disableTransmit();    // set Transmit off
    mySwitch.enableReceive(gdo2);  // Receiver on

    Serial.println("Receive");

    mySwitch.resetAvailable();
  }
}

static void cc1101initialize(void)

{
  // initializing library with custom pins selected
  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, cc1101_CS);
  ELECHOUSE_cc1101.setGDO(gdo0, gdo2);

  // Main part to tune CC1101 with proper frequency, modulation and encoding
  ELECHOUSE_cc1101.Init();                 // must be set to initialize the cc1101!
  ELECHOUSE_cc1101.setGDO0(gdo0);          // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
  ELECHOUSE_cc1101.setCCMode(1);           // set config for internal transmission mode. value 0 is for RAW recording/replaying
  ELECHOUSE_cc1101.setModulation(2);       // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  ELECHOUSE_cc1101.setMHZ(433.92);         // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setDeviation(47.60);    // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
  ELECHOUSE_cc1101.setChannel(0);          // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
  ELECHOUSE_cc1101.setChsp(199.95);        // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
  ELECHOUSE_cc1101.setRxBW(812.50);        // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setDRate(1.2);          // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
  ELECHOUSE_cc1101.setPA(10);              // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
  ELECHOUSE_cc1101.setSyncMode(2);         // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
  ELECHOUSE_cc1101.setSyncWord(211, 145);  // Set sync word. Must be the same for the transmitter and receiver. Default is 211,145 (Syncword high, Syncword low)
  ELECHOUSE_cc1101.setAdrChk(0);           // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
  ELECHOUSE_cc1101.setAddr(0);             // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
  ELECHOUSE_cc1101.setWhiteData(0);        // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
  ELECHOUSE_cc1101.setPktFormat(0);        // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
  ELECHOUSE_cc1101.setLengthConfig(1);     // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
  ELECHOUSE_cc1101.setPacketLength(0);     // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
  ELECHOUSE_cc1101.setCrc(0);              // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
  ELECHOUSE_cc1101.setCRC_AF(0);           // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
  ELECHOUSE_cc1101.setDcFilterOff(0);      // Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
  ELECHOUSE_cc1101.setManchester(0);       // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
  ELECHOUSE_cc1101.setFEC(0);              // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
  ELECHOUSE_cc1101.setPRE(0);              // Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24
  ELECHOUSE_cc1101.setPQT(0);              // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
  ELECHOUSE_cc1101.setAppendStatus(0);     // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
}




void cc1101_c() {
  //void cc1101_trans()
  String s = String(Keybuffer);
  byte abuffer[s.length() + 1];
  s.getBytes(abuffer, s.length() + 1);
  //delay(5);
  //int state = radio_c.transmit(s);
  //int state =
  //radio_c.transmit(Keybuffer, sizeof(Keybuffer));
  ELECHOUSE_cc1101.SendData(abuffer, s.length() + 1, 100);
  //ELECHOUSE_cc1101.SendData(byte(atoi(Keybuffer)), sizeof(Keybuffer), 100);
}

void cc1101_string(String s) {
  //void cc1101_trans()
  byte abuffer[s.length() + 1];
  s.getBytes(abuffer, s.length() + 1);
  //delay(5);
  //int state = radio_c.transmit(s);
  //int state =
  //radio_c.transmit(Keybuffer, sizeof(Keybuffer));
  ELECHOUSE_cc1101.SendData(abuffer, s.length() + 1, 100);
  //ELECHOUSE_cc1101.SendData(byte(atoi(Keybuffer)), sizeof(Keybuffer), 100);
}


void cc1101_i() {

  pin_reset();

  cc1101initialize();






  // carrier frequency:                   434.0 MHz
  // bit rate:                            32.0 kbps
  // frequency deviation:                 60.0 kHz
  // Rx bandwidth:                        250.0 kHz
  // output power:                        7 dBm
  // preamble length:                     32 bits


  float freq = atof(FreqChar);                     //434.0;
  float br = atof(BitRateChar);                    //32.0;
  float freqDev = atof(FreqDevChar);               //60.0;
  float rxBw = atof(RXBandChar);                   //250.0;
  int8_t pwr = atoi(OutPowChar);                   //7;
  uint8_t preambleLength = atoi(PreambleLenChar);  //32;
  int modulation = modul_mode;
  memset(buffer_cc1101, 0, sizeof(buffer_cc1101));
  //buffer_cc1101[61] = { 0 };

  Serial.println(freq);
  Serial.println(br);
  Serial.println(freqDev);
  Serial.println(rxBw);
  Serial.println(pwr);
  Serial.println(preambleLength);

  ELECHOUSE_cc1101.setMHZ(freq);  // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setModulation(modulation);
  ELECHOUSE_cc1101.setDeviation(freqDev);  // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
  ELECHOUSE_cc1101.setRxBW(rxBw);          // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  ELECHOUSE_cc1101.setPA(pwr);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!


  /*
    if (state == RADIOLIB_ERR_NONE) {
      display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
      display.drawStr(25, 15, "CC1101 success!!");
      display.sendBuffer();  // transfer internal memory to the display
    } else {
      display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
      display.drawStr(25, 15, "CC1101 FAILED!!");
      display.sendBuffer();  // transfer internal memory to the display
      while (true)
        ;
    }
  */
}






void cc1101_r()
//void cc1101_recv()
{

  display.drawStr(2, 12, "TESTT CC1101");

  if (ELECHOUSE_cc1101.CheckRxFifo(100)) {

    if (ELECHOUSE_cc1101.CheckCRC()) {  //CRC Check. If "setCrc(false)" crc returns always OK!
      Serial.print("Rssi: ");
      Serial.println(ELECHOUSE_cc1101.getRssi());
      Serial.print("LQI: ");
      Serial.println(ELECHOUSE_cc1101.getLqi());

      int len = ELECHOUSE_cc1101.ReceiveData(buffer_cc1101);
      buffer_cc1101[len] = '\0';
      Serial.println((char *)buffer_cc1101);
      for (int i = 0; i < len; i++) {
        Serial.print(buffer_cc1101[i]);
      }


      Serial.println();
      Serial.println(F("success!"));

      Serial.print(F("[CC1101] Data:\t\t"));
      Serial.println((char *)buffer_cc1101);
      display.setCursor(2, 30);
      display.print((char *)buffer_cc1101);
    }
  } else {


    display.setCursor(2, 30);
    display.print((char *)buffer_cc1101);
  }
  display.sendBuffer();
  /*
    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

    } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

    }*/
}

void cc1101_scan_init() {
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


  start_freq = atof(FreqStartChar);  //start scan frequency set.
  stop_freq = atof(FreqEndChar);     //stop scan frequency set.

  ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, cc1101_CS);
  //  ELECHOUSE_cc1101.setGDO(gdo0, gdo2);
  ELECHOUSE_cc1101.setRxBW(58);
  ELECHOUSE_cc1101.SetRx();
  Serial.println("Frequency Scanner Ready");
}


float detecet_freq = 0;
float detecet_rssi = 0;
void cc1101_scan() {
  ELECHOUSE_cc1101.setMHZ(freq);
  rssi = ELECHOUSE_cc1101.getRssi();
  Serial.println(start_freq);
  Serial.println(stop_freq);

  if (rssi > rssi_threshold) {
    if (rssi > mark_rssi) {
      mark_rssi = rssi;
      mark_freq = freq;
      detecet_freq = mark_freq;
      detecet_rssi = mark_rssi;
    }
  }

  freq += 0.01;

  display.clearBuffer();
  display.setCursor(2, 15);
  display.print("Subghz Analayzer");


  display.setCursor(2, 30);
  display.print("Frequency : ");
  display.setCursor(70, 30);
  display.print(detecet_freq);


  display.setCursor(2, 45);
  display.print("RSSI : ");
  display.setCursor(64, 45);
  display.print(detecet_rssi);

  display.setCursor(2, 55);
  display.print(freq);


  display.sendBuffer();  // transfer internal memory to the display

  if (freq > stop_freq) {


    freq = start_freq;

    if (mark_rssi > rssi_threshold) {

      long fr = mark_freq * 100;

      if (fr == compare_freq) {

        float reciev_delay = millis();  ///////esper

        Serial.print("Freq: ");
        Serial.println(mark_freq);
        Serial.print("Rssi: ");
        Serial.println(mark_rssi);


        if (millis() - reciev_delay >= 60)  ///////esper
        {
          cc1101_r();
        }

        mark_rssi = -100;
        compare_freq = 0;
        mark_freq = 0;
      } else {
        compare_freq = mark_freq * 100;
        freq = mark_freq - 0.10;
        mark_freq = 0;
        mark_rssi = -100;
      }
    }
  }
}
