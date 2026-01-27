#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// MAPEO DE PINES
#define PIN_SERVO 9        // Pin físico 12 (Señal Naranja)
#define PIN_TRIG 16        // Pin físico 21
#define PIN_ECHO 17        // Pin físico 22
#define PIN_BUZZER 14      // Pin físico 19
#define PIN_PULSADOR 22    // Pin físico 29
#define PIN_LED_ROJO 18    // Pin físico 24
#define PIN_LED_VERDE 19   // Pin físico 25
#define PIN_LED_AMA 20     // Pin físico 26
#define PIN_LED_NAR 21     // Pin físico 27

// Función para medir distancia en cm
float get_distance() {
    gpio_put(PIN_TRIG, 1);
    sleep_us(10);
    gpio_put(PIN_TRIG, 0);

    uint32_t s = time_us_32();
    while (gpio_get(PIN_ECHO) == 0 && (time_us_32() - s < 30000));
    uint32_t start = time_us_32();
    while (gpio_get(PIN_ECHO) == 1 && (time_us_32() - s < 30000));
    uint32_t end = time_us_32();

    return (float)(end - start) * 0.01715f;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // 1. Configurar LEDs y Buzzer
    uint pins_out[] = {PIN_BUZZER, PIN_TRIG, PIN_LED_ROJO, PIN_LED_VERDE, PIN_LED_AMA, PIN_LED_NAR};
    for(int i=0; i<6; i++) {
        gpio_init(pins_out[i]);
        gpio_set_dir(pins_out[i], GPIO_OUT);
    }

    // 2. Configurar Entradas (Botón y Echo)
    gpio_init(PIN_PULSADOR);
    gpio_set_dir(PIN_PULSADOR, GPIO_IN);
    gpio_pull_up(PIN_PULSADOR);
    gpio_init(PIN_ECHO);
    gpio_set_dir(PIN_ECHO, GPIO_IN);

    // 3. Configurar Servo en GP9
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(slice, 20000);
    pwm_set_clkdiv(slice, 125.0f);
    pwm_set_enabled(slice, true);

    printf("--- SUPER TEST HARDWARE INICIADO ---\n");

    while (true) {
        // Lógica del Botón (LEDs + Buzzer)
        if (gpio_get(PIN_PULSADOR) == 0) {
            gpio_put(PIN_BUZZER, 1);
            gpio_put(PIN_LED_ROJO, 1); gpio_put(PIN_LED_VERDE, 1);
            gpio_put(PIN_LED_AMA, 1); gpio_put(PIN_LED_NAR, 1);
        } else {
            gpio_put(PIN_BUZZER, 0);
            gpio_put(PIN_LED_ROJO, 0); gpio_put(PIN_LED_VERDE, 0);
            gpio_put(PIN_LED_AMA, 0); gpio_put(PIN_LED_NAR, 0);
        }

        // Lógica del Sensor (Servo)
        float dist = get_distance();
        if (dist < 15.0f && dist > 0.1f) {
            printf("Objeto detectado a %.2f cm -> ABRIENDO\n", dist);
            pwm_set_gpio_level(PIN_SERVO, 2000); // 90 grados
        } else {
            pwm_set_gpio_level(PIN_SERVO, 1000); // 0 grados
        }

        sleep_ms(100);
    }
}
