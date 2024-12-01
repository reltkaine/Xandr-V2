
/**********************************************************/
/* [+] Title:   Arduino Ducky Script Interpreter          */
/* [+] Author:  Baptiste M. (Creased)                     */
/* [+] Website: bmoine.fr                                 */
/* [+] Twitter: @Creased_                                 */
/**********************************************************/
#define KEY_MENU 0xED
#define KEY_PAUSE 0xD0
#define KEY_NUMLOCK 0xDB
#define KEY_PRINTSCREEN 0xCE
#define KEY_SCROLLLOCK 0xCF
#define KEY_SPACE 0xB4
#define KEY_BACKSPACE 0xB2
File root;  // Root directory of SD card

File f;  // f directory of SD card
String file_name;
//File selectedFile;  // Selected file
String *fileNames;      // Array to store file names
String fileo;           //payload
int selectedIndex = 0;  // Index of the selected file
int numFiles = 0;       // Number of files in the f directory
int currentPage = 0;    // Current page number
int totalPages = 0;     // Total number of pages

void processCommand(String command) {
  /*
       Process commands by pressing corresponding key
       (see https://www.arduino.cc/en/Reference/KeyboardModifiers or
            http://www.usb.org/developers/hidpage/Hut1_12v2.pdf#page=53)
  */

  if (command.length() == 1) {  // Process key (used for example for WIN L command)
    char c = (char)command[0];  // Convert string (1-char length) to char
    kyboard_key_r(c);           // Press the key on keyboard
  } else if (command == "ENTER") {
    delay(150);
    kyboard_key_r(KEY_RETURN);

  } else if (command == "MENU" || command == "APP") {
    kyboard_key_r(KEY_MENU);
  } else if (command == "DOWNARROW" || command == "DOWN") {
    kyboard_key_r(KEY_DOWN_ARROW);
  } else if (command == "LEFTARROW" || command == "LEFT") {
    kyboard_key_r(KEY_LEFT_ARROW);
  } else if (command == "RIGHTARROW" || command == "RIGHT") {
    kyboard_key_r(KEY_RIGHT_ARROW);
  } else if (command == "UPARROW" || command == "UP") {
    kyboard_key_r(KEY_UP_ARROW);
  } else if (command == "BREAK" || command == "PAUSE") {
    kyboard_key_r(KEY_PAUSE);
  } else if (command == "CAPSLOCK") {
    kyboard_key_r(KEY_CAPS_LOCK);
  } else if (command == "DELETE" || command == "DEL") {
    kyboard_key_r(KEY_DELETE);
  } else if (command == "END") {
    kyboard_key_r(KEY_END);
  } else if (command == "ESC" || command == "ESCAPE") {
    kyboard_key_r(KEY_ESC);
  } else if (command == "HOME") {
    kyboard_key_r(KEY_HOME);
  } else if (command == "INSERT") {
    kyboard_key_r(KEY_INSERT);
  } else if (command == "NUMLOCK") {
    kyboard_key_r(KEY_NUMLOCK);
  } else if (command == "PAGEUP") {
    kyboard_key_r(KEY_PAGE_UP);
  } else if (command == "PAGEDOWN") {
    kyboard_key_r(KEY_PAGE_DOWN);
  } else if (command == "PRINTSCREEN") {
    kyboard_key_r(KEY_PRINTSCREEN);
  } else if (command == "SCROLLLOCK") {
    kyboard_key_r(KEY_SCROLLLOCK);
  } else if (command == "SPACE") {
    kyboard_key_r(KEY_SPACE);
  } else if (command == "BACKSPACE") {
    kyboard_key_r(KEY_BACKSPACE);
  } else if (command == "TAB") {
    kyboard_key_r(KEY_TAB);
  } else if (command == "GUI" || command == "WINDOWS") {
    kyboard_key_r(KEY_LEFT_GUI);
  } else if (command == "SHIFT") {
    kyboard_key_r(KEY_RIGHT_SHIFT);
  } else if (command == "ALT") {
    kyboard_key_r(KEY_LEFT_ALT);
  } else if (command == "CTRL" || command == "CONTROL") {
    kyboard_key_r(KEY_LEFT_CTRL);
  } else if (command == "F1" || command == "FUNCTION1") {
    kyboard_key_r(KEY_F1);
  } else if (command == "F2" || command == "FUNCTION2") {
    kyboard_key_r(KEY_F2);
  } else if (command == "F3" || command == "FUNCTION3") {
    kyboard_key_r(KEY_F3);
  } else if (command == "F4" || command == "FUNCTION4") {
    kyboard_key_r(KEY_F4);
  } else if (command == "F5" || command == "FUNCTION5") {
    kyboard_key_r(KEY_F5);
  } else if (command == "F6" || command == "FUNCTION6") {
    kyboard_key_r(KEY_F6);
  } else if (command == "F7" || command == "FUNCTION7") {
    kyboard_key_r(KEY_F7);
  } else if (command == "F8" || command == "FUNCTION8") {
    kyboard_key_r(KEY_F8);
  } else if (command == "F9" || command == "FUNCTION9") {
    kyboard_key_r(KEY_F9);
  } else if (command == "F10" || command == "FUNCTION10") {
    kyboard_key_r(KEY_F10);
  } else if (command == "F11" || command == "FUNCTION11") {
    kyboard_key_r(KEY_F11);
  } else if (command == "F12" || command == "FUNCTION12") {
    kyboard_key_r(KEY_F12);
  }
}


