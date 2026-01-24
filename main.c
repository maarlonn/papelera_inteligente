/*
 * Proyecto: Papelera Inteligente SCTR
 * Estructura: Table-Driven FSM
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// --- CONFIGURACIÓN DE PINES ---
#define PIN_SERVO 15
#define PIN_TRIG 16
#define PIN_ECHO 17
#define PIN_LED_ROJO 18    // Estado: LLENA
#define PIN_LED_VERDE 19   // Estado: VACÍA (esperando)
#define PIN_LED_AMA 20     // Apertura: AUTOMÁTICA
#define PIN_LED_NAR 21     // Apertura: MANUAL
#define PIN_BUZZER 14
#define PIN_PULSADOR 22

// --- GLOBALES ---
int contador_aperturas = 0;
const int MAX_APERTURAS = 3;
const float DISTANCIA_UMBRAL = 20.0; // cm

/* ============================
 * 1. DEFINICIÓN DE ESTADOS Y EVENTOS
 * ============================ */
enum state { CERRADA = 0, ABIERTA, LLENA, STATE_MAX };
enum event { NONE = 0, DETECTO_MANO, PULSO_BOTON, TIEMPO_AGOTADO, EVENT_MAX };

/* ============================
 * 2. ACCIONES DE HARDWARE (DRIVERS)
 * ============================ */
void mover_servo(int grados) {
    // 0 grados -> 500us, 90 grados -> 1500us (aprox)
    uint slice_num = pwm_gpio_to_slice_num(PIN_SERVO);
    uint32_t duty = 500 + (grados * 1000 / 90);
    pwm_set_gpio_level(PIN_SERVO, (duty * 20000 / 20000)); // Ajuste duty cycle
}

float read_distance() {
    gpio_put(PIN_TRIG, 1);
    sleep_us(10);
    gpio_put(PIN_TRIG, 0);

    while (gpio_get(PIN_ECHO) == 0);
    uint32_t start = time_us_32();
    while (gpio_get(PIN_ECHO) == 1);
    uint32_t end = time_us_32();

    return (float)(end - start) * 0.01715f;
}

/* ============================
 * 3. MANEJADORES DE TRANSICIÓN
 * ============================ */
enum state trans_cerrada_a_abierta_auto(void) {
    contador_aperturas++;
    gpio_put(PIN_LED_AMA, 1);
    mover_servo(90);
    printf("Apertura Automatica. Contador: %d\n", contador_aperturas);
    return ABIERTA;
}

enum state trans_cerrada_a_abierta_manual(void) {
    contador_aperturas++;
    gpio_put(PIN_LED_NAR, 1);
    mover_servo(90);
    printf("Apertura Manual. Contador: %d\n", contador_aperturas);
    return ABIERTA;
}

enum state trans_abierta_a_cerrada(void) {
    mover_servo(0);
    gpio_put(PIN_LED_AMA, 0);
    gpio_put(PIN_LED_NAR, 0);
    
    if (contador_aperturas >= MAX_APERTURAS) {
        gpio_put(PIN_LED_VERDE, 0);
        gpio_put(PIN_LED_ROJO, 1);
        printf("Papelera LLENA\n");
        return LLENA;
    }
    return CERRADA;
}

/* ============================
 * 4. TABLA DE TRANSICIONES
 * ============================ */
enum state (*trans_table[STATE_MAX][EVENT_MAX])(void) = {
    [CERRADA] = {
        [DETECTO_MANO] = trans_cerrada_a_abierta_auto,
        [PULSO_BOTON]  = trans_cerrada_a_abierta_manual,
    },
    [ABIERTA] = {
        [TIEMPO_AGOTADO] = trans_abierta_a_cerrada,
    },
    [LLENA] = { /* Bloqueada */ },
};

/* ============================
 * 5. PARSEADOR DE EVENTOS
 * ============================ */
enum event get_event(void) {
    if (!gpio_get(PIN_PULSADOR)) return PULSO_BOTON;
    if (read_distance() < DISTANCIA_UMBRAL) return DETECTO_MANO;
    return NONE;
}

/* ============================
 * 6. INICIALIZACIÓN Y MAIN
 * ============================ */
void init_hw() {
    stdio_init_all();
    
    // LEDs y Sensores
    uint32_t leds[] = {PIN_LED_ROJO, PIN_LED_VERDE, PIN_LED_AMA, PIN_LED_NAR};
    for(int i=0; i<4; i++) {
        gpio_init(leds[i]);
        gpio_set_dir(leds[i], GPIO_OUT);
    }
    
    gpio_init(PIN_TRIG); gpio_set_dir(PIN_TRIG, GPIO_OUT);
    gpio_init(PIN_ECHO); gpio_set_dir(PIN_ECHO, GPIO_IN);
    
    gpio_init(PIN_PULSADOR);
    gpio_set_dir(PIN_PULSADOR, GPIO_IN);
    gpio_pull_up(PIN_PULSADOR);

    // PWM para el Servo
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PIN_SERVO);
    pwm_set_wrap(slice, 20000); // 20ms periodo
    pwm_set_clkdiv(slice, 125.0f); // 1MHz
    pwm_set_enabled(slice, true);
    
    mover_servo(0); // Posición inicial
    gpio_put(PIN_LED_VERDE, 1);
}

int main() {
    init_hw();
    enum state st = CERRADA;
    uint32_t timer_abierta = 0;

    for (;;) {
        enum event ev = get_event();
        
        // Lógica de la Tabla
        if (ev != NONE && trans_table[st][ev] != NULL) {
            st = trans_table[st][ev];
            if (st == ABIERTA) timer_abierta = time_us_32();
        }

        // Lógica del Timeout (Tiempo Agotado)
        if (st == ABIERTA && (time_us_32() - timer_abierta > 5000000)) { // 5 segundos
            if (trans_table[st][TIEMPO_AGOTADO] != NULL) {
                st = trans_table[st][TIEMPO_AGOTADO];
            }
        }
        
        sleep_ms(100);
    }
}