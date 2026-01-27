#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// --- CONFIGURACIÓN DE PINES ---
#define PIN_SERVO 9       // Pin físico 12 (Señal)
#define PIN_TRIG 16       // Pin físico 21
#define PIN_ECHO 17       // Pin físico 22
#define PIN_BUZZER 14     // Pin físico 19
#define PIN_PULSADOR 22   // Pin físico 29
#define PIN_LED_ROJO 18   // Pin físico 24
#define PIN_LED_VERDE 19  // Pin físico 25
#define PIN_LED_AMA 20    // Pin físico 26
#define PIN_LED_BLANCO 21 // Pin físico 27 (Tu LED blanco)

// --- VARIABLES DE CONTROL ---
int contador_aperturas = 0;
const int MAX_APERTURAS = 4; // Límite de 4 veces
uint32_t t_cierre = 0;       // Temporizador para cerrar tapa
uint32_t t_buzzer = 0;       // Temporizador para el pitido
bool sistema_bloqueado = false;
bool ya_contado = false;     // Evita que sume más de 1 por apertura

float get_distance() {
    gpio_put(PIN_TRIG, 1);
    sleep_us(10);
    gpio_put(PIN_TRIG, 0);
    uint32_t s = time_us_32();
    while (gpio_get(PIN_ECHO) == 0 && (time_us_32() - s < 30000));
    uint32_t start = time_us_32();
    while (gpio_get(PIN_ECHO) == 1 && (time_us_32() - s < 30000));
    return (float)(time_us_32() - start) * 0.01715f;
}

int main() {
    stdio_init_all();
    
    // Inicialización de Salidas
    uint outs[] = {14, 16, 18, 19, 20, 21};
    for(int i=0; i<6; i++) { gpio_init(outs[i]); gpio_set_dir(outs[i], GPIO_OUT); }
    
    // Inicialización de Entradas
    gpio_init(PIN_ECHO); gpio_set_dir(PIN_ECHO, GPIO_IN);
    gpio_init(PIN_PULSADOR); gpio_set_dir(PIN_PULSADOR, GPIO_IN); gpio_pull_up(PIN_PULSADOR);
    
    // Configuración Servo
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(slice, 20000); pwm_set_clkdiv(slice, 125.0f); pwm_set_enabled(slice, true);

    gpio_put(PIN_LED_VERDE, 1); // Empezamos listos

    while (true) {
        if (!sistema_bloqueado) {
            float dist = get_distance();
            bool boton = (gpio_get(PIN_PULSADOR) == 0);
            bool detectado = (dist < 15.0f && dist > 0.1f);

            // --- LÓGICA DE APERTURA ---
            if (detectado || boton) {
                pwm_set_gpio_level(PIN_SERVO, 2000); // Abrir
                t_cierre = time_us_32(); // Reiniciar cuenta de 5s
                
                if (detectado) gpio_put(PIN_LED_AMA, 1);
                if (boton) gpio_put(PIN_LED_BLANCO, 1);

                // Activar Buzzer 2 segundos si acaba de empezar la detección
                if (!ya_contado) {
                    t_buzzer = time_us_32();
                    gpio_put(PIN_BUZZER, 1);
                    contador_aperturas++;
                    ya_contado = true;
                    printf("Uso detectado! Contador: %d\n", contador_aperturas);
                }
            }

            // --- LÓGICA DE CIERRE (Temporizador 5s) ---
            if (!detectado && !boton && ya_contado) {
                if (time_us_32() - t_cierre > 5000000) { // 5 segundos
                    pwm_set_gpio_level(PIN_SERVO, 1000); // Cerrar
                    gpio_put(PIN_LED_AMA, 0);
                    gpio_put(PIN_LED_BLANCO, 0);
                    ya_contado = false; // Listo para la próxima apertura
                    
                    if (contador_aperturas >= MAX_APERTURAS) {
                        sistema_bloqueado = true;
                    }
                }
            }
        } else {
            // --- ESTADO LLENA ---
            gpio_put(PIN_LED_VERDE, 0);
            gpio_put(PIN_LED_ROJO, 1); // Solo luz roja permanente
            pwm_set_gpio_level(PIN_SERVO, 1000); // Asegurar cierre
        }

        // Control del Buzzer (Apagar a los 2 segundos)
        if (gpio_get(PIN_BUZZER) && (time_us_32() - t_buzzer > 2000000)) {
            gpio_put(PIN_BUZZER, 0);
        }

        sleep_ms(50);
    }
}