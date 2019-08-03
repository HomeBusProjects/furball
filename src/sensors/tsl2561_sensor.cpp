#include "tsl2561_sensor.h"

#if 0
/**************************************************************************/
/*
  Displays some basic information on this sensor from the unified
  sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}
#endif


void TSL2561_Sensor::begin() {
  if (!_tsl2561.begin()) {
    _status = SENSOR_NOT_FOUND;
    Serial.println("TSL2561 not found");
    return;
  }
  
  _status = SENSOR_OKAY;

  _tsl2561.enableAutoRange(true);

  //  _tsl2561.setGain(TSL2561_GAIN_MED);      // 25x gain
  // _tsl2561.setGain(TSL2561_GAIN_HIGH);   // 428x gain
  
  _tsl2561.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
}

void TSL2561_Sensor::handle() {
  sensors_event_t event;

  _tsl2561.getEvent(&event);

  if(event.light) {
    _lux = event.light;

    uint16_t full, ir;
    _tsl2561.getLuminosity(&full, &ir);

    _full = full;
    _ir = ir;
    _visible = full - ir;

    _overloaded = false;
  } else
    _overloaded = true;
}
