#include "bme280_sensor.h"

#define SEALEVELPRESSURE_HPA (1013.25)

/*
 * the BME280 datasheet suggests the following configurations:
 *
 * outdoor weather monitoring:
 * forced mode, 1 sample per minute, oversampling all *1, filter off
 *
 * indoor "navigation"
 * normal mode, tstandby = 0.5ms, filter *16, oversampling pressure*16, temperature*2, humidity*1
 *
 */

void BME280_Sensor::begin() {
  if(!_bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  _present = true;

  // configure for indoor use
  _bme.setSampling(Adafruit_BME280::MODE_NORMAL,
		   Adafruit_BME280::SAMPLING_X2,
		   Adafruit_BME280::SAMPLING_X16,
		   Adafruit_BME280::SAMPLING_X1,
		   Adafruit_BME280::FILTER_X16,
		   Adafruit_BME280::STANDBY_MS_0_5);
  
  Serial.println("BME OK");
}

void BME280_Sensor::handle() {
  _temperature = _bme.readTemperature();
  _pressure = _bme.readPressure() / 100.0F;
  _humidity = _bme.readHumidity();
  _altitude = _bme.readAltitude(SEALEVELPRESSURE_HPA);
}