void processLine(String line) {
  /*
       Process Ducky Script according to the official documentation (see https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).

       (1) Commands without payload:
        - ENTER
        - MENU <=> APP
        - DOWNARROW <=> DOWN
        - LEFTARROW <=> LEFT
        - RIGHTARROW <=> RIGHT
        - UPARROW <=> UP
        - BREAK <=> PAUSE
        - CAPSLOCK
        - DELETE
        - END
        - ESC <=> ESCAPE
        - HOME
        - INSERT
        - NUMLOCK
        - PAGEUP
        - PAGEDOWN
        - PRINTSCREEN
        - SCROLLLOCK
        - SPACE
        - TAB
        - REPLAY (global commands aren't implemented)

       (2) Commands with payload:
        - DEFAULT_DELAY <=> DEFAULTDELAY (global commands aren't implemented.)
        - DELAY (+int)
        - STRING (+string)
        - GUI <=> WINDOWS (+char)
        - SHIFT (+char or key)
        - ALT (+char or key)
        - CTRL <=> CONTROL (+char or key)
        - REM (+string)

  */

  int space = line.indexOf(' ');  // Find the first 'space' that'll be used to separate the payload from the command
  String command = "";
  String payload = "";

  if (space == -1) {  // There is no space -> (1)
    if (
      line == "ENTER" || line == "MENU" || line == "APP" || line == "DOWNARROW" || line == "DOWN" || line == "LEFTARROW" || line == "LEFT" || line == "RIGHTARROW" || line == "RIGHT" || line == "UPARROW" || line == "UP" || line == "BREAK" || line == "PAUSE" || line == "CAPSLOCK" || line == "DELETE" || line == "END" || line == "ESC" || line == "ESCAPE" || line == "HOME" || line == "INSERT" || line == "NUMLOCK" || line == "PAGEUP" || line == "PAGEDOWN" || line == "PRINTSCREEN" || line == "SCROLLLOCK" || line == "SPACE" || line == "TAB") {
      command = line;
    }
  } else {                                // Has a space -> (2)
    command = line.substring(0, space);   // Get chars in line from start to space position
    payload = line.substring(space + 1);  // Get chars in line from after space position to EOL

    if (
      command == "DELAY" || command == "STRING" || command == "GUI" || command == "WINDOWS" || command == "SHIFT" || command == "ALT" || command == "CTRL" || command == "CONTROL" || command == "REM") {
    } else {
      // Invalid command
      command = "";
      payload = "";
    }
  }

  if (payload == "" && command != "") {  // Command from (1)
    processCommand(command);             // Process command
  } else if (command == "DELAY") {       // Delay before the next commande
    delay((int)payload.toInt() + 150);   // Convert payload to integer and make pause for 'payload' time
  } else if (command == "STRING") {      // String processing
    delay(600);
    kyboard_print_r(payload);                           // Type-in the payload
  } else if (command == "REM") {                        // Comment
  } else if (command != "") {                           // Command from (2)
    String remaining = line;                            // Prepare commands to run
    while (remaining.length() > 0) {                    // For command in remaining commands
      int space = remaining.indexOf(' ');               // Find the first 'space' that'll be used to separate commands
      if (space != -1) {                                // If this isn't the last command
        processCommand(remaining.substring(0, space));  // Process command
        remaining = remaining.substring(space + 1);     // Pop command from remaining commands
      } else {                                          // If this is the last command
        processCommand(remaining);                      // Pop command from remaining commands
        remaining = "";                                 // Clear commands (end of loop)
      }
    }
  } else {
    Serial.println("error line");
  }
  kyboard_release();
  //  Keyboard.releaseAll();
}








