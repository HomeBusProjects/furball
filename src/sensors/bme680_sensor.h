#pragma once

#include "sensor.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

class BME680_Sensor : public Sensor {
 public:
  BME680_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin();
  void handle();

  float temperature() { _mark_read(); return _temperature; };
  float humidity() { _mark_read(); return _humidity; };
  float pressure() { _mark_read(); return _pressure; };
  float altitude() { _mark_read(); return _altitude; };
  float gas_resistance() { _mark_read(); return _gas_resistance; };

 private:
  Adafruit_BME680 _bme;
  float _temperature;
  float _humidity;
  float _pressure;
  float _altitude;
  float _gas_resistance;
};
