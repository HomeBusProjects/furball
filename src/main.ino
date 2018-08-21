#include <Arduino.h>

#include <ESP.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "config.h"

#include <ArduinoOTA.h>

#include "bme280_sensor.h"
#include "ccs811_sensor.h"
#include "tsl2561_sensor.h"
#include "adxl345_sensor.h"
#include "lis3dh_sensor.h"
#include "pms_sensor.h"

#include "pir_sensor.h"
#include "sound_level_sensor.h"

#define BUTTON_PIN 33
#define PIR_PIN 34
#define SOUND_PIN 35

BME280_Sensor bme280(UPDATE_DELAY, 0, 0, false);
CCS811_Sensor ccs811(UPDATE_DELAY, 0, 0, false);
TSL2561_Sensor tsl2561(UPDATE_DELAY, 0, 0, false);
ADXL345_Sensor adxl345(UPDATE_DELAY, 0, 0, false);
LIS3DH_Sensor lis3dh(UPDATE_DELAY, 0, 0, false);
PMS_Sensor pms5003(UPDATE_DELAY, 0, 0, false);

PIR_Sensor pir(1, UPDATE_DELAY, 0, 0, false);
SoundLevel_Sensor sound_level(SOUND_PIN, UPDATE_DELAY, 0, 0, false);

#include "uptime.h"

Uptime uptime;

#include <JC_Button.h>   

Button the_button(BUTTON_PIN);

// HomeBus is not ready for Prime Time so leave this out for now
#ifdef HOMEBUS

#include <HomeBus.h>
#include <HomeBusDevice.h>

HomeBus hb("Furball One",
	   "an esp8266 far, far away",
	   "HomeBus Central",
	   "0.0.1",
	   WiFi.mac_address,
	   "0-0-0-0");

HomeBusDevice hb_tsl2561_lux(&hb,
			     "Light sensor",
			     "HomeBus One",
			     100,
			     100,
			     1000,
			     true,
			     "lux",
			     "",
			     ""
			     );

HomeBusDevice hb_tsl2561_ir(&hb,
			    "Infrared sensor",
			    "HomeBus One",
			    100,
			    100,
			    1000,
			    true,
			    "ir",
			    "",
			    ""
			    );


HomeBusDevice hb_ccs811_voc(&hb,
			    "VOC",
			    "HomeBus One",
			    100,
			    100,
			    1000,
			    true,
			    "voc",
			    "",
			    ""
			    );

HomeBusDevice hb_ccs811_voc(&hb,
			    "CO2",
			    "HomeBus One",
			    100,
			    100,
			    1000,
			    false,
			    "co2",
			    "",
			    ""
			    );

#endif


#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

WiFiClient client;

#include <IFTTTWebhook.h>
IFTTTWebhook ifttt(IFTTT_API_KEY, IFTTT_EVENT_NAME);

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temperature_feed(&mqtt, AIO_USERNAME "/feeds/furball.temperature");
Adafruit_MQTT_Publish humidity_feed(&mqtt, AIO_USERNAME "/feeds/furball.humidity");
Adafruit_MQTT_Publish pressure_feed(&mqtt, AIO_USERNAME "/feeds/furball.pressure");

Adafruit_MQTT_Publish lux_feed(&mqtt, AIO_USERNAME "/feeds/furball.lux");
Adafruit_MQTT_Publish ir_feed(&mqtt, AIO_USERNAME "/feeds/furball.ir");

Adafruit_MQTT_Publish voc_feed(&mqtt, AIO_USERNAME "/feeds/furball.voc");
Adafruit_MQTT_Publish eco2_feed(&mqtt, AIO_USERNAME "/feeds/furball.eco2");

Adafruit_MQTT_Publish pir_feed(&mqtt, AIO_USERNAME "/feeds/furball.pir");
Adafruit_MQTT_Publish sound_level_feed(&mqtt, AIO_USERNAME "/feeds/furball.sound");

Adafruit_MQTT_Publish uptime_feed(&mqtt, AIO_USERNAME "/feeds/furball.uptime");
Adafruit_MQTT_Publish freeheap_feed(&mqtt, AIO_USERNAME "/feeds/furball.freeheap");


void mqtt_connect(void) {
  int8_t ret;

  return;

  Serial.print("Connecting to Adafruit IO... ");

  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println("Wrong protocol"); break;
      case 2: Serial.println("ID rejected"); break;
      case 3: Serial.println("Server unavail"); break;
      case 4: Serial.println("Bad user/pass"); break;
      case 5: Serial.println("Not authed"); break;
      case 6: Serial.println("Failed to subscribe"); break;
      default: Serial.println("Connection failed"); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println("Retrying connection...");
    delay(5000);
  }

  Serial.println("Adafruit IO Connected!");
}

#include <rom/rtc.h>

