#include <Arduino.h>

#include "config.h"
#include "hw.h"

#include <multiball/wifi.h>
#include <multiball/ota_updates.h>
#include <multiball/mqtt.h>

#include <ESPmDNS.h>

#include <FS.h>
#include <SPIFFS.h>

#include "sensors/bme680_sensor.h"
#include "sensors/tsl2561_sensor.h"
#include "sensors/pms_sensor.h"

#include "sensors/pir_sensor.h"
#include "sensors/sound_level_sensor.h"

#include "sensors/uptime.h"

Uptime uptime;

BME680_Sensor bme680(UPDATE_DELAY, 0, 0, false);
TSL2561_Sensor tsl2561(UPDATE_DELAY, 0, 0, false);
PMS_Sensor pms5003(UPDATE_DELAY, 0, 0, false);

PIR_Sensor pir(1, UPDATE_DELAY, 0, 0, false);
SoundLevel_Sensor sound_level(SOUND_PIN, UPDATE_DELAY, 0, 0, false);

#include "indicator.h"

// allocate enough space for the prefix and the UUID
#define MAX_UUID_LENGTH 37
#define HOMEBUS_ENDPOINT_LENGTH sizeof("/homebus/device/") + MAX_UUID_LENGTH
static char homebus_endpoint[HOMEBUS_ENDPOINT_LENGTH + 1];


#ifdef BUILD_INFO

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

static char build_info[] = STRINGIZE(BUILD_INFO);
#else
static char build_info[] = "not set";
#endif

static RTC_DATA_ATTR int bootCount = 0;
static RTC_DATA_ATTR int wifi_failures = 0;

void setup() {
  const char* hostname;
  const char *wifi_credentials[] = {
  WIFI_SSID1, WIFI_PASSWORD1,
  WIFI_SSID2, WIFI_PASSWORD2,
  WIFI_SSID3, WIFI_PASSWORD3
  };

  byte mac_address[6];

  bootCount++;

  delay(500);

  Serial.begin(115200);
  Serial.println("Hello World!");
  Serial.printf("Build %s\n", build_info);

  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    }

  if(wifi_begin(wifi_credentials, 3, "furball")) {
    Serial.println(WiFi.localIP());
    Serial.println("[wifi]");

    hostname = wifi_hostname();

    if(!MDNS.begin(hostname))
      Serial.println("Error setting up MDNS responder!");
    else
      Serial.println("[mDNS]");

  } else {
    Serial.println("wifi failure");
  }

  ota_updates_setup();
  Serial.println("[ota_updates]");

  if(!MDNS.begin(hostname))
    Serial.println("Error setting up MDNS responder!");
  else
    Serial.println("[mDNS]");

  mqtt_setup(MQTT_HOST, MQTT_PORT, MQTT_UUID, MQTT_USER, MQTT_PASS);
  Serial.println("[mqtt]");

  snprintf(homebus_endpoint, HOMEBUS_ENDPOINT_LENGTH, "/homebus/device/%s", MQTT_UUID);
  Serial.printf("Homebus endpoint: %s\n", homebus_endpoint);
  Serial.println("[homebus]");

  bme680.begin();
  Serial.println("[bme680]");

  tsl2561.begin();
  Serial.println("[tsl2561]");

#ifndef NO_PMS
  pms5003.begin(Serial1);
  Serial.println("[pms5003]");
#endif

  pir.begin();
  Serial.println("[pir]");

  sound_level.begin();
  Serial.println("[sound pressure]");

  indicator_begin();
  Serial.println("[indicator]");

  delay(500);
}