void run_ducky_payload(fs::FS &fs, String payload) {
  f = fs.open(payload);
  if (!f) {
    Serial.println("Failed to open file for reading");
    delay(100);
  }
  keyboard_mouse_i();
  String line = "";
  int error_count_TX = 0;
  while (f.available()) {
    char c = f.read();
    //    float curr_time_exit = millis();
    // Process char
    if ((int)c == 0x0a) {  // Line ending (LF) reachedFFFFFFFFFFFFFFFF
      Serial.println(line);
      delay(30);
      if (wireless_DS) {
        Serial.println("nrf24####");
        nrf24_string(String(line));  ///send to nrf24
        delay(200);
        Serial.println(line);
        int z = 1;
        float curr_time_exit = millis();
        while (z) {
          nrf24_r();
          if (textbb[0] == '>') {
            z = 0;
            textbb[0] = '\0';
          }
          if ((millis() - curr_time_exit) >= 50)  //////avoid stuck when disconnect or unresponsie
          {
            z = 0;
            error_count_TX++;
            Serial.print("error_count_TX : ");
            Serial.print(error_count_TX);
            Serial.println(" NO RESPONSE :");
          }
        }
      } else {
        processLine(line);  // Process script line by reading command and payload
      }
      line = "";                  // Clean the line to process next
    } else if ((int)c != 0x0d) {  // If char isn't a carriage return (CR)
      line += c;                  // Put char into line
    }
  }

  if (wireless_DS)  //////end of script
  {
    nrf24_string("(");  ////end trnsmitttt
  }

  f.close();
  Serial.println("RELEASE ALLL :");
}


void process_wireless_rec() {
  bool f = 1;
  float curr_time_exit = millis();
  while (f) {
    curr_time_exit = millis();
    if (nrf24_r()) {

      if (textbb[0] == '(')  ////end trnsmitttt
      {
        curr_time_exit = millis();
        Serial.println("Recieve : TERminate");


        f = 0;
        textbb[0] = '\0';
      }
      if (textbb[0] != '\0') {
        curr_time_exit = millis();
        String lineS = String(textbb);
        Serial.println("Recieve : ");
        Serial.print(lineS);
        processLine(lineS);  // Process script line by reading command and payload
        display.clearBuffer();
        display.drawStr(2, 12, "COM NRF24 ");
        display.drawStr(2, 30, textbb);
        display.sendBuffer();

        textbb[0] = '\0';
        nrf24_string(">");
      }
      ///NRFSEND ASK NEXT
      Serial.println("RELEASE ALLL :");
    }
    if ((millis() - curr_time_exit) >= 2000)  //////avoid stuck when disconnect or unresponsie
    {
      f = 0;
      Serial.println("connection drop/unresponsie");
    }
  }
}





void run_payload(fs::FS &fs, String payload /*char *payload*/) {
  f = fs.open(payload);
  if (!f) {
    Serial.println("Failed to open file for reading");
    return;
  }
  keyboard_mouse_i();
  String line = "";
  while (f.available()) {
    char c = f.read();

    // Process char
    if ((int)c == 0x0a) {  // Line ending (LF) reached
      Serial.println(line);
      delay(30);
      processLine(line);  // Process script line by reading command and payload

      line = "";                  // Clean the line to process next
    } else if ((int)c != 0x0d) {  // If char isn't a carriage return (CR)
      line += c;                  // Put char into line
    }
  }

  f.close();
  Serial.println("RELEASE ALLL :");
}



