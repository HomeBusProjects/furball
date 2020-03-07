#include "hw.h"
#include "pms_sensor.h"

void PMS_Sensor::begin(HardwareSerial port) {
  port.begin(9600, (uint32_t)SERIAL_8N1, 25, 26);

  // 16 - ESP32 xmt
  // 17 - ESP32 rcv
  _pms = new Pmsx003(17, 16);
  _pms->begin();
  _pms->waitForData(Pmsx003::wakeupTime);
  _pms->write(Pmsx003::cmdModeActive);
}

void PMS_Sensor::begin() {
}

#define PMS_READ_DELAY 1000

static auto lastRead = millis();

void PMS_Sensor::handle() {
  if(millis() - _last_read_request < PMS_READ_DELAY)
    return;

    const auto n = Pmsx003::Reserved;
    Pmsx003::pmsData data[n];

    Pmsx003::PmsStatus status = _pms->read(data, n);

    switch (status) {
    case Pmsx003::OK: {
#ifdef VERBOSE
      Serial.println("_________________");
      auto newRead = millis();
      Serial.print("Wait time ");
      Serial.println(newRead - _last_read_request);

      // For loop starts from 3
      // Skip the first three data (PM1dot0CF1, PM2dot5CF1, PM10CF1)
      for (size_t i = Pmsx003::PM1dot0; i < n; ++i) { 
	Serial.print(data[i]);
	Serial.print("\t");
	Serial.print(Pmsx003::dataNames[i]);
	Serial.print(" [");
	Serial.print(Pmsx003::metrics[i]);
	Serial.print("]");
	Serial.println();
      }

#endif

      _density_1_0 = data[0];
      _density_2_5 = data[1];
      _density_10_0 = data[2];

      break;
    }
    case Pmsx003::noData:
      break;
    default:
      Serial.println("_________________");
      Serial.println(Pmsx003::errorMsg[status]);
    };

    _last_read_request = millis();
}
