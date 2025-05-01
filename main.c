#include <stdio.h>

#include "bme280_wrapper.h"
#include "gpio_control.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();
  sleep_ms(3000);  // Give USB time to connect

  gpio_control_init();

  printf("Starting system...\n");

  if (!bme280_wrapper_init()) {
    printf("Failed to initialize BME280!\n");
  } else {
    printf("BME280 initialized successfully.\n");
  }

  while (true) {
    // Toggle switches (demo effect)
    for (int i = 0; i < NUM_SWITCH_PINS; ++i) {
      gpio_toggle_switch(i);
      sleep_ms(100);
    }

    // Kill switch logic
    if (gpio_kill_switch_triggered()) {
      gpio_set_status_err(true);
      gpio_set_status_ok(false);
      printf("Kill switch triggered!\n");
    } else {
      gpio_set_status_err(false);
      gpio_set_status_ok(true);
    }

    // Read sensor
    float temp, hum, press;
    if (bme280_read_all(&temp, &hum, &press)) {
      printf("BME280: T = %.2f °C, H = %.2f %%, P = %.2f hPa\n", temp, hum, press);
    } else {
      printf("Failed to read BME280 sensor.\n");
    }

    sleep_ms(1000);
  }

  return 0;
}
