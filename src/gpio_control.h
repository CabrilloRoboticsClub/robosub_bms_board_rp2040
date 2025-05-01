#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"

typedef enum {
    PIN_STATUS_OK    = 22,
    PIN_STATUS_ERR   = 26,
    PIN_KILL_SWITCH  = 16,
    
    PIN_SWITCH_0     = 10,
    PIN_SWITCH_1     = 5,
    PIN_SWITCH_2     = 4,
    PIN_SWITCH_3     = 18,
    PIN_SWITCH_4     = 19,
    PIN_SWITCH_5     = 20
} gpio_pin_t;

#define NUM_SWITCH_PINS 6

extern const gpio_pin_t SWITCH_PINS[NUM_SWITCH_PINS];

void gpio_control_init(void);

void gpio_set_status_ok(bool on);
void gpio_set_status_err(bool on);
void gpio_flash_status_ok(void);
void gpio_flash_status_err(void);

void gpio_set_switch(uint8_t index, bool on);
void gpio_toggle_switch(uint8_t index);

bool gpio_kill_switch_triggered(void);

#endif