void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void sdcard_r() {
  numFiles = 0;

  display.clearBuffer();  // clear the internal memory
  display.setCursor(0, 15);
  display.print("test");
  //    display.sendBuffer();          // transfer internal memory to the display
  pinMode(PIN_SPI_SD_CS, OUTPUT);

  sd_spi.begin(PIN_SPI_SD_CLK, PIN_SPI_SD_MISO, PIN_SPI_SD_MOSI, PIN_SPI_SD_CS);


  //pinMode(PIN_SPI_SD_CS, OUTPUT);
  //   display.clearBuffer();
  //   display.setCursor(0, 15);
  display.println("SDbegin");
  //    display.sendBuffer();

  if (!SD.begin(PIN_SPI_SD_CS, sd_spi)) {
    //       display.clearBuffer();
    //       display.setCursor(0, 15);
    display.println("Failed");
    display.sendBuffer();
    //    return proceed;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    //       display.clearBuffer();
    //       display.setCursor(0, 15);
    // display.println("No SD card attached");
    // display.sendBuffer();
    //    return proceed;  /////////////////////
  }

  //    display.clearBuffer();
  //    display.setCursor(0, 15);
  //display.println("SD Card Type: ");
  //    display.sendBuffer();
  if (cardType == CARD_MMC) {
    //    display.clearBuffer();
    //    display.setCursor(0, 15);
    //display.println("MMC");
    //    display.sendBuffer();

  } else if (cardType == CARD_SD) {
    //      display.clearBuffer();
    //      display.setCursor(0, 15);
    //display.println("SDSC");
    //      display.sendBuffer();

  } else if (cardType == CARD_SDHC) {
    //     display.clearBuffer();
    //     display.setCursor(0, 15);
    //display.println("SDHC");
    //     display.sendBuffer();

  } else {
    //      display.clearBuffer();
    //      display.setCursor(0, 15);
    //display.println("UNKNOWN");
    //      display.sendBuffer();
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  //    display.clearBuffer();
  //    display.setCursor(0, 15);
  display.println("Size:");
  display.println(String(cardSize));
  display.sendBuffer();
}





int startIndex = 0;






///////////////////////////////////////////////////////
void displayFileList() {
  // Clear the display
  display.clearBuffer();
  //  display.setFont(u8g2_font_ncenB14_tr);

  // Display the file list
  int y = 12;


  //numFiles
  for (int i = 0; i <= 3; i++) {
    int indexa = startIndex + i;
    if (indexa >= numFiles) break;

    if (indexa == selectedIndex) {
      display.drawBox(0, y - 12, 128, 12);  // Highlight the selected file
      display.setDrawColor(0);              // Set the text color to black for the selected file
    }
    display.setCursor(0, y);
    display.print(fileNames[indexa]);
    y += 12;
    display.setDrawColor(1);  // Set the text color to white for the rest of the files
    Serial.println("index");
    Serial.println(indexa);
  }

  // Send the buffer to the display
  display.sendBuffer();
}


void sdcard_s() {
  sd_spi.begin(PIN_SPI_SD_CLK, PIN_SPI_SD_MISO, PIN_SPI_SD_MOSI, PIN_SPI_SD_CS);
  if (!SD.begin(PIN_SPI_SD_CS, sd_spi)) {
    display.clearBuffer();
    display.drawStr(0, 16, "SD Card Error");
    Serial.println("Failed SD sdcard_s()");
    display.sendBuffer();
    while (1)
      ;  // Halt the program if SD card initialization fails
  }

  // Open the f directory
  f = SD.open("/");

  // Count the number of files in the f directory
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }
    numFiles++;
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(numFiles);
    display.sendBuffer();

    entry.close();
  }

  // Close the f directory
  f.close();
  // Allocate memory for file names array
  fileNames = new String[numFiles];

  // Open the f directory again to populate file names array
  f = SD.open("/");
  int i = 0;
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(i);
    display.sendBuffer();

    fileNames[i] = entry.name();
    entry.close();
    i++;
  }

  // Close the f directory
  f.close();

  displayFileList();
}

