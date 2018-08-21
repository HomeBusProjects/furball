#pragma once

#include "sensor.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

class TSL2561_Sensor : public Sensor {
 public:
  //  TSL2561_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : _tsl2561(0x39, TSL2561_PACKAGE_T_FN_CL), Sensor(update_frequency, accuracy, precision, calibrated) {};

  TSL2561_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin();
  void handle();

  uint32_t full() { _mark_read(); return _full;};
  uint32_t ir() { _mark_read(); return _ir;};
  uint32_t visible() { _mark_read(); return _visible;};
  uint32_t lux() { _mark_read(); return _lux;};

 private:
  //  Adafruit_TSL2561_Unified _tsl2561 = Adafruit_TSL2561_Unified(0x39, TSL2561_PACKAGE_T_FN_CL);
  Adafruit_TSL2561_Unified _tsl2561 = Adafruit_TSL2561_Unified(0x39, 0);

  uint32_t _full = 0;
  uint32_t _ir = 0;
  uint32_t _visible = 0;
  uint32_t _lux = 0;
  boolean _overloaded = false;
};
