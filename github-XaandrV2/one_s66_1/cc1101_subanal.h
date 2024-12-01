#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>

#define RADIOLIB_CHECK_PARAMS (0)

static const uint32_t subghz_frequency_list[] = {
  /* 300 - 348 */
  300000000,
  302757000,
  303875000,
  303900000,
  304250000,
  307000000,
  307500000,
  307800000,
  309000000,
  310000000,
  312000000,
  312100000,
  312200000,
  313000000,
  313850000,
  314000000,
  314350000,
  314980000,
  315000000,
  318000000,
  330000000,
  345000000,
  348000000,
  350000000,

  /* 387 - 464 */
  387000000,
  390000000,
  418000000,
  430000000,
  430500000,
  431000000,
  431500000,
  433075000, /* LPD433 first */
  433220000,
  433420000,
  433657070,
  433889000,
  433920000, /* LPD433 mid */
  434075000,
  434176948,
  434190000,
  434390000,
  434420000,
  434620000,
  434775000, /* LPD433 last channels */
  438900000,
  440175000,
  464000000,
  467750000,

  /* 779 - 928 */
  779000000,
  868350000,
  868400000,
  868800000,
  868950000,
  906400000,
  915000000,
  925000000,
  928000000,
};

typedef struct
{
  uint32_t frequency_coarse;
  int rssi_coarse;
  uint32_t frequency_fine;
  int rssi_fine;
} FrequencyRSSI;

const int rssi_threshold_sub = -75;

void start_c1101() {

  radio_c.begin();
  Serial.println("Frequency Analayzer Started!");
}

void sub_anal() {
    display.clearBuffer();
  display.setCursor(2, 10);
  display.print("Subghz Analayzer");
  display.sendBuffer();  // transfer internal memory to the display

  int rssi;
  FrequencyRSSI frequency_rssi = {
    .frequency_coarse = 0, .rssi_coarse = -100, .frequency_fine = 0, .rssi_fine = -100
  };

  // First stage: coarse scan
  radio_c.setRxBandwidth(650); // 58, 68, 81, 102, 116, 135, 162, 203, 232, 270, 325, 406, 464, 541, 650 and 812 kHz    (81kHz seems to work best for me)
  size_t array_size = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
  for (size_t i = 0; i < array_size; i++) {
    uint32_t frequency = subghz_frequency_list[i];
    if (frequency != 467750000 && frequency != 464000000 && frequency != 390000000 && frequency != 312000000 && frequency != 312100000 && frequency != 312200000 && frequency != 440175000) {
      radio_c.setFrequency((float)frequency / 1000000.0);
      radio_c.receiveDirect();
      delay(2);
      rssi = radio_c.getRSSI();

      if (frequency_rssi.rssi_coarse < rssi) {
        frequency_rssi.rssi_coarse = rssi;
        frequency_rssi.frequency_coarse = frequency;
      }
    }
  }

  // Second stage: fine scan
  if (frequency_rssi.rssi_coarse > rssi_threshold_sub) {
    // for example -0.3 ... 433.92 ... +0.3 step 20KHz
    radio_c.setRxBandwidth(58);
    for (uint32_t i = frequency_rssi.frequency_coarse - 300000; i < frequency_rssi.frequency_coarse + 300000; i += 20000) {
      uint32_t frequency = i;
      radio_c.setFrequency((float)frequency / 1000000.0);
      radio_c.receiveDirect();
      delay(2);
      rssi = radio_c.getRSSI();

      if (frequency_rssi.rssi_fine < rssi) {
        frequency_rssi.rssi_fine = rssi;
        frequency_rssi.frequency_fine = frequency;
      }
    }
  }

  // Deliver results fine
  if (frequency_rssi.rssi_fine > rssi_threshold_sub) {
    Serial.printf("FINE        Frequency: %.2f  RSSI: %d\n", (float)frequency_rssi.frequency_fine / 1000000.0, frequency_rssi.rssi_fine);
       display.clearBuffer();
      display.setCursor(2, 1);
      display.print("Subghz Analayzer");

      display.setCursor(2, 15);
      display.print("Frequency:");
      display.setCursor(70, 15);
      display.print((float)frequency_rssi.frequency_fine / 1000000.0);

            display.setCursor(2, 30);
      display.print("RSSI:");
      display.setCursor(70, 30);
      display.print(frequency_rssi.rssi_fine);

      display.sendBuffer();  // transfer internal memory to the display
 
 
  }

  // Deliver results coarse
  else if (frequency_rssi.rssi_coarse > rssi_threshold_sub) {
    Serial.printf("COARSE      Frequency: %.2f  RSSI: %d\n", (float)frequency_rssi.frequency_coarse / 1000000.0, frequency_rssi.rssi_coarse);
  }
  
  delay(10);
}
