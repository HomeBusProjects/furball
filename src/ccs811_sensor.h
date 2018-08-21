#pragma once

#include "sensor.h"

#include <SparkFunCCS811.h>

#define CCS811_DEFAULT_ADDR 0x5B
#define CCS811_ALTERNATE_ADDR 0x5A //Alternate I2C Address

class CCS811_Sensor : public Sensor {
 public:
  CCS811_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated, uint8_t i2c_address = CCS811_DEFAULT_ADDR) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin();
  void handle();

  uint16_t voc() {  _mark_read(); return _voc; };
  uint16_t eco2() {  _mark_read(); return _eco2; };
  float temperature() {  _mark_read(); return _temperature; };

 private:
  CCS811 _ccs811 = CCS811(CCS811_ALTERNATE_ADDR);

  uint16_t _voc;
  uint16_t _eco2;
  float _temperature;

  uint16_t _i2c_address;
};