void loop() {
  static unsigned long next_loop = 0;

  wifi_handle();

  ota_updates_handle();

  mqtt_handle();

  indicator_loop();

  if(next_loop > millis())
    return;

  next_loop = millis() + UPDATE_DELAY;

  bme680.handle();
  tsl2561.handle();
#ifndef NO_PMS
  pms5003.handle();
#endif

  sound_level.handle();
  pir.handle();

  if(bme680.ready_for_update()) {
#ifdef VERBOSE
    Serial.printf("Temperature %0.1f\n", bme680.temperature());
    Serial.printf("Pressure %0.1f\n", bme680.pressure());
    Serial.printf("Humidity %0.1f\n", bme680.humidity());
#endif
  }

  if(tsl2561.ready_for_update()) {
#ifdef VERBOSE
    Serial.printf("IR %d\n", tsl2561.ir());
    Serial.printf("Visible %d\n", tsl2561.visible());
    Serial.printf("Full %d\n", tsl2561.full());
    Serial.printf("Lux %d\n", tsl2561.lux());
#endif
  }

#ifndef NO_PMS
  if(pms5003.ready_for_update()) {
#ifdef VERBOSE
    Serial.printf("PMS5003 1.0 %d\n", pms5003.density_1_0());
    Serial.printf("PMS5003 2.5 %d\n", pms5003.density_2_5());
    Serial.printf("PMS5003 10.0 %d\n", pms5003.density_10_0());
#endif
  }
#endif

  if(sound_level.ready_for_update()) {
#ifdef VERBOSE
    Serial.printf("Sound average %u\n", sound_level.sound_level());
    Serial.printf("Sound max %u\n", sound_level.sound_max());
    Serial.printf("Sound min %u\n", sound_level.sound_min());
    Serial.printf("Sound samples %u\n", sound_level.sample_count());
#endif
  }

#ifdef VERBOSE
  Serial.printf("Uptime %.2f seconds\n", uptime.uptime() / 1000.0);
  Serial.printf("Free heap %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Wifi RSSI %d\n", WiFi.RSSI());
#endif

  uint16_t pm1 = pms5003.density_1_0();
  uint16_t pm25 = pms5003.density_2_5();
  uint16_t pm10 = pms5003.density_10_0();

  if(pm1 > 10000 && uptime.uptime() < 60*1000)
    pm1 = 0;

  if(pm25 > 10000 && uptime.uptime() < 60*1000)
    pm25 = 0;

  if(pm10 > 10000 && uptime.uptime() < 60*1000)
    pm10 = 0;

  char buffer[700];
  IPAddress local = WiFi.localIP();
  snprintf(buffer, 700, "{ \"id\": \"%s\", \"system\": { \"name\": \"%s\", \"build\": \"%s\", \"freeheap\": %d, \"uptime\": %lu, \"ip\": \"%d.%d.%d.%d\", \"rssi\": %d, \"reboots\": %d, \"wifi_failures\": %d }, \"environment\": { \"temperature_adjusted\": true, \"temperature\": %.1f, \"humidity\": %.1f, \"pressure\": %.1f }, \"air\": {  \"tvoc\": %0.2f, \"pm1\": %d, \"pm25\": %d, \"pm10\": %d }, \"light\": {  \"lux\": %d, \"full_light\": %d, \"ir\": %d, \"visible\": %d }, \"sound\": { \"average\": %d, \"min\": %d, \"max\": %d, \"samples\": %d, \"squared\": true }, \"presence\": %s }",
	   MQTT_UUID,
	   wifi_hostname(), build_info, ESP.getFreeHeap(), uptime.uptime()/1000, local[0], local[1], local[2], local[3], WiFi.RSSI(), bootCount, wifi_failures,
#ifdef TEMPERATURE_ADJUSTMENT
	   bme680.temperature() + TEMPERATURE_ADJUSTMENT,
#else
	   bme680.temperature(),
#endif

 bme680.humidity(), bme680.pressure(),
	   bme680.gas_resistance(), 
#ifdef NO_PMS
	   0, 0, 0,
#else
	   pm1, pm25, pm10,
#endif
	   tsl2561.lux(), tsl2561.full(), tsl2561.ir(), tsl2561.visible(),
	   sound_level.sound_level(), sound_level.sound_min(), sound_level.sound_max(), sound_level.sample_count(),
	   pir.presence() ? "true" : "false");

#ifdef VERBOSE
    Serial.println(buffer);
#endif
    mqtt_publish(homebus_endpoint, buffer, true);
}
