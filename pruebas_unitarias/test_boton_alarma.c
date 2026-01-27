#include "pico/stdlib.h"
#include "hardware/pads_bank0.h"

#define PIN_LED 19
#define PIN_BOTON 22
#define PIN_ALARMA 14

int main() {
    stdio_init_all();

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    gpio_init(PIN_ALARMA);
    gpio_set_dir(PIN_ALARMA, GPIO_OUT);
    
    // Configura el pin para dar el máximo de corriente al módulo U2
    pads_set_drive_strength(PIN_ALARMA, PADS_BANK0_GPIO0_DRIVE_12MA);

    gpio_init(PIN_BOTON);
    gpio_set_dir(PIN_BOTON, GPIO_IN);
    gpio_pull_up(PIN_BOTON);

    while (true) {
        if (gpio_get(PIN_BOTON) == 0) {
            gpio_put(PIN_LED, 1);
            gpio_put(PIN_ALARMA, 1);
        } else {
            gpio_put(PIN_LED, 0);
            gpio_put(PIN_ALARMA, 0);
        }
        sleep_ms(10);
    }
}
EO
cat <<EOF > pruebas_unitarias/test_boton_alarma.c
#include "pico/stdlib.h"
#include "hardware/pads_bank0.h"

#define PIN_LED 19
#define PIN_BOTON 22
#define PIN_ALARMA 14

int main() {
    stdio_init_all();

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    gpio_init(PIN_ALARMA);
    gpio_set_dir(PIN_ALARMA, GPIO_OUT);
    
    // Configura el pin para dar el máximo de corriente al módulo U2
    pads_set_drive_strength(PIN_ALARMA, PADS_BANK0_GPIO0_DRIVE_12MA);

    gpio_init(PIN_BOTON);
    gpio_set_dir(PIN_BOTON, GPIO_IN);
    gpio_pull_up(PIN_BOTON);

    while (true) {
        if (gpio_get(PIN_BOTON) == 0) {
            gpio_put(PIN_LED, 1);
            gpio_put(PIN_ALARMA, 1);
        } else {
            gpio_put(PIN_LED, 0);
            gpio_put(PIN_ALARMA, 0);
        }
        sleep_ms(10);
    }
}
