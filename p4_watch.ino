#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g(U8G2_R0);

#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  char timeStr[9];
  char dateStr[20];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);

  u8g.setFont(u8g_font_6x10);
  u8g.drawStr(20, 30, timeStr);
  u8g.drawStr(20, 50, dateStr);
}

void setup() {
  u8g.begin();
  u8g.setFont(u8g_font_6x10);

  // Show WiFi connecting screen
  u8g.clearBuffer();
  u8g.drawStr(25, 30, "Connecting to");
  u8g.drawStr(25, 50, "WiFi");
  u8g.sendBuffer();

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    u8g.clearBuffer();
    u8g.drawStr(25, 40, "...");
    u8g.sendBuffer();
  }

  // Display IP address
  char ipStr[20];
  snprintf(ipStr, sizeof(ipStr), "%s", WiFi.localIP().toString().c_str());

  u8g.clearBuffer();
  u8g.drawStr(10, 30, "Connected to:");
  u8g.drawStr(10, 50, ipStr);
  u8g.sendBuffer();

  delay(2000); // Hold for 2 seconds before switching to time display

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  u8g.clearBuffer();
  printLocalTime();
  u8g.sendBuffer();
  delay(1000);
}
