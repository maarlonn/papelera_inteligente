#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// --- MAPEO DE PINES ---
#define PIN_SERVO 15       // Pin físico 20 (GP15)
#define PIN_TRIG 16        // Pin físico 21
#define PIN_ECHO 17        // Pin físico 22
#define PIN_BUZZER 14      // Pin físico 19
#define PIN_PULSADOR 22    // Pin físico 29
#define PIN_LED_ROJO 18    // Pin físico 24
#define PIN_LED_VERDE 19   // Pin físico 25
#define PIN_LED_AMA 20     // Pin físico 26
#define PIN_LED_NAR 21     // Pin físico 27

// --- CONSTANTES ---
const int MAX_USOS = 4;
int contador = 0;
uint32_t t_cierre = 0, t_pitido = 0, t_reset = 0;
bool esta_abierta = false, pitando = false, reseteado = false;

// Función de distancia segura (No bloqueante)
float get_distance() {
    gpio_put(PIN_TRIG, 0); sleep_us(2);
    gpio_put(PIN_TRIG, 1); sleep_us(10);
    gpio_put(PIN_TRIG, 0);

    uint32_t timeout = 25000; 
    uint32_t s = time_us_32();
    while (gpio_get(PIN_ECHO) == 0 && (time_us_32() - s < timeout));
    uint32_t t1 = time_us_32();
    while (gpio_get(PIN_ECHO) == 1 && (time_us_32() - t1 < timeout));
    uint32_t t2 = time_us_32();

    return (float)(t2 - t1) * 0.01715f;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // 1. Inicializar Salidas
    uint pins_out[] = {14, 16, 18, 19, 20, 21};
    for(int i=0; i<6; i++) {
        gpio_init(pins_out[i]);
        gpio_set_dir(pins_out[i], GPIO_OUT);
        gpio_put(pins_out[i], 0);
    }

    // 2. Inicializar Entradas
    gpio_init(PIN_PULSADOR); gpio_set_dir(PIN_PULSADOR, GPIO_IN); gpio_pull_up(PIN_PULSADOR);
    gpio_init(PIN_ECHO); gpio_set_dir(PIN_ECHO, GPIO_IN);

    // 3. Configurar PWM para GP15
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint sl = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(sl, 20000); 
    pwm_set_clkdiv(sl, 125.0f); 
    pwm_set_enabled(sl, true);
    pwm_set_gpio_level(PIN_SERVO, 500); 

    while (true) {
        float dist = get_distance();
        bool btn = (gpio_get(PIN_PULSADOR) == 0);
        bool detectado = (dist < 15.0f && dist > 1.0f); 

        // Lógica de Reset Universal (5 segundos)
        if (btn) {
            if (t_reset == 0) t_reset = time_us_32();
            if (time_us_32() - t_reset > 5000000 && !reseteado) {
                contador = 0;
                gpio_put(PIN_BUZZER, 1); sleep_ms(200); gpio_put(PIN_BUZZER, 0);
                gpio_put(PIN_LED_ROJO, 0);
                pwm_set_gpio_level(PIN_SERVO, 500);
                reseteado = true;
                printf("Reset OK!\n");
            }
        } else {
            t_reset = 0;
            reseteado = false;
        }

        if (contador < MAX_USOS) {
            gpio_put(PIN_LED_VERDE, 1);
            gpio_put(PIN_LED_ROJO, 0);

            if (detectado || (btn && !reseteado)) {
                pwm_set_gpio_level(PIN_SERVO, 2400); // Abrir
                t_cierre = time_us_32();
                if (detectado) gpio_put(PIN_LED_AMA, 1);
                if (btn) gpio_put(PIN_LED_NAR, 1);

                if (!esta_abierta) {
                    gpio_put(PIN_BUZZER, 1); t_pitido = time_us_32();
                    pitando = true; contador++; esta_abierta = true;
                    printf("Abierto! Uso: %d\n", contador);
                }
            }

            if (!detectado && !btn && esta_abierta) {
                if (time_us_32() - t_cierre > 3000000) { // 3 segundos
                    pwm_set_gpio_level(PIN_SERVO, 500); // Cerrar
                    gpio_put(PIN_LED_AMA, 0); gpio_put(PIN_LED_NAR, 0);
                    esta_abierta = false;
                }
            }
        } else {
            // Bloqueo: Papelera llena
            gpio_put(PIN_LED_VERDE, 0); gpio_put(PIN_LED_ROJO, 1);
            gpio_put(PIN_LED_AMA, 0); gpio_put(PIN_LED_NAR, 0);
            pwm_set_gpio_level(PIN_SERVO, 500); 
        }

        if (pitando && (time_us_32() - t_pitido > 2000000)) {
            gpio_put(PIN_BUZZER, 0); pitando = false;
        }
        sleep_ms(20);
    }
}
