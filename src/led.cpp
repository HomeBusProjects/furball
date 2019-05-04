#include "led.h"

static uint8_t pwm_channel = 0;

#define FREQUENCY 5000
#define RESOLUTION 8

void LED::_debug() {
  Serial.printf("red pin %d, green pin %d, blue pin %d\n", _red_pin, _green_pin, _blue_pin);
  Serial.printf("red chan %d, green chan %d, blue chan %d\n", _red_channel, _green_channel, _blue_channel);
  delay(500);
}

void LED::begin() {
  _red_channel = pwm_channel++;
  _green_channel = pwm_channel++;
  _blue_channel = pwm_channel++;

  _attach_pwm();

  //  _debug();
}


void LED::set(uint8_t red, uint8_t green, uint8_t blue) {
#if 0
  analogOutput(_red, red);
  analogOutput(_green, green);
  analogOutput(_blue, blue);
#endif

  if(_off) {
    _attach_pwm();
    _off = false;
  }

  if(red == 0 &&  green == 0 && blue == 0) {
    off();
    return;
  }

  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  
  ledcWrite(_red_channel, red);
  ledcWrite(_green_channel, green);
  ledcWrite(_blue_channel, blue);
}

void LED::off() {
  _detach_pwm();

  pinMode(_red_pin, OUTPUT);
  pinMode(_green_pin, OUTPUT);
  pinMode(_blue_pin, OUTPUT);

  digitalWrite(_red_pin, HIGH);
  digitalWrite(_green_pin, HIGH);
  digitalWrite(_blue_pin, HIGH);

  _off = true;
}

void LED::_attach_pwm() {
  ledcSetup(_red_channel, FREQUENCY, RESOLUTION);
  ledcSetup(_green_channel, FREQUENCY, RESOLUTION);
  ledcSetup(_blue_channel, FREQUENCY, RESOLUTION);

  ledcAttachPin(_red_pin, _red_channel);
  ledcAttachPin(_green_pin, _green_channel);
  ledcAttachPin(_blue_pin, _blue_channel);
}

void LED::_detach_pwm() {
  ledcDetachPin(_red_pin);
  ledcDetachPin(_green_pin);
  ledcDetachPin(_blue_pin);
}

