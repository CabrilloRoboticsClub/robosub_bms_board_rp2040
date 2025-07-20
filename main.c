#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

#include "ssd1306.h"
#include "ina780.h"
#include "bme280_wrapper.h"
#include "gpio_control.h"

#define BMS 1
#include "crc.h"
#include "uart_proto.h"

#define OLED_I2C i2c1
#define OLED_SDA_PIN 2
#define OLED_SCL_PIN 3
#define OLED_ADDR 0x3C

ssd1306_t oled;
const uint LED_PIN = 25;

int main() {
  stdio_init_all();
  sleep_ms(3000);

  gpio_control_init();
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  ina780_init();

  // OLED I2C setup
  i2c_init(OLED_I2C, 400 * 1000);
  gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(OLED_SDA_PIN);
  gpio_pull_up(OLED_SCL_PIN);

  ssd1306_init(&oled, 128, 64, OLED_ADDR, OLED_I2C);
  ssd1306_clear(&oled);
  ssd1306_draw_string(&oled, 0, 0, 1, "RoboSub BMS Init");
  ssd1306_show(&oled);
  sleep_ms(1000);

  printf("Starting system...\n");

  if (!bme280_wrapper_init()) {
    printf("Failed to initialize BME280!\n");
    ssd1306_clear(&oled);
    ssd1306_draw_string(&oled, 0, 0, 1, "BME280 Init FAIL");
    ssd1306_show(&oled);
    ssd1306_clear(&oled);
  } else {
    printf("BME280 initialized successfully.\n");
    ssd1306_clear(&oled);
    ssd1306_draw_string(&oled, 0, 0, 1, "BME280 Init OK");
    ssd1306_show(&oled);
    ssd1306_clear(&oled);
  }

  gpio_put(25, 1);
  sleep_ms(1000);
  uart_inst inst = {uart0, 0, 1, 115200};
  uart_initialization(inst);

  printf("entering loop\n");
  sleep_ms(1000);

  sensor_data retval;

  int motherfucker = 0;

  while (true) {
    printf("waiting...\n");
    bool res = get_response(inst, &retval);
    printf("done waiting. result is %d\n", res);
    motherfucker = (motherfucker + 1) % 2;
    gpio_put(25, motherfucker);
    //sleep_ms(1000);
    // toggle switches
    //for (int i = 0; i < NUM_SWITCH_PINS; ++i) {
    //  gpio_toggle_switch(i);
    //  sleep_ms(100);
    //}

    //// kill switch logic
    //if (gpio_kill_switch_triggered()) {
    //  gpio_set_status_err(true);
    //  gpio_set_status_ok(false);
    //  printf("Kill switch triggered!\n");

    //  ssd1306_clear(&oled);
    //  ssd1306_draw_string(&oled, 0, 0, 1, "KILL SWITCH ON");
    //  ssd1306_show(&oled);
    //  sleep_ms(1000);
    //  continue; // skip rest of loop
    //} else {
    //  gpio_set_status_err(false);
    //  gpio_set_status_ok(true);
    //}

    //// BME280
    //float temp, hum, press;
    //if (bme280_read_all(&temp, &hum, &press)) {
    //  printf("BME280: T = %.2f °C, H = %.2f %%, P = %.2f hPa\n", temp, hum, press);

    //  char line1[22], line2[22];
    //  snprintf(line1, sizeof(line1), "T=%.1fC H=%.0f%%", temp, hum);
    //  snprintf(line2, sizeof(line2), "P=%.1f hPa", press);

    //  ssd1306_clear(&oled);
    //  ssd1306_draw_string(&oled, 0, 0, 1, "BME280 Data:");
    //  ssd1306_draw_string(&oled, 0, 16, 1, line1);
    //  ssd1306_draw_string(&oled, 0, 32, 1, line2);
    //  ssd1306_show(&oled);
    //  sleep_ms(1000);
    //} else {
    //  printf("Failed to read BME280 sensor.\n");
    //}

    //// INA780
    float current = ina780_read_current();
    float voltage = ina780_read_bus_voltage();
    float chip_temp = ina780_read_temperature();
    float power = ina780_read_power();
    float energy = ina780_read_energy();

    printf("INA780: Current = %.3f A, Voltage = %.3f V, Temp = %.2f °C, Power = %.3f W, Engergy = %.3f Wh\n",
           current, voltage, chip_temp, power, energy);

    //char line3[22], line4[22], line5[22];
    //snprintf(line3, sizeof(line3), "I=%.2fA V=%.2fV", current, voltage);
    //snprintf(line4, sizeof(line4), "T=%.1fC P=%.2fW", chip_temp, power);
    //snprintf(line5, sizeof(line5), "E=%.3fWh", energy);

    //ssd1306_clear(&oled);
    //ssd1306_draw_string(&oled, 0, 0, 1, "INA780 Data:");
    //ssd1306_draw_string(&oled, 0, 16, 1, line3);
    //ssd1306_draw_string(&oled, 0, 32, 1, line4);
    //ssd1306_draw_string(&oled, 0, 48, 1, line5);
    //ssd1306_show(&oled);

    //sleep_ms(1000);
  }

  return 0;
}
