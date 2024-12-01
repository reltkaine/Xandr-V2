//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
const char Mobile_KB[4][10] = {
  { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' },
  { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' },
  { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';' },
  { 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/' }
};
const char Mobile_KB_1[4][10] = {
  { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')' },
  { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' },
  { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':' },
  { 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?' }
};

const char Mobile_NumKeys[4][10] = {
  { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' },
  { '-', '=', '[', ']', '\\', ';', '\'', ',', '.', '/' },
  { '_', '+', '{', '}', '|', ':', '\"', '>', '?', '`' },
  { ' ', ' ', ' ', ' ', '#', '%', '^', '*', '+', '=' }
};

const char Mobile_SymKeys[3][10] = {
  { '[', ']', '{', '}', '#', '%', '^', '*', '+', '=' },
  { '_', '\\', '|', '~', '<', '>' },  //4
  { '.', ',', '?', '!', '\'' }
};


int keycap = 1;
int curx = 3;
int cury = 2;
char Keybuffer[32];
int keycounter = 0;
char selkey[1];


void draw_key() {
  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 9; x++) {
      char bug[1];
      //bug[0]=Mobile_SymKeys[y][x];
      //bug[0]=pgm_read_byte(&(Mobile_KB[y][x]));
      if (keycap == 0) {
        bug[0] = Mobile_KB[y][x];
      }
      if (keycap == 1) {
        bug[0] = Mobile_KB_1[y][x];
      }
      if (keycap == 2) {
        bug[0] = Mobile_NumKeys[y][x];
      }
      display.drawStr(25 + 5 + (fontw * x), 27 + (fonth * y), bug);  // write something to the internal memory
    }
  }
}



int draw_cur() {
  int escape = 1;
  //selkey[0]=pgm_read_byte(&(Mobile_KB[cury][curx]));



  if (keycap == 0) {
    selkey[0] = Mobile_KB[cury][curx];
  }
  if (keycap == 1) {
    selkey[0] = Mobile_KB_1[cury][curx];
  }
  if (keycap == 2) {
    selkey[0] = Mobile_NumKeys[cury][curx];
  }


  //-A
  if (readButtonStateY(BTN_SEL)) {
    if (keycounter == 16) {
      keycounter = 15;
    }
    Keybuffer[keycounter] = selkey[0];
    keycounter++;
  }
  //-UP
  if (readButtonStateY(BTN_UP)) {
    cury--;
    if (cury == -1) {
      cury = 3;
    }
  }
  //-DOWN
  if (readButtonStateY(BTN_DOWN)) {
    cury++;
    if (cury == 4) {
      cury = 0;
    }
  }

  //-B
  if (readButtonStateY(BTN_B)) {
    if (keycounter > 0) {
      keycounter--;
      Keybuffer[keycounter] = ' ';  //\0
      Keybuffer[keycounter] = '\0';
    }
    if (keycounter < 0) {
      keycounter = 0;

      Keybuffer[keycounter] = ' ';  //\0
      Keybuffer[keycounter] = '\0';
    }
  }
  //-X
  if (readButtonStateY(BTN_ESC)) {
    keycap++;
    if (keycap == 3) {
      keycap = 0;
    }
  }

  //-Y
  if (readButtonStateY(BTN_Y)) {
    escape = 0;
  }
  //-LEFT
  if (readButtonStateY(BTN_LFT)) {
    curx--;
    if (curx == -1) {
      curx = 9;
    }
  }
  //-RIGHT
  if (readButtonStateY(BTN_RGT)) {
    curx++;
    if (curx == 10) {
      curx = 0;
    }
  }

  display.drawFrame(25 + 3 + (fontw * curx), 27 + (fonth * (cury - 1)), fontw + 3, fonth + 2);
  //u8g2.drawFrame(3+(fontw*3), 15+(fonth*(2-1)), 7, 12);

  return escape;
}


void run_key() {
  int mexit;
  mexit = 1;
  curx = 3;
  cury = 2;

  while (mexit) {
    display.firstPage();
    do {
      display.clearBuffer();      // clear the internal memory
      display.setFont(fontName);  // choose a suitable font
      draw_key();
      mexit = draw_cur();
      display.drawStr(0, 21, selkey);
      display.drawStr(0, 15, Keybuffer);
      display.sendBuffer();  // transfer internal memory to the display
    } while (display.nextPage());
  }
}
