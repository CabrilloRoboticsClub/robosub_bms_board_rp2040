#include <stdio.h>

#include "bme280_wrapper.h"
#include "gpio_control.h"
#include "ina780.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();
  sleep_ms(3000);

  gpio_control_init();
  ina780_init();

  printf("Starting system...\n");

  if (!bme280_wrapper_init()) {
    printf("Failed to initialize BME280!\n");
  } else {
    printf("BME280 initialized successfully.\n");
  }

  while (true) {
    // toggle switches
    for (int i = 0; i < NUM_SWITCH_PINS; ++i) {
      gpio_toggle_switch(i);
      sleep_ms(100);
    }

    // kill switch logic (currently reversed just noticed)
    if (gpio_kill_switch_triggered()) {
      gpio_set_status_err(true);
      gpio_set_status_ok(false);
      printf("Kill switch triggered!\n");
    } else {
      gpio_set_status_err(false);
      gpio_set_status_ok(true);
    }

    // BME280
    float temp, hum, press;
    if (bme280_read_all(&temp, &hum, &press)) {
      printf("BME280: T = %.2f °C, H = %.2f %%, P = %.2f hPa\n", temp, hum, press);
    } else {
      printf("Failed to read BME280 sensor.\n");
    }

    // INA780
    float current = ina780_read_current();
    float voltage = ina780_read_bus_voltage();
    float chip_temp = ina780_read_temperature();

    printf("INA780: Current = %.3f A, Voltage = %.3f V, Temp = %.2f °C\n", current, voltage, chip_temp);

    sleep_ms(1000);
  }

  return 0;
}
