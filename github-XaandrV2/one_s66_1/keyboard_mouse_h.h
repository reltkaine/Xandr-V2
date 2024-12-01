//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd


void keyboard_mouse_i() {
#ifdef CONFIG_IDF_TARGET_ESP32S2
  Mouse.begin();
  Keyboard.begin();

#else
  Keyboard.begin();
#endif
}
void keyboard_mouse_u() {
  // Mouse.end()
  //Keyboard.end();
}


void mouse_r(int x, int y) {
#ifdef CONFIG_IDF_TARGET_ESP32S2

  Mouse.move(x, y);
//#else kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
//  int no = 0;
#endif
}
void kyboard(char c) {
#ifdef CONFIG_IDF_TARGET_ESP32S2
  Keyboard.write(c);

#else
  int  exit_b = 1;
  while (exit_b) {

    if (Keyboard.isConnected()) {
      Keyboard.write(c);
      exit_b = 0;
    }

  }

#endif
}

void kyboard_release() {
#ifdef CONFIG_IDF_TARGET_ESP32S2
  Keyboard.releaseAll();

#else

  int  exit_b = 1;
  while (exit_b) {

    if (Keyboard.isConnected()) {
      Keyboard.releaseAll();
      exit_b = 0;
    }

  }

#endif
}
void kyboard_print_r(String c) {
#ifdef CONFIG_IDF_TARGET_ESP32S2
  Keyboard.print(c);

#else
  int  exit_b = 1;
  while (exit_b) {
    if (Keyboard.isConnected()) {
      Keyboard.print(c);
      exit_b = 0;
    }
  }
#endif
}
void kyboard_key_r(uint8_t k) {  //KEY_LEFT_GUI KEY_LEFT_SHIFT KEY_RETURN
#ifdef CONFIG_IDF_TARGET_ESP32S2

  Keyboard.press(k);

#else
  int  exit_b = 1;
  while (exit_b) {

    if (Keyboard.isConnected()) {
      Keyboard.press(k);
      exit_b = 0;
    }

  }

#endif
}
