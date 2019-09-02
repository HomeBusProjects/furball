#include "indicator.h"
#include "hw.h"

#include <WiFi.h>
#include <Ticker.h>
#include <PubSubClient.h>

#define INDICATOR_INTERVAL 0.5

// don't do this yet
// #include <FastLED.h>

static Ticker indicator_ticker;

extern PubSubClient mqtt_client;

static bool indicator_flash = true;
static bool indicator_on = true;

void indicator_set_color(uint8_t red, uint8_t green, uint8_t blue) {
  ledcWrite(0, red);
  ledcWrite(1, green);
  ledcWrite(2, blue);
}

void indicator_loop() {
  if(WiFi.status() == WL_CONNECTED && mqtt_client.connected()) {
    indicator_set_color(0x00, 0xff, 0x00);
    indicator_flash = false;
    return;
  }

  indicator_flash = true;
  if(WiFi.status() != WL_CONNECTED) {
    indicator_set_color(255, 0, 0);
    return;
  }

  indicator_set_color(255, 194, 0);
}

void indicator_callback() {
  if(!indicator_flash)
    return;

  if(indicator_on) {
    indicator_on = false;
    indicator_set_color(0, 0, 0);
    return;
  }

  indicator_on = true;
  indicator_loop();    
}

void indicator_begin() {
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);

  ledcAttachPin(LED_RED, 0);
  ledcAttachPin(LED_GREEN, 1);
  ledcAttachPin(LED_BLUE, 2);

  indicator_ticker.attach(INDICATOR_INTERVAL, indicator_callback);
}
