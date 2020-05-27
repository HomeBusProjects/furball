#include "diagnostics.h"

#include <Arduino.h>

#include <multiball/app.h>

void i2c_begin(), i2c_scan(), blink_start(int), wifi_begin(), wifi_scan(), wifi_info(), esp_output(), wifi_disconnect(), wifi_connect(const char*, const char*);

bool wifi_connected();

static void diagnostics_homebus();
static int serial_read_line();

static void output_menu() {
  Serial.println("1. Show ESP information");
  Serial.println("5. Scan wifi networks");
  Serial.println("6. Scan I2C devices");
  Serial.println("7. Review/edit Homebus config");
  Serial.println("0. Quit/Restart");
}


void diagnostics_setup(){
  Serial.begin(115200);

  //  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println("Hello World");
  delay(5000);

  i2c_begin();

  output_menu();
}

void diagnostics_loop(void (*streamer)()) {
  static boolean should_stream = false;

  if (Serial.available() > 0) {
    char cmd[2];

    Serial.readBytes(cmd, 1);
    
    switch(cmd[0]) {
    case '1':
      esp_output();
      break;

    case '5':
      wifi_scan();
      break;

    case '6':
      i2c_scan();
      break;

    case '7':
      diagnostics_homebus();

    case '0':
      Serial.println("RESTART ESP");
      ESP.restart();
      break;

    case 'h':
    case '?':
    case '\n':
      output_menu();
      break;

    case 's':
      should_stream = true;
      break;

    default:
      Serial.printf("Unknown command '%c'\n", cmd[0]);
    }
  }

  if(!should_stream)
    return;

  static unsigned long next_sample = 0;
  if(millis() < next_sample)
    return;

  next_sample = millis() + 1000;
  (*streamer)();
}

/* basic line reading function which handles backspace and delete
 * can't believe I had to write this; I must be missing something...
 */
static int serial_read_line(char* buf, int length) {
  int n = 0;

  while(1) {
    if(Serial.available() == 0) {
      yield();
      continue;
    }

    char c = Serial.read();

    //    Serial.printf("%d", (int)c);

    if(c == -1 || c == '\n') {
      yield();
      continue;
    }

    if(c == '\b' || c == 0x7f) {
      n--;
      buf[n] = '\0';
      Serial.print('\b');
      continue;
    }

    if(n == length) {
      Serial.print("\07");
      continue;
    }
    
    if(c == '\r') {
      Serial.println();
      return n;
    }

    Serial.print(c);

    buf[n++] = c;
    buf[n] = '\0';
  }
}

static void diagnostics_homebus() {
  Serial.print("Homebus state: "); Serial.println(App.config.get("hb-state"));
  Serial.print("Homebus uuid: "); Serial.println(App.config.get("hb-uuid: "));
  Serial.print("Homebus broker: "); Serial.println(App.config.get("hb-broker: "));
  Serial.print("Homebus username: "); Serial.println(App.config.get("hb-username: "));
  Serial.print("Homebus password: "); Serial.println(App.config.get("hb-password: "));
  Serial.print("Homebus port: "); Serial.println(App.config.get("hb-port: "));
}
