cmake_minimum_required(VERSION 3.13)

# 1. Importar el SDK de la Raspberry Pi Pico
include(pico_sdk_import.cmake)

project(papelera_inteligente C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 2. Inicializar el SDK
pico_sdk_init()

# 3. Definir el archivo principal de vuestro código
add_executable(papelera_inteligente
    main.c
)

# 4. Enlazar las librerías necesarias para vuestro hardware (Rúbrica: Calidad)
# pico_stdlib: Funciones básicas (GPIO, tiempo, monitor serie)
# hardware_pwm: Necesario para mover el servomotor
# hardware_gpio: Control de LEDs, Pulsador y Buzzer
# hardware_timer: Para medir con precisión el eco del HC-SR04
target_link_libraries(papelera_inteligente 
    pico_stdlib 
    hardware_pwm 
    hardware_gpio
    hardware_timer
)

# 5. Generar los archivos finales (.uf2 para la placa)
pico_add_extra_outputs(papelera_inteligente)