void sdcard_s_folder(String Folder_target) {
  sd_spi.begin(PIN_SPI_SD_CLK, PIN_SPI_SD_MISO, PIN_SPI_SD_MOSI, PIN_SPI_SD_CS);
  if (!SD.begin(PIN_SPI_SD_CS, sd_spi)) {
    display.clearBuffer();
    display.drawStr(0, 16, "SD Card Error");
    display.sendBuffer();
    while (1)
      ;  // Halt the program if SD card initialization fails
  }

  // Open the f directory
  f = SD.open("/" + Folder_target);

  // Count the number of files in the f directory
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }
    numFiles++;
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(numFiles);
    display.sendBuffer();

    entry.close();
  }

  // Close the f directory
  f.close();
  // Allocate memory for file names array
  fileNames = new String[numFiles];

  // Open the f directory again to populate file names array
  f = SD.open("/" + Folder_target);
  int i = 0;
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(i);
    display.sendBuffer();

    fileNames[i] = entry.name();
    entry.close();
    i++;
  }

  // Close the f directory
  f.close();

  displayFileList();
}







int count_line_script;



void performAction() {
  String h = fileNames[selectedIndex].c_str();
  fileo = "/" + h;
  //int lawak = fileo.length();

  //fileo.toCharArray(file_array, lawak + 1);
  Serial.println("performAction :" + fileo);
  f = SD.open(fileo);
  Serial.println(fileNames[selectedIndex].c_str());

  if (!f) {
    Serial.println("Failed to open file for reading");
    delay(100);
  }

  // Read and display the content of the selected file
  count_line_script = 0;
  while (f.available()) {
    display.clearBuffer();
    //      display.setFont(u8g2_font_ncenB14_tr);
    display.setCursor(0, 16);
    display.print("Selected file:");
    display.setCursor(0, 32);
    display.print(fileNames[selectedIndex]);
    display.sendBuffer();
    String sut = f.readStringUntil('\n');
    count_line_script++;

    Serial.println(sut);
    delay(10);
  }

  // Wait for the user to release the button
  //    while (readButtonStateY(BTN_A));

  // Close the selected file and f directory
  f.close();
}


void performAction_Folder(String Folder) {
  String h = fileNames[selectedIndex].c_str();
  fileo = "/" + Folder + "/" + h;
  //int lawak = fileo.length();

  //fileo.toCharArray(file_array, lawak + 1);
  Serial.println("performAction :" + fileo);
  f = SD.open(fileo);
  Serial.println(fileNames[selectedIndex].c_str());

  if (!f) {
    Serial.println("Failed to open file for reading");
    delay(100);
  }

  // Read and display the content of the selected file
  count_line_script = 0;
  while (f.available()) {
    display.clearBuffer();
    //      display.setFont(u8g2_font_ncenB14_tr);
    display.setCursor(0, 16);
    display.print("Selected file:");
    display.setCursor(0, 32);
    display.print(fileNames[selectedIndex]);
    display.sendBuffer();
    String sut = f.readStringUntil('\n');
    count_line_script++;

    Serial.println(sut);
    delay(10);
  }

  // Wait for the user to release the button
  //    while (readButtonStateY(BTN_A));

  // Close the selected file and f directory
  f.close();
}


