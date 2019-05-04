#pragma once

#include "sensor.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

class BME680_Sensor : public Sensor {
 public:
  BME680_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin();
  void handle();

  uint16_t temperature() { _mark_read(); return _temperature; };
  uint16_t humidity() { _mark_read(); return _humidity; };
  uint16_t pressure() { _mark_read(); return _pressure; };
  uint16_t altitude() { _mark_read(); return _altitude; };
  float gas_resistance() { _mark_read(); return _gas_resistance; };

 private:
  Adafruit_BME680 _bme;
  uint16_t _temperature;
  uint16_t _humidity;
  uint16_t _pressure;
  uint16_t _altitude;
  float _gas_resistance;
};
