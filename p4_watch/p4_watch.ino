#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <U8g2lib.h>
#include <stdio.h>
#include <string.h>
#include "FS.h"
#include "SPIFFS.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g(U8G2_R0);

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

#define BUTTON_PIN 4

bool showClock = true;

/**
 * @brief Prints the calendar events from a CSV file to the OLED screen.
 * This function opens a file named "calendar.csv", reads each line,
 * parses the event name and time, and displays them. It assumes the
 * file is formatted as: "Event Name, Time," per line.
 */
void printCalendar() {
  u8g.setFont(u8g_font_6x10);
  int yPos = 10; // Starting Y-position for the first line

  FILE* file = fopen("/calendar.txt", "r");
  if (file == NULL) {
    u8g.drawStr(10, 30, "Error opening file!");
    return;
  }

  char line[256];
  u8g.drawStr(10, yPos, "Today's Schedule:");
  yPos += 12; // Move to the next line

  while (fgets(line, sizeof(line), file)) {
    char* eventName = strtok(line, ",");
    char* eventTime = strtok(NULL, ",");

    if (eventName != NULL && eventTime != NULL) {
      u8g.drawStr(10, yPos, eventName);
      u8g.drawStr(90, yPos, eventTime);
      yPos += 10;
    }
  }

  fclose(file);
}

/**
 * @brief Prints the current local time and date to the OLED screen.
 * This function retrieves the time from the NTP server and formats it
 * for display.
 */
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    u8g.drawStr(10, 30, "Failed to get time");
    return;
  }

  char timeStr[9];
  char dateStr[20];
  
  // Format the time and date strings
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  strftime(dateStr, sizeof(dateStr), "%a %d %b %Y", &timeinfo);

  u8g.setFont(u8g_font_6x10);
  u8g.drawStr(20, 30, "Current Time:");
  u8g.drawStr(20, 42, timeStr);
  u8g.drawStr(20, 54, dateStr);
}

void setup() {
  u8g.begin();
  u8g.setFont(u8g_font_6x10);
  Serial.begin(115200);

  [fs]
  files = ["calendar.txt"]

  // Set the button pin as an input with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Show WiFi connecting screen
  u8g.clearBuffer();
  u8g.drawStr(25, 30, "Connecting to");
  u8g.drawStr(25, 50, "WiFi");
  u8g.sendBuffer();

  // The Wokwi-GUEST network is pre-configured to work in the simulator
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

  // Configure NTP time synchronization
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  u8g.clearBuffer();

  static int lastButtonState = HIGH;
  
  // Read the current state of the button
  int currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    Serial.println("Button pressed.");
    showClock = !showClock;
  }

  lastButtonState = currentButtonState;

  if (showClock) {
    printLocalTime();
  } else {
    printCalendar();
  }

  u8g.sendBuffer();
  delay(50);
}
