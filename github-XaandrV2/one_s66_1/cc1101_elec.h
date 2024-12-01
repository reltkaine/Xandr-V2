// Initialize CC1101 board with default settings, you may change your preferences here

unsigned long receivedValue = 0;
int receivedBitLength = 0;
int receivedProtocol = 0;




#define BUF_LENGTH 128
#define RECORDINGBUFFERSIZE 4096  // Buffer for recording the frames
byte bigrecordingbuffer[RECORDINGBUFFERSIZE] = { 0 };
float cc1101_capture_freq = 433.92;
byte textbuffer[BUF_LENGTH];
static void cc1101initialize2() {
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
  ELECHOUSE_cc1101.setDRate(9.6);          // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
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

void cc1101_scan_elec() {
  int setting, setting2, len;
  uint16_t brute, poweroftwo;
  byte j, k;
  float settingf1;
  float settingf2;
  // variables for frequency scanner
  float freq;
  long compare_freq;
  float mark_freq;
  int rssi;
  int mark_rssi = -100;
  int modulation = modul_mode;

  settingf1 = atof(FreqStartChar);

  settingf2 = atof(FreqEndChar);

  Serial.print("modulaton  ");

  Serial.println(modulation);

  Serial.print(F("\r\nScanning frequency range from : "));
  Serial.print(settingf1);
  Serial.print(F(" MHz to "));
  Serial.print(settingf2);
  Serial.print(F(" MHz, press any key for stop or wait...\r\n"));
  // initialize parameters for scanning
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setModulation(modulation);
  ELECHOUSE_cc1101.setRxBW(58);
  ELECHOUSE_cc1101.SetRx();
  // Do scanning until some key pressed
  freq = settingf1;  // start frequency for scanning
  mark_rssi = -100;

  int Cc1101_scan_exit = 1;
  display.clearBuffer();
  display.setCursor(2, 10);
  display.print("Subghz Analayzer");
  display.setCursor(2, 18);
  display.print(String(settingf1));
  display.setCursor(32, 18);
  display.print(String(settingf2));

  display.sendBuffer();  // transfer internal memory to the display

  while (Cc1101_scan_exit) {
    if (readButtonStateY(BTN_Y)) {
      Cc1101_scan_exit = 0;
    }

    ELECHOUSE_cc1101.setMHZ(freq);
    rssi = ELECHOUSE_cc1101.getRssi();
    if (rssi > -75) {
      if (rssi > mark_rssi) {
        mark_rssi = rssi;
        mark_freq = freq;
      };
    };

    freq += 0.01;

    if (freq > settingf2) {
      freq = settingf1;

      if (mark_rssi > -75) {
        long fr = mark_freq * 100;
        if (fr == compare_freq) {
          Serial.print(F("\r\nSignal found at  "));
          Serial.print(F("Freq: "));
          Serial.print(mark_freq);
          cc1101_capture_freq = mark_freq;
          Serial.print(F(" Rssi: "));
          Serial.println(mark_rssi);

          display.clearBuffer();
          display.setCursor(2, 10);
          display.print("Subghz Analayzer");

          display.setCursor(2, 25);
          display.print("Frequency:");
          display.setCursor(70, 25);
          display.print(mark_freq);

          display.setCursor(2, 40);
          display.print("RSSI:");
          display.setCursor(70, 40);
          display.print(mark_rssi);

          display.sendBuffer();  // transfer internal memory to the display

          mark_rssi = -100;
          compare_freq = 0;
          mark_freq = 0;
        } else {
          compare_freq = mark_freq * 100;
          freq = mark_freq - 0.10;
          mark_freq = 0;
          mark_rssi = -100;
        };
      };

    };  // end of IF freq>stop frequency

  };  // End of While
}

void cc1101_recraw() {
  bigrecordingbuffer[RECORDINGBUFFERSIZE] = { 0 };

  ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);
  int setting = 400;
  int setting2;
  if (setting > 0) {
    // setup async mode on CC1101 with GDO0 pin processing
    ELECHOUSE_cc1101.setCCMode(0);
    ELECHOUSE_cc1101.setPktFormat(3);
    ELECHOUSE_cc1101.SetRx();
    int modulation = modul_mode;
    ELECHOUSE_cc1101.setModulation(modulation);
    //start recording to the buffer with bitbanging of GDO0 pin state
    Serial.print(F("\r\nWaiting for radio signal to start RAW recording...\r\n"));
    pinMode(gdo0, INPUT);

    // this is only for ESP32 boards because they are getting some noise on the beginning
    setting2 = digitalRead(gdo0);
    delayMicroseconds(1000);


    // waiting for some data first or serial port signal
    // while (!Serial.available() ||  (digitalRead(gdo0) == LOW) );
    int Cc1101_scan_exit = 1;
    display.clearBuffer();
    display.setCursor(2, 10);
    display.print("Starting RAW recording to the buffer");
    display.setCursor(2, 30);
    display.print(cc1101_capture_freq);


    display.sendBuffer();  // transfer internal memory to the display
    while (Cc1101_scan_exit) {
      if (readButtonStateY(BTN_Y)) {
        Cc1101_scan_exit = 0;
      }
      // this is only for ESP32 boards because they are getting some noise on the beginning
      setting2 = digitalRead(gdo0);
      delayMicroseconds(10);

      while (digitalRead(gdo0) == LOW && !readButtonStateY(BTN_Y))
        ;
      Serial.print(F("\r\nNNOOO...\r\n"));
      display.clearBuffer();
      display.setCursor(2, 10);
      display.print("NONE");
      display.sendBuffer();  // transfer internal memory to the display

      //start recording to the buffer with bitbanging of GDO0 pin state
      Serial.print(F("\r\nStarting RAW recording to the buffer...\r\n"));
      display.clearBuffer();
      display.setCursor(2, 10);
      display.print("Capturing RAW recording to the buffer");

      display.sendBuffer();  // transfer internal memory to the display

      pinMode(gdo0, INPUT);

      for (int i = 0; i < RECORDINGBUFFERSIZE; i++) {
        byte receivedbyte = 0;
        for (int j = 7; j > -1; j--)  // 8 bits in a byte
        {
          bitWrite(receivedbyte, j, digitalRead(gdo0));  // Capture GDO0 state into the byte
          delayMicroseconds(setting);                    // delay for selected sampling interval
        };
        // store the output into recording buffer
        bigrecordingbuffer[i] = receivedbyte;
      }
      Serial.print(F("\r\nRecording RAW data complete.\r\n\r\n"));
      // setting normal pkt format again
      display.clearBuffer();
      display.setCursor(2, 10);
      display.print("CAPTURED");

      display.sendBuffer();  // transfer internal memory to the display

      ELECHOUSE_cc1101.setCCMode(1);
      ELECHOUSE_cc1101.setPktFormat(0);
      ELECHOUSE_cc1101.SetRx();
    }
  } else {
    Serial.print(F("Wrong parameters.\r\n"));
  };
}

