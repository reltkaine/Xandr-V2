//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
#include <Wire.h>
#include <PCF8574.h>


// pins for buttons:
#define BTN_SEL 4//3  //A Select button
#define BTN_A 4//3    //A Select button

#define BTN_UP 0//2    // Up
#define BTN_DOWN 1  // Down
#define BTN_ESC 6//0   ///X
#define BTN_X 6//0     ///X

#define BTN_LFT 2//6
#define BTN_RGT 3//5
#define BTN_B 5//4  ////B
#define BTN_Y 7  ////y

#define up_btn 0//2     //p2  
#define down_btn 1//1   //p1  
#define left_btn /2//6   //p6  
#define right_btn  3//5 //p5  

#define b_btn  5//4     //p4  
#define y_btn  7//7     //p7  
#define a_btn  4//3     //p3  
#define x_btn  6//0     //p0  
//#define LED_PIN LED_BUILTIN
int a_clicked = 0;

//PCF8574 pcf8574(0x20); // initialize PCF8574 with the appropriate address
unsigned long lastPressedTime[8];
bool toggleState[8];
bool ledState = false;

bool readButtonState(int buttonPin) {
  bool currentState = !pcf8574.read(buttonPin); // invert the read value since the buttons are active-low
  if (currentState != toggleState[buttonPin] && millis() - lastPressedTime[buttonPin] >= 60) {
    lastPressedTime[buttonPin] = millis();
    toggleState[buttonPin] = currentState;
    return toggleState[buttonPin];
  }
  return false;

}
bool buttonState[8];
bool IRAM_ATTR readButtonStateY(int buttonPin) {
  bool currentState = !pcf8574.read(buttonPin); // invert the read value since the buttons are active-low
  if (currentState != toggleState[buttonPin]) {
    lastPressedTime[buttonPin] = millis();
  }
  if ( (millis() - lastPressedTime[buttonPin]) >= 50) {
    if ( currentState != buttonState[buttonPin]) {
      buttonState[buttonPin] = currentState;
      if (buttonState[buttonPin]) {
        return true;
      } else {
        return false;
      }
    }
  }
  toggleState[buttonPin] = currentState;
  return false;
}


bool readButtonStateX(int buttonPin) {



  bool currentState = !pcf8574.read(buttonPin); // invert the read value since the buttons are active-low
  if (currentState != toggleState[buttonPin] ) {
    lastPressedTime[buttonPin] = millis();
  }
  if (millis() - lastPressedTime[buttonPin] >= 50) {
    return currentState;
  }
  toggleState[buttonPin] = currentState;
  return false;
}
