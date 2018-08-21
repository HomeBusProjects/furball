#include "lis3dh_sensor.h"

void LIS3DH_Sensor::begin() {
  if (!_lis3dh.begin(0x18)) {
    Serial.println("Could not find a valid LIS3DH sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  _status = SENSOR_OKAY;
  Serial.println("LIS3DH OK");

  _lis3dh.setRange(LIS3DH_RANGE_16_G);   // 2, 4, 8 or 16 G!
}

void LIS3DH_Sensor::handle() {
  sensors_event_t event; 

  _lis3dh.getEvent(&event);
  _accel_x = event.acceleration.x;
  _accel_y = event.acceleration.y;
  _accel_z = event.acceleration.z;

#if 0
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y); 
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z); 
  Serial.println(" m/s^2 ");

  Serial.println();
#endif
}
