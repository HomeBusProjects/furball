#include "adxl345_sensor.h"

void ADXL345_Sensor::begin() {
  if (!_adxl.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    _status = SENSOR_NOT_FOUND;
    return;
  }

  _status = SENSOR_OKAY;
  Serial.println("ADXL345 OK");

  _adxl.setRange(ADXL345_RANGE_16_G);
}

void ADXL345_Sensor::handle() {
  sensors_event_t event; 

  _adxl.getEvent(&event);
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
