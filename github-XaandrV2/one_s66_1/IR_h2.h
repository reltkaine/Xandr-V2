/*
   IRremoteESP8266: SmartIRRepeater.ino - Record and playback IR codes.
   Copyright 2019 David Conran (crankyoldgit)

   This program will try to capture incoming IR messages and tries to
   intelligently replay them back.
   It uses the advanced detection features of the library, and the custom
   sending routines. Thus it will try to use the correct frequencies,
   duty cycles, and repeats as it thinks is required.
   Anything it doesn't understand, it will try to replay back as best it can,
   but at 38kHz.
   Note:
     That might NOT be the frequency of the incoming message, so some not
     recogised messages that are replayed may not work. The frequency & duty
     cycle of unknown incoming messages is lost at the point of the Hardware IR
     demodulator. The ESP can't see it.

                                 W A R N I N G
     This code is just for educational/example use only. No help will be given
     to you to make it do something else, or to make it work with some
     weird device or circuit, or to make it more usable or practical.
     If it works for you. Great. If not, Congratulations on changing/fixing it.

   An IR detector/demodulator must be connected to the input, kRecvPin.
   An IR LED circuit must be connected to the output, kIrLedPin.

   Example circuit diagrams (both are needed):
    https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
    https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending

   Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
       have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
       See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Some digital camera/phones can be used to see if the IR LED is flashed.
       Replace the IR LED with a normal LED if you don't have a digital camera
       when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP will interfere.
 *     * Pin 16/D0: Has no interrupts on the ESP8266, so can't be used for IR
         receiving with this library.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
       for your first time. e.g. ESP-12 etc.

   Changes:
     Version 1.0: June, 2019
       - Initial version.
*/

#include <Arduino.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

// ==================== start of TUNEABLE PARAMETERS ====================

// The GPIO an IR detector/demodulator is connected to. Recommended: 14 (D5)
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.

const uint16_t kRecvPin = IR_RECEIVE_PIN;
// ARDUINO_ESP32C3_DEV

// GPIO to use to control the IR LED circuit. Recommended: 4 (D2).
const uint16_t kIrLedPin = IR_SEND_PIN;

// The Serial connection baud rate.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/resender, let's use a larger
// than expected buffer so we can handle very large IR messages.
const uint16_t kCaptureBufferSize = 1024;  // 1024 == ~511 bits

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
const uint8_t kTimeout = 50;  // Milli-Seconds

// kFrequency is the modulation frequency all UNKNOWN messages will be sent at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

// ==================== end of TUNEABLE PARAMETERS ====================

// The IR transmitter.
IRsend irsend(kIrLedPin);
// The IR receiver.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
// Somewhere to store the captured message.
decode_results results;

// This section of code runs only once at start-up.
void IR_i() {
  irrecv.enableIRIn();  // Start up the IR receiver.
  irsend.begin();       // Start up the IR sender.

  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();

  Serial.print("SmartIRRepeater is now running and waiting for IR input "
               "on Pin ");
  Serial.println(kRecvPin);
  Serial.print("and will retransmit it on Pin ");
  Serial.println(kIrLedPin);
}



// The repeating section of the code


decode_type_t protocol;
uint16_t size;
bool success;
uint16_t *raw_array;
void IR_r()

{


  // Check if an IR message has been received.
  if (irrecv.decode(&results)) {  // We have captured something.
    // The capture has stopped at this point.
    protocol = results.decode_type;
    size = results.bits;
    success = true;
    // Is it a protocol we don't understand?
    //    if (protocol == decode_type_t::UNKNOWN) {  // Yes.
    // Convert the results into an array suitable for sendRaw().
    // resultToRawArray() allocates the memory we need for the array.
    raw_array = resultToRawArray(&results);
    // Find out how many elements are in the array.
    size = getCorrectedRawLength(&results);

      Serial.println("hellow");//  }
       yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.

  irrecv.resume();




  yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.


  }
  display.clearBuffer();
  display.setCursor(2, 37);
  display.print("RAWDATA");
  display.setCursor(60, 37);
  display.print(results.value, HEX);

  display.setCursor(2, 49);
  display.print("PROTOCOL");
  display.setCursor(60, 49);
  display.print(typeToString(protocol).c_str());
  //  display.print(getProtocolString(IrReceiver.decodedIRData.protocol));
  display.sendBuffer();  // transfer internal memory to the display

  if (readButtonStateY(BTN_B)) {
    yield();
    irsend.sendRaw(raw_array, size, kFrequency);
    yield();
    // Deallocate the memory allocated by resultToRawArray().
    delete [] raw_array;
  }


  if (readButtonStateY(BTN_SEL)) {
    yield();
    if (hasACState(protocol)) {  // Does the message require a state[]?
      // It does, so send with bytes instead.
      success = irsend.send(protocol, results.state, size / 8);
    } else {  // Anything else must be a simple message protocol. ie. <= 64 bits
      success = irsend.send(protocol, results.value, size);
    }
    yield();
  }
// irrecv.resume();




  yield();  // Or delay(milliseconds); This ensures the ESP doesn't WDT reset.

}
