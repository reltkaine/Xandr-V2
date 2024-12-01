//\\\\\\\\\\scanner
/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   ________________________________________ */

#include <Arduino.h>






#define CHANNELS 64
int channel[CHANNELS];

int line;
char grey[] = " .:-=+*aRW";


#define NRF24_CONFIG 0x00
#define NRF24_EN_AA 0x01
#define NRF24_RF_CH 0x05
#define NRF24_RF_SETUP 0x06
#define NRF24_RPD 0x09
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
char filled = 'F';
char drawDirection = 'R';
char slope = 'W';

byte sensorArray[129];

byte getregister(byte r) {
  byte c;

  digitalWrite(CSN, LOW);
  SPI.transfer(r & 0x1F);
  c = SPI.transfer(0);
  digitalWrite(CSN, HIGH);

  return c;
}

void setregister(byte r, byte v) {
  digitalWrite(CSN, LOW);
  SPI.transfer((r & 0x1F) | 0x20);
  SPI.transfer(v);
  digitalWrite(CSN, HIGH);
}

void powerUp(void) {
  setregister(NRF24_CONFIG, getregister(NRF24_CONFIG) | 0x02);
  delayMicroseconds(130);
}

void powerDown(void) {
  setregister(NRF24_CONFIG, getregister(NRF24_CONFIG) & ~0x02);
}

void enable(void) {
  digitalWrite(CE, HIGH);
}

void disable(void) {
  digitalWrite(CE, LOW);
}

void setrx(void) {
  setregister(NRF24_CONFIG, getregister(NRF24_CONFIG) | 0x01);
  enable();
  delayMicroseconds(100);
}

void scanChannels(void) {
  disable();
  for (int j = 0; j < 55; j++) {
    for (int i = 0; i < CHANNELS; i++) {
      setregister(NRF24_RF_CH, (128 * i) / CHANNELS);
      setrx();
      delayMicroseconds(40);
      disable();
      if (getregister(NRF24_RPD) > 0) channel[i]++;
    }
  }
}

void outputChannels(void) {
  int norm = 0;

  for (int i = 0; i < CHANNELS; i++)
    if (channel[i] > norm) norm = channel[i];

  Serial.print('|');
  for (int i = 0; i < CHANNELS; i++) {
    int pos;
    if (norm != 0) pos = (channel[i] * 10) / norm;
    else pos = 0;
    if (pos == 0 && channel[i] > 0) pos++;
    if (pos > 9) pos = 9;
    Serial.print(grey[pos]);
    channel[i] = 0;
  }

  Serial.print("| ");
  Serial.println(norm);

  display.clearBuffer();

  display.drawLine(0, 0, 0, 63);
  display.drawLine(127, 0, 127, 63);

  for (byte count = 0; count < 64; count += 10) {
    display.drawLine(127, count, 122, count);
    display.drawLine(0, count, 5, count);
  }

  for (byte count = 10; count < 127; count += 10) {
    display.drawPixel(count, 0);
    display.drawPixel(count, 63);
  }

  if (norm < 10) {
    byte drawHeight = map(norm, 0, 20, 0, 64);
    sensorArray[0] = drawHeight;
  }
  if (norm > 10) {
    byte drawHeight = map(norm, 0, 40, 0, 64);
    sensorArray[0] = drawHeight;
  }




  for (byte count = 1; count <= 127; count++) {
    if (filled == 'D' || filled == 'd') {
      if (drawDirection == 'L' || drawDirection == 'l') {
        display.drawPixel(count, 63 - sensorArray[count - 1]);
      } else {
        display.drawPixel(127 - count, 63 - sensorArray[count - 1]);
      }
    } else {
      if (drawDirection == 'L' || drawDirection == 'l') {
        if (slope == 'W' || slope == 'w') {
          display.drawLine(count, 63, count, 63 - sensorArray[count - 1]);
        } else {
          display.drawLine(count, 0, count, 63 - sensorArray[count - 1]);
        }
      } else {
        if (slope == 'W' || slope == 'w') {
          display.drawLine(127 - count, 63, 127 - count, 63 - sensorArray[count - 1]);
        } else {
          display.drawLine(127 - count, 0, 127 - count, 63 - sensorArray[count - 1]);
        }
      }
    }
  }

  //display.setFont(display_font_ncenB08_tr);
  display.setCursor(12, 12);
  display.print(norm);

  display.sendBuffer();

  for (byte count = 127; count >= 2; count--) {
    sensorArray[count - 1] = sensorArray[count - 2];
  }
}

