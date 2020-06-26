#include <Arduino.h>

#include "config.h"
#include "hw.h"

#include <multiball/app.h>
#include <multiball/wifi.h>
#include <multiball/ota_updates.h>
#include <multiball/homebus.h>

#ifdef USE_DIAGNOSTICS
#include <multiball/diagnostics.h>
#endif

#include <ESPmDNS.h>

#include <FS.h>
#include <SPIFFS.h>

#include "furball.h"
#include "diagnostics.h"

MultiballApp App;

void setup() {
  const wifi_credential_t wifi_credentials[] = {
    { WIFI_SSID1, WIFI_PASSWORD1 },
    { WIFI_SSID2, WIFI_PASSWORD2 },
    { WIFI_SSID3, WIFI_PASSWORD3 }
  };

  delay(500);

  App.wifi_credentials(3, wifi_credentials);
  App.begin("furball");
  Serial.println("[app]");

#ifdef MQTT_HOST
  Serial.println("[credentials]");
  homebus_stuff(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS, MQTT_UUID);
#endif

  Serial.println("[homebus]");
  homebus_configure("Furball", "CTRLH Electronics Lab", "Homebus", "v4");
  homebus_setup();

  furball_setup();
  Serial.println("[furball]");

#ifdef USE_DIAGNOSTICS
  diagnostics_setup();
  Serial.println("[diagnostics]");
#endif

  delay(500);
}

void loop() {
  App.handle();

  furball_loop();

#ifdef USE_DIAGNOSTICS
  diagnostics_loop(furball_stream);
#endif
}