char* reboot_reason(int code) {
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
  
void setup() {
  char hostname[sizeof(FURBALL_HOSTNAME) + 8];
  byte mac_address[6];

  delay(5000);

  Serial.begin(115200);
  Serial.println("Hello World!");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  WiFi.macAddress(mac_address);
  snprintf(hostname, sizeof(hostname), "%s %02x%02x%02x", FURBALL_HOSTNAME, (int)mac_address[3], (int)mac_address[4], (int)mac_address[5]);
  Serial.printf("Hostname is %s\n", hostname);

  WiFi.setHostname(hostname);
  while(!WiFi.isConnected()) {
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.println("Connected!");

  ifttt.trigger("reboot", reboot_reason(rtc_get_reset_reason(0)),  reboot_reason(rtc_get_reset_reason(1)));

  if(!MDNS.begin(hostname))
    Serial.println("Error setting up MDNS responder!");
  else
    Serial.println("mDNS responder started");

  Serial.println("MQTT");
  mqtt_connect();

#ifdef ESP32
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

  bme280.begin();
  ccs811.begin();
  tsl2561.begin();
  adxl345.begin();
  lis3dh.begin();
  pms5003.begin(Serial1);

  pir.begin();
  sound_level.begin();

  Serial.printf("BME280 status %s\n", bme280.status_str());
  Serial.printf("CCS811 status %s\n", ccs811.status_str());
  Serial.printf("TSL2561 status %s\n", tsl2561.status_str());
  Serial.printf("ADXL345 status %s\n", adxl345.status_str());
  Serial.printf("lis3hd status %s\n", lis3dh.status_str());

  Serial.println();
  Serial.println();
  Serial.printf("PIR status %s\n", pir.status_str());
  Serial.printf("Sound level status %s\n", sound_level.status_str());

  delay(5000);
}


void loop() {
  static unsigned long last_loop = 0;

  ArduinoOTA.handle();

  bme280.handle();
  ccs811.handle();
  tsl2561.handle();
  lis3dh.handle();
  pms5003.handle();

  sound_level.handle();
  pir.handle();

  if(bme280.ready_for_update()) {
    temperature_feed.publish(bme280.temperature());
    pressure_feed.publish(bme280.pressure());
    humidity_feed.publish(bme280.humidity());

    Serial.printf("Temperature %d\n", bme280.temperature());
    Serial.printf("Pressure %d\n", bme280.pressure());
    Serial.printf("Humidity %d\n", bme280.humidity());
  }

  if(ccs811.ready_for_update()) {
    eco2_feed.publish(ccs811.eco2());
    voc_feed.publish(ccs811.voc());

    Serial.printf("ECO2 %d\n", ccs811.eco2());
    Serial.printf("VOC %d\n", ccs811.voc());
  }

  if(tsl2561.ready_for_update()) {
    lux_feed.publish(tsl2561.lux());
    ir_feed.publish(tsl2561.ir());

    Serial.printf("IR %d\n", tsl2561.ir());
    Serial.printf("Visible %d\n", tsl2561.visible());
    Serial.printf("Full %d\n", tsl2561.full());
    Serial.printf("Lux %d\n", tsl2561.lux());
  }

  if(lis3dh.ready_for_update()) {
    Serial.printf("LIS3DH x %d\n", lis3dh.accel_x());
    Serial.printf("LIS3DH y %d\n", lis3dh.accel_y());
    Serial.printf("LIS3DH z %d\n", lis3dh.accel_z());
  }

  if(adxl345.ready_for_update()) {
    Serial.printf("ADXL345 x %d\n", adxl345.accel_x());
    Serial.printf("ADXL345 y %d\n", adxl345.accel_y());
    Serial.printf("ADXL345 z %d\n", adxl345.accel_z());
  }

  if(pms5003.ready_for_update()) {
    Serial.printf("PMS5003 1.0 %d\n", pms5003.density_1_0());
    Serial.printf("PMS5003 2.5 %d\n", pms5003.density_2_5());
    Serial.printf("PMS5003 10.0 %d\n", pms5003.density_10_0());
  }

  if(sound_level.ready_for_update()) {
    //    sound_level_feed.publish(sound_level.sound_level());
    //    Serial.printf("Sound level %d\n", sound_level.sound_level());

    sound_level_feed.publish(sound_level.sample_value());
    Serial.printf("Sound level %d with %d samples\n", sound_level.sample_value(), sound_level.sample_count());
    sound_level.start_sampling();
  }

  if(millis() - last_loop < UPDATE_DELAY)
    return;

  last_loop = millis();

  if(! mqtt.ping(3)) {
    if(! mqtt.connected())
      mqtt_connect();
  }

  uptime_feed.publish((unsigned)uptime.uptime()/1000);
  Serial.printf("Uptime %.2f seconds\n", uptime.uptime() / 1000.0);

  freeheap_feed.publish(ESP.getFreeHeap());
  Serial.printf("Free heap %u bytes\n", ESP.getFreeHeap());

#ifdef REST_API_ENDPOINT
  char buffer[500];
  snprintf(buffer, 500, "{\"temperature\": %d, \"humidity\": %d, \"pressure\": %d, \"eco2\": %d, \"tvoc\": %d, \"pm1\": %d, \"pm25\": %d, \"pm10\": %d, \"freeheap\": %d, \"uptime\": %lu, \"lux\": %d, \"full_light\": %d, \"ir\": %d, \"visible\": %d }",
	   bme280.temperature(), bme280.humidity(), bme280.pressure(),
	   ccs811.eco2(), ccs811.voc(), 
	   pms5003.density_1_0(), pms5003.density_2_5(), pms5003.density_10_0(),
	   ESP.getFreeHeap(), uptime.uptime()/1000,
	   tsl2561.lux(), tsl2561.full(), tsl2561.ir(), tsl2561.visible());

    Serial.println(buffer);
    post(buffer);
#endif
}

void post(char *json) {
  HTTPClient http;

  http.begin(String(REST_API_ENDPOINT));
  http.addHeader("Content-Type", "application/json");
  int response = http.POST(json);
  if(response > 0) {
    Serial.printf("HTTP status code %d\n", response);
  } else {
    Serial.printf("HTTPClient error %d\n", response);
  }

  http.end();
}
