#include "pico/stdlib.h"

int main() {
    const uint LED_PINS[2] = {22, 26}; 

    for (int i = 0; i < 2; ++i) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], GPIO_OUT);
    }

    while (true) {
        for (int i = 0; i < 2; ++i) {
            gpio_put(LED_PINS[i], 1); 
            sleep_ms(200);
            gpio_put(LED_PINS[i], 0); 
        }
    }
}
