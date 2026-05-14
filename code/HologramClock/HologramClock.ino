/*************************************************** 
// Hologram Clock - HologramClock.ino
//
// Copyright (c) 2025, Moritz v. Sivers
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
 ****************************************************/

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>

#include <WiFiUdp.h>
#include <time.h>

const char* ntpServer = "europe.pool.ntp.org";

// Die Definition für Deutschland (Sommer-/Winterzeit automatisch)
// P1OT = Central European Time, M3.5.0 = März, letzte Woche, Sonntag
// M10.5.0 = Oktober, letzte Woche, Sonntag
const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3";

Adafruit_7segment matrix = Adafruit_7segment();

bool blinkColon = false;

// simple counter
void demo_mode() {
  for (uint16_t counter = 0; counter < 10; counter ++) {
      matrix.writeDigitNum(0, counter, false);
      matrix.writeDigitNum(1, counter, false);
      blinkColon = !blinkColon;
      matrix.drawColon(blinkColon);    
      matrix.writeDigitNum(3, counter, false);
      matrix.writeDigitNum(4, counter, false);

      matrix.writeDisplay();
      delay(1000);
  }
}

// show time
void showTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Zeit konnte nicht abgefragt werden");
    return;
  }

  uint16_t toh = timeinfo.tm_hour / 10;
  uint16_t hour = timeinfo.tm_hour % 10;
  uint16_t tom = timeinfo.tm_min / 10;
  uint16_t min = timeinfo.tm_min % 10;

  matrix.writeDigitNum(0, toh, false);
  matrix.writeDigitNum(1, hour, false);
  blinkColon = !blinkColon;
  matrix.drawColon(blinkColon);    
  matrix.writeDigitNum(3, tom, false);
  matrix.writeDigitNum(4, min, false);

  matrix.writeDisplay();

  delay(1000);
}

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("Hologram Clock");
#endif
  matrix.begin(0x70);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  //wm.resetSettings();

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect("HologramClockAP")) {
    Serial.println("failed to connect Wifi and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  // Konfiguriere die Zeit mit dem TZ-String
  configTime(0, 0, ntpServer);
  setenv("TZ", TZ_INFO, 1);
  tzset();

}

void loop() {

  // demo_mode();
  showTime();
  
}