void asciitohex(byte *ascii_ptr, byte *hex_ptr, int len) {
  byte i, j, k;
  for (i = 0; i < len; i++) {
    // high byte first
    j = ascii_ptr[i] / 16;
    if (j > 9) {
      k = j - 10 + 65;
    } else {
      k = j + 48;
    }
    hex_ptr[2 * i] = k;
    // low byte second
    j = ascii_ptr[i] % 16;
    if (j > 9) {
      k = j - 10 + 65;
    } else {
      k = j + 48;
    }
    hex_ptr[(2 * i) + 1] = k;
  };
  hex_ptr[(2 * i) + 2] = '\0';
}

void cc1101_showraw() {
  // show the content of recorded RAW signal as hex numbers
  Serial.print(F("\r\nRecorded RAW data:\r\n"));
  for (int i = 0; i < RECORDINGBUFFERSIZE; i = i + 32) {
    asciitohex(&bigrecordingbuffer[i], textbuffer, 32);
    Serial.print((char *)textbuffer);
  }
  display.clearBuffer();
  display.setCursor(2, 10);
  display.print("RECRAW");
  display.setCursor(20, 10);
  display.print((char *)textbuffer);

  display.sendBuffer();  // transfer internal memory to the display
  Serial.print(F("\r\n\r\n"));


  // handling SHOWBIT command
}


