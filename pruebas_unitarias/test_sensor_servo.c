#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>

#define PIN_SERVO 9
#define PIN_TRIG 16
#define PIN_ECHO 17

void set_servo_angle(int angle) {
    uint slice = pwm_gpio_to_slice_num(PIN_SERVO);
    // 500us a 2500us es el rango estándar para servos (0.5ms a 2.5ms)
    uint32_t duty = 500 + (angle * 2000 / 180);
    pwm_set_gpio_level(PIN_SERVO, duty);
}

float get_distance() {
    gpio_put(PIN_TRIG, 1);
    sleep_us(10);
    gpio_put(PIN_TRIG, 0);

    while (gpio_get(PIN_ECHO) == 0);
    uint32_t start = time_us_32();
    while (gpio_get(PIN_ECHO) == 1);
    uint32_t end = time_us_32();

    return (float)(end - start) * 0.01715f;
}

int main() {
    stdio_init_all();

    // Configurar Sensor
    gpio_init(PIN_TRIG);
    gpio_set_dir(PIN_TRIG, GPIO_OUT);
    gpio_init(PIN_ECHO);
    gpio_set_dir(PIN_ECHO, GPIO_IN);

    // Configurar PWM para Servo en GPIO 9
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(slice, 20000); // 20ms periodo
    pwm_set_clkdiv(slice, 125.0f); // 1MHz
    pwm_set_enabled(slice, true);

    set_servo_angle(0); // Posición inicial cerrada

    while (true) {
        float dist = get_distance();
        
        if (dist < 20.0f && dist > 0) { // Si detecta algo a menos de 20cm
            set_servo_angle(90);       // Abre tapa
            sleep_ms(2000);            // Mantiene abierta 2 segundos
            set_servo_angle(0);        // Cierra tapa
        }
        sleep_ms(100);
    }
}
