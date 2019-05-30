#pragma once

#include <Arduino.h>

#include "sensor.h"

#ifdef ORIGINAL_PMS_LIBRARY
#include <PMS.h>
#endif

#ifdef NEW_PMS_LIBRARY
#include <pms.h>
#endif

class PMS_Sensor : public Sensor {
 public:
  PMS_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin(HardwareSerial);
  void begin();
  void handle();

  uint16_t density_1_0() { _mark_read(); return _density_1_0; };
  uint16_t density_2_5() { _mark_read(); return _density_2_5; };
  uint16_t density_10_0() { _mark_read(); return _density_10_0; };

 private:
#ifdef ORIGINAL_PMS_LIBRARY
  PMS *_pms;
#endif

#ifdef NEW_PMS_LIBRARY
  Pmsx003 *_pms;
#endif

  uint16_t _density_1_0;
  uint16_t _density_2_5;
  uint16_t _density_10_0;

  uint32_t _last_read_request = 0;
};
