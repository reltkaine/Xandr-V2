//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
String wifi_SSID;
int wifi_ch;


void wifi_i() {
  // Wifi stop
  //  WiFi.disconnect(true);  // Disconnect from the network
  //  WiFi.mode(WIFI_OFF);    // Switch WiFi off

  //  delay(100);
  wifi_SSID = "";
  wifi_ch = 0;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from the network
  //esp_wifi_restore();WiFi.eraseAP();
}



void wifi_scan() {
  display.clearBuffer();
  display.drawStr(25, 15, "Scan start");
  display.sendBuffer();
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  display.drawStr(25, 15, "Scan \n done");
  if (n == 0) {
    display.drawStr(25, 30, "no networks found");
    display.sendBuffer();

  } else {
    int countw = 0;
    int exit_w = 1;

    while (exit_w) {
      if (readButtonStateY(BTN_Y)) {

        display.drawStr(25, 15, "Scan Wifi Exit");
        display.sendBuffer();
        delay(20);
        exit_w = 0;
      }

      if (readButtonStateY(BTN_SEL)) {
        countw++;

        if (n == countw) { countw = 0; }
        display.clearBuffer();
      }

      display.setCursor(0, 15);
      display.print(countw + 1);
      display.setCursor(10, 15);
      wifi_SSID = WiFi.SSID(countw);
      wifi_ch = WiFi.channel(countw);
      display.print((String)wifi_SSID);

      //      display.print((String)WiFi.SSID(countw));
      display.setCursor(8, 27);
      display.print((String)WiFi.RSSI(countw));
      display.setCursor(8, 39);
      //      display.print((String)WiFi.channel(countw));
      display.print(wifi_ch);

      switch (WiFi.encryptionType(countw)) {
        case WIFI_AUTH_OPEN:
          display.setCursor(8, 51);
          display.print("open");
          break;
        case WIFI_AUTH_WEP:
          display.setCursor(8, 51);
          display.print("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          display.setCursor(8, 51);
          display.print("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          display.setCursor(8, 51);
          display.print("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          display.setCursor(8, 51);
          display.print("WPA+WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          display.setCursor(8, 51);
          display.print("WPA2-EAP");
          break;
        case WIFI_AUTH_WPA3_PSK:
          display.setCursor(8, 51);
          display.print("WPA3");
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          display.setCursor(8, 51);
          display.print("WPA2+WPA3");
          break;
        case WIFI_AUTH_WAPI_PSK:
          display.setCursor(8, 51);
          display.print("WAPI");
          break;
        default:
          display.setCursor(8, 51);
          display.print("unknown");
      }
      display.sendBuffer();
    }
    /*


    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found

      display.setCursor(0, 20*i);display.println(String (i + 1));
      display.setCursor(20, 20*i);display.println( (String)WiFi.SSID(i));
      display.setCursor(60, 20*i);display.println( (String)WiFi.RSSI(i));
      display.setCursor(80, 20*i);display.println( (String)WiFi.channel(i));
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:
          display.setCursor(100, 20*i);display.print("open");
          break;
        case WIFI_AUTH_WEP:
          display.setCursor(100, 20*i);display.print("WEP");
          break;
        case WIFI_AUTH_WPA_PSK:
          display.setCursor(100, 20*i);display.print("WPA");
          break;
        case WIFI_AUTH_WPA2_PSK:
          display.setCursor(100, 20*i);display.print("WPA2");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          display.setCursor(100, 20*i);display.print("WPA+WPA2");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          display.setCursor(100, 20*i);display.print("WPA2-EAP");
          break;
        case WIFI_AUTH_WPA3_PSK:
          display.setCursor(100, 20*i);display.print("WPA3");
          break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
          display.setCursor(100, 20*i);display.print("WPA2+WPA3");
          break;
        case WIFI_AUTH_WAPI_PSK:
          display.setCursor(100, 20*i);display.print("WAPI");
          break;
        default:
          display.setCursor(100, 20*i);display.print("unknown");
      }
      display.sendBuffer();
      delay(10);
    }
    */
  }
  display.sendBuffer();
  Serial.println("");

  // Delete the scan result to free memory for code below.
  //WiFi.scanDelete();
}


void wifi_connect(char *passfuck) {



  /*.c_str();
*/

  WiFi.begin(wifi_SSID.c_str(), passfuck);
  // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;

  // Wait for the WiFi event
  while (true) {

    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.print("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}
