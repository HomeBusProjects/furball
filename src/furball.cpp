#include "furball.h"

#include <Arduino.h>

#include "config.h"
#include "hw.h"

#include <multiball/app.h>
#include <multiball/wifi.h>
#include <multiball/homebus.h>

#ifdef USE_DIAGNOSTICS
#include <multiball/diagnostics.h>
#endif

#include "sensors/bme680_sensor.h"
#include "sensors/tsl2561_sensor.h"
#include "sensors/pms_sensor.h"

#include "sensors/pir_sensor.h"
#include "sensors/sound_level_sensor.h"

#include "sensors/uptime.h"

static Uptime uptime;

static BME680_Sensor bme680(UPDATE_DELAY, 0, 0, false);
static TSL2561_Sensor tsl2561(UPDATE_DELAY, 0, 0, false);
static PMS_Sensor pms5003(UPDATE_DELAY, 0, 0, false);

static PIR_Sensor pir(1, UPDATE_DELAY, 0, 0, false);
static SoundLevel_Sensor sound_level(SOUND_PIN, UPDATE_DELAY, 0, 0, false);

void furball_setup() {
  bme680.begin();
  Serial.println("[bme680]");

  tsl2561.begin();
  Serial.println("[tsl2561]");

  pms5003.begin(Serial1);
  Serial.println("[pms5003]");

  pir.begin();
  Serial.println("[pir]");

  sound_level.begin();
  Serial.println("[sound intensity]");
}

static boolean furball_air_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.air-sensor\": { \"temperature\": %.1f, \"humidity\": %.1f, \"pressure\": %.1f } }",
	   homebus_uuid(),
#ifdef TEMPERATURE_ADJUSTMENT
	   bme680.temperature() + TEMPERATURE_ADJUSTMENT,
#else
	   bme680.temperature(),
#endif
	   bme680.humidity(), bme680.pressure());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_air_quality_update(char* buf, size_t buf_len) {
  uint16_t pm1 = pms5003.density_1_0();
  uint16_t pm25 = pms5003.density_2_5();
  uint16_t pm10 = pms5003.density_10_0();

  if(pm1 > 10000 && uptime.uptime() < 60*1000)
    pm1 = 0;

  if(pm25 > 10000 && uptime.uptime() < 60*1000)
    pm25 = 0;

  if(pm10 > 10000 && uptime.uptime() < 60*1000)
    pm10 = 0;

  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.air-quality-sensor\": {  \"tvoc\": %0.2f, \"pm1\": %d, \"pm25\": %d, \"pm10\": %d } }",
	   homebus_uuid(),
	   bme680.gas_resistance(), pm1, pm25, pm10);

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_light_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.light-sensor\": {  \"lux\": %d, \"full_light\": %d, \"ir\": %d, \"visible\": %d } }",
	   homebus_uuid(),
	   tsl2561.lux(), tsl2561.full(), tsl2561.ir(), tsl2561.visible());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}


static boolean furball_sound_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.sound-sensor\": { \"average\": %d, \"min\": %d, \"max\": %d, \"samples\": %d } }",
	   homebus_uuid(),
	   sound_level.sound_level(), sound_level.sound_min(), sound_level.sound_max(), sound_level.sample_count()
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_presence_update(char* buf, size_t buf_len) {
  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.presence-sensor\": { \"presence\": %s } }",
	   homebus_uuid(),
	   pir.presence() ? "true" : "false"
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

/*
 * we do this once at startup, and not again unless our IP address changes
 */
static boolean furball_system_update(char* buf, size_t buf_len) {
  static IPAddress oldIP = IPAddress(0, 0, 0, 0);
  static String mac_address = WiFi.macAddress();
  IPAddress local = WiFi.localIP();

  if(oldIP == local)
    return false;

  snprintf(buf,
	   buf_len,
	   "{ \"id\": \"%s\", \"org.homebus.experimental.furball-system\": { \"name\": \"%s\", \"build\": \"%s\", \"ip\": \"%d.%d.%d.%d\", \"mac_addr\": \"%s\" } }",
	   homebus_uuid(),
	   App.hostname().c_str(), App.build_info().c_str(), local[0], local[1], local[2], local[3], mac_address.c_str()
	   );

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}

static boolean furball_diagnostic_update(char* buf, size_t buf_len) {
  snprintf(buf, buf_len, "{ \"id\": \"%s\", \"org.homebus.experimental.furball-diagnostic\": { \"freeheap\": %d, \"uptime\": %lu, \"rssi\": %d, \"reboots\": %d, \"wifi_failures\": %d } }",
	   homebus_uuid(),
	   ESP.getFreeHeap(), uptime.uptime()/1000, WiFi.RSSI(), App.boot_count(), App.wifi_failures());

#ifdef VERBOSE
  Serial.println(buf);
#endif

  return true;
}


void furball_loop() {
  static unsigned long next_loop = 0;

  if(next_loop > millis())
    return;

  next_loop = millis() + UPDATE_DELAY;

  bme680.handle();
  tsl2561.handle();
  pms5003.handle();
  sound_level.handle();
  pir.handle();

  #define BUFFER_LENGTH 700
  char buffer[BUFFER_LENGTH + 1];

  if(furball_air_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.air-sensor", buffer);

  if(furball_air_quality_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.air-quality-sensor", buffer);

  if(furball_light_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.light-sensor", buffer);

  if(furball_sound_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.sound-sensor", buffer);

  if(furball_presence_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.presence-sensor", buffer);

  if(furball_system_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.furball-system", buffer);

  if(furball_diagnostic_update(buffer, BUFFER_LENGTH))
    homebus_publish_to("org.homebus.experimental.furball-diagnostic", buffer);
}

/* 
 * this callback is used to stream sensor data for diagnostics
 */
#ifdef USE_DIAGNOSTICS
void furball_stream() {
  static uint8_t count = 0;

  if(count == 0)
    Serial.println("TEMP PRES HUMD TVOC   IR VISB FULL  LUX  1.0  2.5 10.0  SMAX  SMIN  SAVG  SCNT  PIR");

  if(++count == 10)
    count = 0;

  bme680.handle();
  tsl2561.handle();
  pms5003.handle();
  sound_level.handle();

  Serial.printf( "%03.1f %4.0f %4.0f %4.0f %4d %4d %4d %4d %4d %4d %4d %5d %5d %5d %5d    %c\n",
		 bme680.temperature(),
		 bme680.pressure(),
		 bme680.humidity(),
		 bme680.gas_resistance(),
		 tsl2561.ir(),
		 tsl2561.visible(),
		 tsl2561.full(),
		 tsl2561.lux(),
		 pms5003.density_1_0(),
		 pms5003.density_2_5(),
		 pms5003.density_10_0(),
		 sound_level.sound_max(),
		 sound_level.sound_min(),
		 sound_level.sound_level(),
		 sound_level.sample_count(),
		 pir.presence() ? '1' : '0');

  if(0) {
  Serial.println("[system]");
  Serial.printf("  Uptime %.2f seconds\n", uptime.uptime() / 1000.0);
  Serial.printf("  Free heap %u bytes\n", ESP.getFreeHeap());
  Serial.printf("  Wifi RSSI %d\n", WiFi.RSSI());
  }
}
#endif
