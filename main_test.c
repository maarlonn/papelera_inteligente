#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <limits.h>

#define PIN_SERVO 9
#define PIN_TRIG 16
#define PIN_ECHO 17
#define PIN_BUZZER 14
#define PIN_PULSADOR 22
#define PIN_LED_ROJO 18
#define PIN_LED_VERDE 19
#define PIN_LED_AMA 20
#define PIN_LED_NAR 21
#define PIN_PROBE_TEST 26

const int MAX_USOS = 4;
uint32_t t_cierre = 0, t_pitido = 0, t_reset = 0;
bool esta_abierta = false, pitando = false;
int contador = 0;

uint32_t test_min_lat = UINT32_MAX;
uint32_t test_max_lat = 0;
int test_muestras = 0;

float get_distance() {
    gpio_put(PIN_TRIG, 0);
    sleep_us(2);
    gpio_put(PIN_TRIG, 1);
    sleep_us(10);
    gpio_put(PIN_TRIG, 0);

    uint32_t timeout = 30000;
    uint32_t s = time_us_32();
    
    while (gpio_get(PIN_ECHO) == 0) {
        if (time_us_32() - s > timeout) return -1.0f; 
    }
    uint32_t start = time_us_32();
    
    while (gpio_get(PIN_ECHO) == 1) {
        if (time_us_32() - start > timeout) return -1.0f;
    }
    uint32_t end = time_us_32();

    return (float)(end - start) * 0.01715f;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    uint pins_out[] = {14, 16, 18, 19, 20, 21};
    for(int i=0; i<6; i++) {
        gpio_init(pins_out[i]);
        gpio_set_dir(pins_out[i], GPIO_OUT);
    }

    gpio_init(PIN_PULSADOR); gpio_set_dir(PIN_PULSADOR, GPIO_IN); gpio_pull_up(PIN_PULSADOR);
    gpio_init(PIN_ECHO); gpio_set_dir(PIN_ECHO, GPIO_IN);

    gpio_init(PIN_PROBE_TEST); gpio_set_dir(PIN_PROBE_TEST, GPIO_OUT); gpio_put(PIN_PROBE_TEST, 0);
    printf("--- SISTEMA INICIADO: MODO TEST ACTIVO ---\n");

    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint sl = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(sl, 20000); pwm_set_clkdiv(sl, 125.0f); pwm_set_enabled(sl, true);

    pwm_set_gpio_level(PIN_SERVO, 500); 

    while (true) {
        uint32_t t_inicio_lectura = time_us_32();

        float dist = get_distance();
        bool btn = (gpio_get(PIN_PULSADOR) == 0);
        bool detectado = (dist < 15.0f && dist > 1.0f); 

        if (contador < MAX_USOS) {
            gpio_put(PIN_LED_VERDE, 1);
            gpio_put(PIN_LED_ROJO, 0);

            if (detectado || btn) {
                
                if (!esta_abierta) {
                    uint32_t t_final_decision = time_us_32();
                    uint32_t latencia = t_final_decision - t_inicio_lectura;
                    
                    gpio_put(PIN_PROBE_TEST, 1);

                    if (latencia < test_min_lat) test_min_lat = latencia;
                    if (latencia > test_max_lat) test_max_lat = latencia;
                    test_muestras++;

                    printf(">> [TEST] Latencia: %d us | Jitter: %d us | Muestras: %d\n", 
                           latencia, (test_max_lat - test_min_lat), test_muestras);
                }

                pwm_set_gpio_level(PIN_SERVO, 2400); 

                gpio_put(PIN_PROBE_TEST, 0); 

                t_cierre = time_us_32();
                
                if (detectado) gpio_put(PIN_LED_AMA, 1);
                if (btn) gpio_put(PIN_LED_NAR, 1);

                if (!esta_abierta) {
                    gpio_put(PIN_BUZZER, 1); t_pitido = time_us_32();
                    pitando = true; contador++; esta_abierta = true;
                    printf("Distancia: %.2f cm | Uso: %d\n", dist, contador);
                }
            }

            if (!detectado && !btn && esta_abierta) {
                if (time_us_32() - t_cierre > 3000000) { 
                    pwm_set_gpio_level(PIN_SERVO, 500); 
                    gpio_put(PIN_LED_AMA, 0); gpio_put(PIN_LED_NAR, 0);
                    esta_abierta = false;
                }
            }
        } else {
            gpio_put(PIN_LED_VERDE, 0); gpio_put(PIN_LED_ROJO, 1);
            gpio_put(PIN_LED_AMA, 0); gpio_put(PIN_LED_NAR, 0);
            pwm_set_gpio_level(PIN_SERVO, 500); 

            if (btn) {
                if (t_reset == 0) t_reset = time_us_32();
                if (time_us_32() - t_reset > 5000000) {
                    contador = 0; t_reset = 0;
                    gpio_put(PIN_BUZZER, 1); sleep_ms(200); gpio_put(PIN_BUZZER, 0);
                    
                    test_min_lat = UINT32_MAX; test_max_lat = 0; test_muestras = 0;
                    printf("--- [TEST] Estadisticas reiniciadas ---\n");
                }
            } else { t_reset = 0; }
        }

        if (pitando && (time_us_32() - t_pitido > 2000000)) {
            gpio_put(PIN_BUZZER, 0); pitando = false;
        }
        sleep_ms(50); 
    }
}
