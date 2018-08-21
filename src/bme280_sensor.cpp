#include "bme280_sensor.h"

#define SEALEVELPRESSURE_HPA (1013.25)

void BME280_Sensor::begin() {
  if(!_bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  Serial.println("BME OK");
}

void BME280_Sensor::handle() {
  _temperature = _bme.readTemperature();
  _pressure = _bme.readPressure() / 100.0F;
  _humidity = _bme.readHumidity();
  _altitude = _bme.readAltitude(SEALEVELPRESSURE_HPA);
}