void cc1101_scan_raw() {
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);

  //  ELECHOUSE_cc1101.setMHZ(433.83);//(cc1101_capture_freq);
  ELECHOUSE_cc1101.SetRx();
  mySwitch.enableReceive(gdo2);   //35(gdo2);
  mySwitch.enableTransmit(gdo0);  //(gdo0);


  mySwitch.disableTransmit();
  ELECHOUSE_cc1101.SetRx();
  ELECHOUSE_cc1101.SetRx();
  ELECHOUSE_cc1101.SetRx();
  mySwitch.enableReceive(gdo2);  //35(gdo2);
  int modulation = modul_mode;
  ELECHOUSE_cc1101.setModulation(modulation);

  Serial.println(modulation);
  display.clearBuffer();
  display.setCursor(0, 10);
  display.print("Replay Attack 1:");
  display.setCursor(115, 10);
  display.print(modulation);
  display.print(" MHz");
  display.setCursor(6, 20);
  display.print(cc1101_capture_freq);
  display.print(" MHz");

  display.sendBuffer();


  int Cc1101_scan_exit = 1;
  while (Cc1101_scan_exit) {
    if (readButtonStateY(BTN_Y)) {
      Cc1101_scan_exit = 0;
    }
    if (mySwitch.available()) {
      receivedValue = mySwitch.getReceivedValue();
      receivedBitLength = mySwitch.getReceivedBitlength();
      unsigned int *rawSignal = mySwitch.getReceivedRawdata();
      receivedProtocol = mySwitch.getReceivedProtocol();
      Serial.print(F("Received:"));

      if (receivedValue != 0) {
        display.clearBuffer();
        display.setCursor(0, 10);
        display.print("Received:");
        Serial.print(F("Received:"));
        display.setCursor(70, 10);
        display.print(receivedValue);
        display.setCursor(0, 30);
        display.printf("Bit:%d ", receivedBitLength);
        display.setCursor(40, 30);
        display.printf("Ptc:%d ", receivedProtocol);
        display.setCursor(0, 50);
        display.print("Freq:");
        display.setCursor(40, 50);
        display.print(cc1101_capture_freq);
        display.print(" MHz");
        display.sendBuffer();
        mySwitch.resetAvailable();
      }
    }
    mySwitch.disableTransmit();
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.SetRx();
    mySwitch.enableReceive(gdo2);  //35(gdo2);
    mySwitch.resetAvailable();
  }
}
void cc1101_rxraw() {
  int setting = 10000;
  int Cc1101_scan_exit = 1;
  if (setting > 0 && Cc1101_scan_exit == 1) {
    if (readButtonStateY(BTN_Y)) {
      Cc1101_scan_exit = 0;
    }
    // setup async mode on CC1101 with GDO0 pin processing
    ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);
    ELECHOUSE_cc1101.setCCMode(0);
    //       ELECHOUSE_cc1101.setClb(1,13,15);/////000
    //       ELECHOUSE_cc1101.setClb(2,16,19);
    //       ELECHOUSE_cc1101.setModulation(2);
    //       ELECHOUSE_cc1101.setDRate(512);
    //       ELECHOUSE_cc1101.setRxBW(256);//000

    ELECHOUSE_cc1101.setPktFormat(3);
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);
    Serial.println(cc1101_capture_freq);
    int modulation = modul_mode;
    ELECHOUSE_cc1101.setModulation(modulation);
    Serial.println(modulation);
    //start recording to the buffer with bitbanging of GDO0 pin state
    Serial.print(F("\r\nSniffer enabled...\r\n"));
    pinMode(gdo0, INPUT);





    // Any received char over Serial port stops printing  RF received bytes

    while (Cc1101_scan_exit) {
      if (readButtonStateY(BTN_Y)) {
        Cc1101_scan_exit = 0;
      }

      // we have to use the buffer not to introduce delays
      for (int i = 0; i < RECORDINGBUFFERSIZE; i++) {
        byte receivedbyte = 0;
        for (int j = 7; j > -1; j--)  // 8 bits in a byte
        {
          bitWrite(receivedbyte, j, digitalRead(gdo0));  // Capture GDO0 state into the byte
          delayMicroseconds(setting);                    // delay for selected sampling interval
        };
        // store the output into recording buffer
        bigrecordingbuffer[i] = receivedbyte;
      };
      // when buffer full print the ouptput to serial port
      for (int i = 0; i < RECORDINGBUFFERSIZE; i = i + 32) {
        asciitohex(&bigrecordingbuffer[i], textbuffer, 32);
        Serial.print((char *)textbuffer);
      };
      Serial.println();

    };  // end of While loop

    Serial.print(F("\r\nStopping the sniffer.\n\r\n"));

    // setting normal pkt format again
    ELECHOUSE_cc1101.setCCMode(1);
    ELECHOUSE_cc1101.setPktFormat(0);
    ELECHOUSE_cc1101.SetRx();
  } else {
    Serial.print(F("Wrong parameters.\r\n"));
  };
}

//rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr

/*
   This sketch simply repeats data received from remote weather sensors made by Cresta.

   Setup:
    - connect digital output of a 433MHz receiver to digital pin 2 of Arduino.
    - connect transmitter input of a 433MHz transmitter to digital pin 11
    - An LED on pin 13 will tell you if and when a signal has been received and transmitted.

   Library:
   https://github.com/mattwire/arduino-dev/tree/master/libraries/RemoteSensor
   https://github.com/LSatan/SmartRC-CC1101-Driver-Lib

*/

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SensorReceiver.h>
#include <SensorTransmitter.h>
#include <NewRemoteReceiver.h>
#include <NewRemoteTransmitter.h>


int Interr = 1;  // Interrupt Numer
int Anz = 3;     // number of retransmissions
int Tw = 0;      // Wait Miliseconds before sending

int debug = 1;  // Debugmode ein (1)/aus(0)



int pinRx = gdo2;
int pinTx = gdo0;  // for esp32! Receiver on GPIO pin 4. Transmit on GPIO pin 2.
//LED_PIN = LED_BUILTIN;  // set led on GPIO pin 32.





void retransmit(byte *data) {
  // Data received

  // Wait a second after a receiving. There's little point for decoding and sending the same signal multiple times.
  SensorReceiver::disable();
  interrupts();  // delay() requires that interrupts are enabled
  delay(1000);

  // Flash LED when transmitting.
  digitalWrite(LED_PIN, HIGH);

  // Transmit signal. Note: this is a static method, no object required!
  ELECHOUSE_cc1101.SetTx();  // set Transmit on
  SensorTransmitter::sendPackage(pinTx, data);

  digitalWrite(LED_PIN, LOW);

  noInterrupts();
  ELECHOUSE_cc1101.SetRx();  // set Receive on
  SensorReceiver::enable();
}



void repeater_init() {


  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  } else {
    Serial.println("Connection Error");
  }

  //CC1101 Settings:                (Settings with "//" are optional!)
  ELECHOUSE_cc1101.Init();          // must be set to initialize the cc1101!
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  //ELECHOUSE_cc1101.setPA(10);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
  ELECHOUSE_cc1101.setMHZ(433.92);  // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.

  pinMode(LED_PIN, OUTPUT);

  // Since we're not instantiating SensorTransmitter, but only use the static methods of SensorTransmitter,
  // the pin mode must be set manually.
  pinMode(pinTx, OUTPUT);

  // When no signal has been received, the LED is lit.
  digitalWrite(LED_PIN, HIGH);

  // Init the receiver on interrupt pin 0 (digital pin 2).
  // Set the callback to function "retransmit", which is called
  // whenever valid sensor data has been received.
  ELECHOUSE_cc1101.SetRx();  // set Receive on
  SensorReceiver::init(pinRx, retransmit);
}





//=========================================================================================





NewRemoteCode receivedCode;    //  Global var to store received code for retramitt it
boolean codeLearned =  false ;   //  Flag to set a valid learned code has been received



void   retransmitter (NewRemoteCode retransmitterCode)
{

  //  Disable the receiver; otherwise it might pick up the retransmit as well.
  NewRemoteReceiver::disable ();

  //  Store received code for retramitt it in loop()
  receivedCode = retransmitterCode;
  //  Set valid learned code has been received
  codeLearned =  true ;
}