void checkButtons() {
}


void scannerSetup() {
  // Serial.begin(115200);
#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
#endif
  display.begin();

  for (byte count = 0; count <= 128; count++) {
    sensorArray[count] = 0;
  }

  Serial.println("Starting 2.4GHz Scanner ...");
  Serial.println();

  SPI.begin(sck, miso, mosi, CSN);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(10000000);
  SPI.setBitOrder(MSBFIRST);

  pinMode(CE, OUTPUT);
  pinMode(CSN, OUTPUT);


  disable();

  powerUp();
  setregister(NRF24_EN_AA, 0x0);
  setregister(NRF24_RF_SETUP, 0x0F);
}

void scannerLoop() {

  checkButtons();
  scanChannels();
  outputChannels();
}
//=========================================================

//analyxer

#define N 128
uint8_t values[N];

#define CHANNELS 64
int CHannel[CHANNELS];


void ScanChannels1(void) {
  disable();
  // for (int j = 0; j < 10; j++) {
  for (int i = 0; i < CHANNELS; i++) {
    setregister(NRF24_RF_CH, (128 * i) / CHANNELS);
    setrx();
    delayMicroseconds(40);
    disable();
    if (getregister(NRF24_RPD) > 0) CHannel[i]++;
  }
  // }
}


void writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(CSN, LOW);
  SPI.transfer(reg | 0x20);
  SPI.transfer(value);
  digitalWrite(CSN, HIGH);
}

uint8_t readRegister(uint8_t reg) {
  digitalWrite(CSN, LOW);
  SPI.transfer(reg & 0x1F);
  uint8_t result = SPI.transfer(0x00);
  digitalWrite(CSN, HIGH);
  return result;
}

void setChanneln(uint8_t CHannel) {
  writeRegister(NRF24_RF_CH, CHannel);
}



void startListening() {
  digitalWrite(CE, HIGH);
}

void stopListening() {
  digitalWrite(CE, LOW);
}

bool carrierDetected() {
  return readRegister(NRF24_RPD) & 0x01;
}


void analyzerSetup() {

#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
#endif

  pinMode(CE, OUTPUT);
  pinMode(CSN, OUTPUT);

  SPI.begin(sck, miso, mosi, CSN);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(10000000);
  SPI.setBitOrder(MSBFIRST);

  display.begin();
  display.clearBuffer();
  display.sendBuffer();

  digitalWrite(CSN, HIGH);
  digitalWrite(CE, LOW);

  disable();

  powerUp();
  writeRegister(NRF24_EN_AA, 0x00);
  writeRegister(NRF24_RF_SETUP, 0x0F);
}

void analyzerLoop() {

  ScanChannels1();

  memset(values, 0, sizeof(values));

  int n = 50;
  while (n--) {
    int i = N;
    while (i--) {
      setChanneln(i);
      startListening();
      delayMicroseconds(128);
      stopListening();
      if (carrierDetected()) {
        ++values[i];
      }
    }
  }

  display.clearBuffer();
  int barWidth = SCREEN_WIDTH / N;
  int x = 0;
  for (int i = 0; i < N; ++i) {
    int v = 63 - values[i] * 3;
    if (v < 0) {
      v = 0;
    }
    display.drawVLine(x, v - 10, 64 - v);
    x += barWidth;
  }

  // display.setFont(display_font_ncenB08_tr);
  display.setCursor(0, 64);
  display.print("1...5...10...25..50...80...128");
  display.sendBuffer();

  delay(50);
}



///------------------------------------------jammer




/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   ________________________________________ */

#include <Arduino.h>

const int num_channelsnn = 64;
int value[num_channelsnn];
int valuesDisplay[32];
int channelsnn = 1;
const int num_reps = 50;
bool jamming = false;
const byte address[6] = "00001";

uint8_t dataRateIndex = 0;  // Index for cycling through data rates
uint8_t paLevelIndex = 0;   // Index for cycling through PA levels




