#include "gpio_control.h"
#include "pico/stdlib.h"

const gpio_pin_t SWITCH_PINS[NUM_SWITCH_PINS] = {
    PIN_SWITCH_0,
    PIN_SWITCH_1,
    PIN_SWITCH_2,
    PIN_SWITCH_3,
    PIN_SWITCH_4,
    PIN_SWITCH_5
};

void gpio_control_init(void) {
    gpio_init(PIN_STATUS_OK);
    gpio_set_dir(PIN_STATUS_OK, GPIO_OUT);
    
    gpio_init(PIN_STATUS_ERR);
    gpio_set_dir(PIN_STATUS_ERR, GPIO_OUT);

    gpio_init(PIN_KILL_SWITCH);
    gpio_set_dir(PIN_KILL_SWITCH, GPIO_IN);
    gpio_pull_up(PIN_KILL_SWITCH);

    for (int i = 0; i < NUM_SWITCH_PINS; i++) {
        gpio_init(SWITCH_PINS[i]);
        gpio_set_dir(SWITCH_PINS[i], GPIO_OUT);
        gpio_put(SWITCH_PINS[i], 0);
    }
}

void gpio_set_status_ok(bool on) {
    gpio_put(PIN_STATUS_OK, on);
}

void gpio_set_status_err(bool on) {
    gpio_put(PIN_STATUS_ERR, on);
}

void gpio_flash_status_ok(void) {
    gpio_put(PIN_STATUS_OK, 1);
    sleep_ms(100);
    gpio_put(PIN_STATUS_OK, 0);
}

void gpio_flash_status_err(void) {
    gpio_put(PIN_STATUS_ERR, 1);
    sleep_ms(100);
    gpio_put(PIN_STATUS_ERR, 0);
}

void gpio_set_switch(uint8_t index, bool on) {
    if (index < NUM_SWITCH_PINS) {
        gpio_put(SWITCH_PINS[index], on);
    }
}

void gpio_toggle_switch(uint8_t index) {
    if (index < NUM_SWITCH_PINS) {
        bool current = gpio_get(SWITCH_PINS[index]);
        gpio_put(SWITCH_PINS[index], !current);
    }
}

bool gpio_kill_switch_triggered(void) {
    return !gpio_get(PIN_KILL_SWITCH);
}