void target_line(int x) {
  String h = fileNames[selectedIndex].c_str();
  fileo = "/" + h;
  //int lawak = fileo.length();

  //fileo.toCharArray(file_array, lawak + 1);
  f = SD.open(fileo);
  Serial.println(fileNames[selectedIndex].c_str());

  if (!f) {
    Serial.println("Failed to open file for reading");
    delay(100);
  }

  keyboard_mouse_i();

  // Read and display the content of the selected file
  count_line_script = 0;
  String line = "";
  while (f.available()) {
    display.clearBuffer();
    //      display.setFont(u8g2_font_ncenB14_tr);
    display.setCursor(0, 16);
    display.print("Selected file:");
    display.setCursor(0, 32);
    display.print(fileNames[selectedIndex]);
    display.sendBuffer();
    String sut = f.readStringUntil('\n');

    count_line_script++;

    if (count_line_script == x) {

      f.seek(f.position() - 1);
      while (true) {
        char c = f.read();
        if ((int)c == 0x0a || c == -1) {  // Line ending (LF) reachedFFFFFFFFFFFFFFFF
          Serial.println(line);
          nrf24_string(String(line));
          break;
        } else if ((int)c != 0x0d) {  // If char isn't a carriage return (CR)
          line += c;                  // Put char into line
        }
      }
      Serial.println(sut);
      delay(10);
    }

    // Wait for the user to release the button
    //    while (readButtonStateY(BTN_A));

    // Close the selected file and f directory
    f.close();
  }
}


void read_string_file(fs::FS &fs, String payload) {
  f = fs.open(payload);
  if (!f) {
    Serial.println("Failed to open file for reading");
    delay(100);
  }

  while (f.available()) {
    html_content = f.readString();
    Serial.println(html_content);
  }

  f.close();
}



