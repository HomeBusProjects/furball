#pragma once

#include "sensor.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

class LIS3DH_Sensor : public Sensor {
 public:
  LIS3DH_Sensor(uint16_t update_frequency, uint16_t accuracy, uint16_t precision, boolean calibrated) : Sensor(update_frequency, accuracy, precision, calibrated) {};

  void begin();
  void handle();

  uint32_t accel_x() { _mark_read(); return _accel_x; };
  uint32_t accel_y() { _mark_read(); return _accel_y; };
  uint32_t accel_z() { _mark_read(); return _accel_z; };

 private:
  Adafruit_LIS3DH _lis3dh;

  uint32_t _accel_x;
  uint32_t _accel_y;
  uint32_t _accel_z;
};
