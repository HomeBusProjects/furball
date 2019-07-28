#include "sound_level_sensor.h"

/*
 * Nornmal human hearing is 20 - 20,000Hz
 * so let's take 20,000 samples per second and report their average and min/max
 * (except this code is actually taking 20,000 samples per minute...)
 */

static uint8_t audio_pin;

static volatile uint32_t audio_min     = 0xffffffff;
static volatile uint32_t audio_max     = 0;
static volatile uint32_t audio_sum = 0;
static volatile uint32_t audio_samples = 0;

static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// important to do as little as possible during the interrupt
// other interrupts are locked out at this time and it's not safe
// to call most Arduino, ESP-IDF or FreeRTOS functions
static void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);

  uint32_t sample = analogRead(audio_pin);

  sample *= sample;

  audio_sum += sample;
  audio_samples++;

  if(sample > audio_max)
    audio_max = sample;
  else if(sample < audio_min)
    audio_min = sample;

  portEXIT_CRITICAL_ISR(&timerMux);
}

void SoundLevel_Sensor::begin() {
  static hw_timer_t *timer = NULL;  

  audio_pin = _pin;

  pinMode(_pin, INPUT);

  // https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
  //  timer = timerBegin(0, 80, true);
  timer = timerBegin(0, 400, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10, true);
  timerAlarmEnable(timer);
}

void SoundLevel_Sensor::handle() {
  portENTER_CRITICAL(&timerMux);

  if(audio_samples)
    _sample_average = audio_sum / audio_samples;
  else
    _sample_average = 0;

  _sample_min = audio_min;
  _sample_max = audio_max;
  _sample_count = audio_samples;

  audio_min     = 0xffffffff;
  audio_max     = 0;
  audio_sum     = 0;
  audio_samples = 0;

  portEXIT_CRITICAL(&timerMux);
}