void  ReTrans (
  unsigned int period,
  unsigned long address,
  unsigned long groupBit,
  unsigned long unit,
  unsigned long switchType,
  boolean dimLevelPresent,
  byte dimLevel)
{

  // Print the received code.
  Serial.print("Code: ");
  Serial.print(address);
  Serial.print(" Period: ");
  Serial.println(period);
  Serial.print(" unit: ");
  Serial.println(unit);
  Serial.print(" groupBit: ");
  Serial.println(groupBit);
  Serial.print(" switchType: ");
  Serial.println(switchType);

  if (dimLevelPresent) {
    Serial.print(" dimLevel: ");
    Serial.println(dimLevel);
  }

}
void ReTrans_setup() {

  if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
    Serial.println("Connection OK");
  } else {
    Serial.println("Connection Error");
  }

  //CC1101 Settings:                (Settings with "//" are optional!)
  ELECHOUSE_cc1101.Init();          // must be set to initialize the cc1101!
  //ELECHOUSE_cc1101.setRxBW(812.50);  // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
  //ELECHOUSE_cc1101.setPA(10);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
  ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);  // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.



  ELECHOUSE_cc1101.SetRx();  // set Receive on
  // See example ShowReceivedCode for info on this1
  NewRemoteReceiver::init(pinRx, Interr, ReTrans);

  Serial.println("Receiver initialized... ");

}


//  mySwitch.enableReceive(gdo2);   //35(gdo2);
//  mySwitch.enableTransmit(gdo0);  //(gdo0);

#define RX_PIN gdo2
#define TX_PIN gdo0

void rpattackSetup() {


  display.clearBuffer();

  display.setCursor(0, 10);
  display.print("Received:");
  display.setCursor(70, 10);
  display.print("000000");
  display.setCursor(0, 30);
  display.printf("Bit: -");
  display.setCursor(40, 30);
  display.printf("Ptc: -");
  display.setCursor(0, 50);
  display.print("Freq:");
  display.setCursor(40, 50);
  display.print("0");
  display.print(" MHz");
  display.sendBuffer();
  delay(1000);
  display.clearBuffer();

  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetRx();

  mySwitch.enableReceive(RX_PIN);
  mySwitch.enableTransmit(TX_PIN);
}

void rpattackLoop() {

  static int lastPosition = -1;


  ELECHOUSE_cc1101.setMHZ(cc1101_capture_freq);

  display.clearBuffer();
  display.setCursor(0, 10);
  display.print("Received:");
  display.setCursor(70, 10);
  display.print(receivedValue);
  display.setCursor(0, 30);
  display.printf("Bit:%d ", receivedBitLength);
  display.setCursor(40, 30);
  display.printf("Ptc:%d ", receivedProtocol);
  display.setCursor(0, 50);
  display.print("Freq:");
  display.setCursor(40, 50);
  display.print(cc1101_capture_freq);
  display.print(" MHz");
  display.sendBuffer();


  if (mySwitch.available()) {
    receivedValue = mySwitch.getReceivedValue();
    receivedBitLength = mySwitch.getReceivedBitlength();
    unsigned int* rawSignal = mySwitch.getReceivedRawdata();
    receivedProtocol = mySwitch.getReceivedProtocol();

    if (receivedValue != 0) {
      display.clearBuffer();
      display.setCursor(0, 10);
      display.print("Received:");
      display.setCursor(70, 10);
      display.print(receivedValue);
      display.setCursor(0, 30);
      display.printf("Bit:%d ", receivedBitLength);
      display.setCursor(40, 30);
      display.printf("Ptc:%d ", receivedProtocol);
      display.setCursor(0, 50);
      display.print("Freq:");
      display.setCursor(40, 50);
      display.print(cc1101_capture_freq);
      display.print(" MHz");
      display.sendBuffer();

      mySwitch.resetAvailable();
    }
  }

  if (readButtonStateY(BTN_SEL) == LOW && receivedValue != 0) {
    mySwitch.disableReceive();
    delay(100);
    mySwitch.enableTransmit(TX_PIN);
    ELECHOUSE_cc1101.SetTx();

    display.clearBuffer();
    display.setCursor(0, 10);
    display.print("Value:");
    display.setCursor(40, 10);
    display.print(receivedValue);
    display.setCursor(0, 30);
    display.print("Sending...");
    display.sendBuffer();

    mySwitch.setProtocol(receivedProtocol);
    mySwitch.send(receivedValue, receivedBitLength);

    delay(500);
    display.clearBuffer();
    display.setCursor(0, 10);
    display.print("Value:");
    display.setCursor(40, 10);
    display.print(receivedValue);
    display.setCursor(0, 30);
    display.print("Done!");
    display.sendBuffer();

    ELECHOUSE_cc1101.SetRx();
    mySwitch.disableTransmit();
    delay(100);
    mySwitch.enableReceive(RX_PIN);
  }
}
