#include "bme680_sensor.h"

#define SEALEVELPRESSURE_HPA (1013.25)

void BME680_Sensor::begin() {
  if(!_bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  _bme.setTemperatureOversampling(BME680_OS_8X);
  _bme.setHumidityOversampling(BME680_OS_2X);
  _bme.setPressureOversampling(BME680_OS_4X);
  _bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  _bme.setGasHeater(320, 150);
;}

void BME680_Sensor::handle() {
  _temperature = _bme.readTemperature();
  _pressure = _bme.readPressure() / 100.0F;
  _humidity = _bme.readHumidity();
  _gas_resistance = _bme.gas_resistance / 1000.0;
}
