#include <Arduino.h>

class LED {
public:
  LED(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin) : _red_pin(red_pin), _green_pin(green_pin), _blue_pin(blue_pin) {};

  void begin();
  void set(uint8_t red, uint8_t green, uint8_t blue);

  void off();

private:
  bool _off = false;

  uint8_t _red_pin;
  uint8_t _green_pin;
  uint8_t _blue_pin;

  uint8_t _red_channel;
  uint8_t _green_channel;
  uint8_t _blue_channel;

  void _attach_pwm();
  void _detach_pwm();

  void _debug();
};
