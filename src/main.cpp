#include <Arduino.h>

#include <Esp.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <FS.h>
#include <SPIFFS.h>

#include "config.h"
#include "hw.h"

#include <ArduinoOTA.h>

#include <PubSubClient.h>
static WiFiClient wifi_mqtt_client;
PubSubClient mqtt_client(wifi_mqtt_client);

#include "sensors/bme680_sensor.h"
#include "sensors/tsl2561_sensor.h"
#include "sensors/pms_sensor.h"

#include "sensors/pir_sensor.h"
#include "sensors/sound_level_sensor.h"

BME680_Sensor bme680(UPDATE_DELAY, 0, 0, false);
TSL2561_Sensor tsl2561(UPDATE_DELAY, 0, 0, false);
PMS_Sensor pms5003(UPDATE_DELAY, 0, 0, false);

PIR_Sensor pir(1, UPDATE_DELAY, 0, 0, false);
SoundLevel_Sensor sound_level(SOUND_PIN, UPDATE_DELAY, 0, 0, false);

#include "indicator.h"


#include "sensors/uptime.h"

Uptime uptime;


WiFiMulti wifiMulti;

#include <rom/rtc.h>

const char* reboot_reason(int code) {
  switch(code) {
    case 1 : return "POWERON_RESET";          /**<1, Vbat power on reset*/
    case 3 : return "SW_RESET";               /**<3, Software reset digital core*/
    case 4 : return "OWDT_RESET";             /**<4, Legacy watch dog reset digital core*/
    case 5 : return "DEEPSLEEP_RESET";        /**<5, Deep Sleep reset digital core*/
    case 6 : return "SDIO_RESET";             /**<6, Reset by SLC module, reset digital core*/
    case 7 : return "TG0WDT_SYS_RESET";       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : return "TG1WDT_SYS_RESET";       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : return "RTCWDT_SYS_RESET";       /**<9, RTC Watch dog Reset digital core*/
    case 10 : return "INTRUSION_RESET";       /**<10, Instrusion tested to reset CPU*/
    case 11 : return "TGWDT_CPU_RESET";       /**<11, Time Group reset CPU*/
    case 12 : return "SW_CPU_RESET";          /**<12, Software reset CPU*/
    case 13 : return "RTCWDT_CPU_RESET";      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return "EXT_CPU_RESET";         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return "RTCWDT_BROWN_OUT_RESET";/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return "RTCWDT_RTC_RESET";      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return "NO_MEAN";
  }
}
  
// allocate enough space for the prefix and the UUID
#define MAX_UUID_LENGTH 37
#define HOMEBUS_ENDPOINT_LENGTH sizeof("/homebus/device/") + MAX_UUID_LENGTH
static char homebus_endpoint[HOMEBUS_ENDPOINT_LENGTH + 1];
static char hostname[sizeof("furball-%02x%02x%02x") + 1];
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
  byte mac_address[6];

  bootCount++;

  delay(500);

  Serial.begin(115200);
  Serial.println("Hello World!");
  Serial.printf("Build %s\n", build_info);

  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    }

  WiFi.macAddress(mac_address);
  snprintf(hostname, sizeof(hostname), "furball-%02x%02x%02x", (int)mac_address[3], (int)mac_address[4], (int)mac_address[5]);
  Serial.printf("Hostname: %s\n", hostname);

  snprintf(homebus_endpoint, HOMEBUS_ENDPOINT_LENGTH, "/homebus/device/%s", MQTT_UUID);
  Serial.printf("Homebus endpoint: %s\n", homebus_endpoint);

  WiFi.setHostname(hostname);

  wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

  static int wifi_tries = 0;
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);

    if(wifi_tries++ > 100) {
      wifi_failures++;
      ESP.restart();
    }
  }

  Serial.println(WiFi.localIP());
  Serial.println("[wifi]");

  if(!MDNS.begin(hostname))
    Serial.println("Error setting up MDNS responder!");
  else
    Serial.println("[mDNS]");

#ifdef ESP32
  ArduinoOTA.setHostname(hostname);
   ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
      ESP.restart();
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
#endif

  ArduinoOTA.begin();

#if 0
  if(!load_homebus_config()) {
    homebus_go();
    Serial.println("[homebus]");
  } else {
    // first argument is the client ID, not the server name
    mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
    mqtt_client.connect(hostname, MQTT_USER, MQTT_PASS);
    while(!mqtt_client.connected()) {
      Serial.print('+');
      delay(200);
    }

    Serial.println("[homebus]");
    Serial.println("[mqtt]");
  }
#else
    mqtt_client.setServer(MQTT_HOST, MQTT_PORT);
    mqtt_client.connect(MQTT_UUID, MQTT_USER, MQTT_PASS);

    Serial.println("[homebus]");
    Serial.println("[mqtt]");
#endif

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
  static unsigned long last_mqtt_check = 0;

  mqtt_client.loop();

  indicator_loop();

  if(millis() > last_mqtt_check + 5000) {
    if(!mqtt_client.connected()) {
      mqtt_client.connect(MQTT_UUID, MQTT_USER, MQTT_PASS);
      Serial.println("mqtt reconnect");
    }

    last_mqtt_check = millis();
  }

  ArduinoOTA.handle();

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
    Serial.printf("Sound average %u\n", sound_level.sound_level());
    Serial.printf("Sound max %u\n", sound_level.sound_max());
    Serial.printf("Sound min %u\n", sound_level.sound_min());
    Serial.printf("Sound samples %u\n", sound_level.sample_count());
  }


  if(next_loop > millis() || !mqtt_client.connected())
    return;

  next_loop = millis() + UPDATE_DELAY;

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
	   hostname, build_info, ESP.getFreeHeap(), uptime.uptime()/1000, local[0], local[1], local[2], local[3], WiFi.RSSI(), bootCount, wifi_failures,
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

    Serial.println(buffer);
    if(!mqtt_client.publish(homebus_endpoint, buffer, true))
      Serial.println("MQTT publish /furball failed! :(");
}