int sdcard_sel() {
  //char file_array[] = "";
  int rerun = 0;
  fileo = "";


  if (readButtonStateY(BTN_LFT) || readButtonStateY(BTN_DOWN)) {
    selectedIndex = (selectedIndex - 1 + numFiles) % numFiles;  // Move to the previous file
    Serial.println("LFT");
    Serial.println("LFT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);

    displayFileList();
  } else if (readButtonStateY(BTN_RGT) || readButtonStateY(BTN_UP)) {
    selectedIndex = (selectedIndex + 1) % numFiles;  // Move to the next file
    Serial.println("RGT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);

    displayFileList();
  }

  else if (readButtonStateY(BTN_SEL)) {
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 15, "Ducky Start");
    display.drawStr(25, 24, "Ducky Unload");

    display.sendBuffer();  // transfer internal memory to the display

    // Open the selected file
    Serial.println("SEL");
    wireless_DS = 0;

    performAction();
    Serial.print("Number of line : ");
    Serial.println(count_line_script);
    rerun = 1;
  }
/*
  else if (readButtonStateY(BTN_B)) {

    Serial.println("B");
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(15, 15, "Ducky Start NRF");
    display.drawStr(15, 24, "Ducky Unload NRF");
    display.sendBuffer();  // transfer internal memory to the display

    wireless_DS = 1;
    delay(10);
    if (wireless_DS) {
      nrf24_string("DS");
      Serial.println("send DS.......B");
    }

    performAction();
    Serial.print("Number of line : ");
    Serial.println(count_line_script);
    rerun = 1;
  }

*/

  //startIndex = selectedIndex - (selectedIndex % 4);


  return rerun;
}

int sdcard_sel_Folder(String Folder_x) {
  //char file_array[] = "";
  int rerun = 0;
  fileo = "";


  if (readButtonStateY(BTN_LFT) || readButtonStateY(BTN_DOWN)) {
    selectedIndex = (selectedIndex - 1 + numFiles) % numFiles;  // Move to the previous file
    Serial.println("LFT");
    Serial.println("LFT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);

    displayFileList();
  } else if (readButtonStateY(BTN_RGT) || readButtonStateY(BTN_UP)) {
    selectedIndex = (selectedIndex + 1) % numFiles;  // Move to the next file
    Serial.println("RGT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);

    displayFileList();
  }

  else if (readButtonStateY(BTN_SEL)) {
    // Open the selected file
    Serial.println("SEL");

    performAction_Folder(Folder_x);
    rerun = 1;
  }
  return rerun;
}


int sdcard_B() {
  //char file_array[] = "";
  int rerun = 0;
  fileo = "";


  if (readButtonStateY(BTN_LFT) || readButtonStateY(BTN_DOWN)) {
    selectedIndex = (selectedIndex - 1 + numFiles) % numFiles;  // Move to the previous file
    Serial.println("LFT");
    Serial.println("LFT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);

    displayFileList();
  } else if (readButtonStateY(BTN_RGT) || readButtonStateY(BTN_UP)) {
    selectedIndex = (selectedIndex + 1) % numFiles;  // Move to the next file
    Serial.println("RGT");
    Serial.println(selectedIndex);
    Serial.println(numFiles);
    Serial.println(fileNames[selectedIndex].c_str());
    startIndex = selectedIndex - (selectedIndex % 4);
    displayFileList();
  }

  else if (readButtonStateY(BTN_SEL)) {
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 15, "Ducky Start");
    display.drawStr(25, 24, "Ducky Unload");

    display.sendBuffer();  // transfer internal memory to the display

    // Open the selected file
    Serial.println("SEL");
    wireless_DS = 0;

    performAction();
    Serial.print("Number of line : ");
    Serial.println(count_line_script);
    nrf24_string("DS");
    while (!nrf24_r()) {
      if (textbb[0] == '>') {
        nrf24_string(String(count_line_script));
      } else {
        target_line(atoi(textbb));
      }
    }


    rerun = 1;
  }
  return rerun;
}

void open_file(fs::FS &fs, String extension_file, String folder_in) {
  int i = 0;
  do {
    file_name = "/" + folder_in + "/" + (String)i + "." + extension_file;
    i++;
  } while (fs.exists(file_name));

  Serial.println(file_name);

  f = fs.open(file_name, FILE_WRITE);
  f.close();
}

void save_file(fs::FS &fs, String file_save) {
  Serial.println("saving file");

  uint32_t startTime = millis();
  uint32_t finishTime;
  Serial.println("SAVE in" + file_name + "'");

  f = fs.open(file_name, FILE_APPEND);
  if (!f) {
    Serial.println("Failed to open file '" + file_name + "'");
    return;
  }

  f.print(file_save);

  f.close();

  finishTime = millis() - startTime;

  //  Serial.printf("\n%u bytes written for %u ms\n", finishTime);
}



void sdcard_s_ex(String extension_name, String Folder_save) {
  sd_spi.begin(PIN_SPI_SD_CLK, PIN_SPI_SD_MISO, PIN_SPI_SD_MOSI, PIN_SPI_SD_CS);
  if (!SD.begin(PIN_SPI_SD_CS, sd_spi)) {
    display.clearBuffer();
    display.drawStr(0, 16, "SD Card Error");
    display.sendBuffer();
    while (1)
      ;  // Halt the program if SD card initialization fails
  }

  // Open the f directory
  f = SD.open("/" + Folder_save);

  // Count the number of files in the f directory
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }

    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(numFiles);
    display.sendBuffer();





    String exname;
    String exname_ex;
    int index_exname;
    int length_exname;

    exname = entry.name();
    Serial.println(exname);
    index_exname = exname.indexOf(".");
    Serial.println(index_exname);
    length_exname = exname.length();
    Serial.println(length_exname);
    exname_ex = exname.substring(index_exname, length_exname);
    Serial.println(exname_ex);
    if (exname_ex == extension_name) {


      numFiles++;
    }
    entry.close();
  }

  // Close the f directory
  f.close();
  // Allocate memory for file names array
  fileNames = new String[numFiles];

  // Open the f directory again to populate file names array
  f = SD.open("/" + Folder_save);
  int i = 0;
  while (true) {
    File entry = f.openNextFile();
    if (!entry) {
      // No more files in the f directory
      break;
    }
    display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
    display.drawStr(25, 40, "File#");
    display.setCursor(50, 45);
    display.print(i);
    display.sendBuffer();

    String exname;
    String exname_ex;
    int index_exname;
    int length_exname;

    exname = entry.name();
    Serial.println(exname);
    index_exname = exname.indexOf(".");
    Serial.println(index_exname);
    length_exname = exname.length();
    Serial.println(length_exname);
    exname_ex = exname.substring(index_exname, length_exname);
    Serial.println(exname_ex);

    if (exname_ex == extension_name) {
      fileNames[i] = entry.name();


      i++;
    }
    entry.close();
  }

  // Close the f directory
  f.close();

  displayFileList();
}
