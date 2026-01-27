#include "pico/stdlib.h"

#define LED_TEST 19 

int main() {
    stdio_init_all();
    
    gpio_init(LED_TEST);
    gpio_set_dir(LED_TEST, GPIO_OUT);

    while (true) {
        gpio_put(LED_TEST, 1);
        sleep_ms(500);
        gpio_put(LED_TEST, 0);
        sleep_ms(500);
    }
}