void setRadioParameters() {
  switch (dataRateIndex) {
    case 0: radio.setDataRate(RF24_250KBPS); break;
    case 1: radio.setDataRate(RF24_1MBPS); break;
    case 2: radio.setDataRate(RF24_2MBPS); break;
  }

  switch (paLevelIndex) {
    case 0: radio.setPALevel(RF24_PA_MIN); break;
    case 1: radio.setPALevel(RF24_PA_LOW); break;
    case 2: radio.setPALevel(RF24_PA_HIGH); break;
    case 3: radio.setPALevel(RF24_PA_MAX); break;
  }

  Serial.print("Data Rate: ");
  Serial.println(dataRateIndex);
  Serial.print("PA Level: ");
  Serial.println(paLevelIndex);
}

void radioSetChannel(int channelsnn) {
  radio.setChannel(channelsnn);
}

void jammer() {
  const char text[] = {
    0x52, 0x75, 0x66, 0x69, 0x6E, 0x6F,
    0x56, 0x69, 0x63, 0x74, 0x6F, 0x72, 0x44,
    0x52, 0x53, 0x61, 0x62, 0x61,
    0x6C, 0x6C, 0x61, 0x58,
    0x61, 0x6E, 0x64, 0x65, 0x72, 0x4D, 0x69, 0x73, 0x73
  };
  for (int i = ((channelsnn * 5) + 1); i < ((channelsnn * 5) + 23); i++) {
    radio.setChannel(i);
    bool result = radio.write(&text, sizeof(text));
    if (result) {
      Serial.println("Transmission successful");
    } else {
      Serial.println("Transmission failed");
    }
    //   delay(1);
  }
}

void pressBt01() {


  if (readButtonStateY(BTN_UP)) {
    if (channelsnn < 13) {
      channelsnn++;
    } else {
      channelsnn = 0;
    }
    Serial.print("Channel: ");
    Serial.println(channelsnn);
  }

  if (readButtonStateY(BTN_SEL)) {
    jamming = !jamming;
    Serial.println(jamming ? "Jamming started" : "Jamming stopped");
  }

  if (readButtonStateY(BTN_RGT)) {
    dataRateIndex = (dataRateIndex + 1) % 3;  // Cycle through data rates
    setRadioParameters();
    Serial.println("Data rate changed");
  }

  if (readButtonStateY(BTN_LFT)) {
    paLevelIndex = (paLevelIndex + 1) % 4;  // Cycle through power levels
    setRadioParameters();
    Serial.println("Power level changed");
  }
}

void jammerSetup() {

#ifdef CONFIG_IDF_TARGET_ESP32S2  //8888888888888888888888888888888888888888888888888     esp32s2

#else  //*888888888888888888&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     esp32helo typemaster

  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
#endif

  SPI.begin(sck, miso, mosi, CSN);

  pinMode(CE, OUTPUT);
  pinMode(CSN, OUTPUT);

  display.begin();
  display.clearBuffer();
  display.sendBuffer();

  radio.begin();

  radio.powerDown();
  delay(1000);
  radio.powerUp();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);

  setRadioParameters();
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Radio configured and ready");
}

bool continue_jan = 1;
void jammerLoop() {






  pressBt01();

  display.clearBuffer();
  //    display.setFont(display_font_ncenB08_tr);

  // Display channel
  display.setCursor(0, 10);
  display.print("Channel : ");
  display.setCursor(80, 10);
  display.print("[");
  display.print(channelsnn);
  display.print("]");

  // Display PA level
  display.setCursor(0, 25);
  display.print("PA Level: ");
  display.setCursor(80, 25);
  switch (paLevelIndex) {
    case 0: display.print("MIN"); break;
    case 1: display.print("LOW"); break;
    case 2: display.print("HIGH"); break;
    case 3: display.print("MAX"); break;
  }

  // Display data rate
  display.setCursor(0, 40);
  display.print("Data Rate: ");
  display.setCursor(80, 40);
  switch (dataRateIndex) {
    case 0: display.print("250KBPS"); break;
    case 1: display.print("1MBPS"); break;
    case 2: display.print("2MBPS"); break;
  }

  display.setCursor(0, 60);
  display.print("Jamming: ");

  if (jamming) {
    display.setCursor(80, 60);
    display.print("Active ");
  } else {
    display.setCursor(80, 60);
    display.print("disable ");
  }


  if (jamming && continue_jan) {
    display.setCursor(80, 60);
    display.print("Active ");
    Serial.println("Starting jamming on channel " + String(channelsnn + 1));
    jammer();
  }



  display.sendBuffer();
  continue_jan = !continue_jan;
  delay(5);
}
