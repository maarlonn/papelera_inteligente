# Rama de Validacion de Hardware

Esta rama contiene las pruebas unitarias para verificar el funcionamiento individual y conjunto de los perifericos de la Papelera Inteligente.

## Estructura de Pruebas
Los codigos se encuentran en la carpeta pruebas_unitarias/:
* **test_led_blink.c**: Verificacion basica de salida digital (LED Verde).
* **test_boton_alarma.c**: Prueba de entrada (Pulsador) y salida de potencia (Buzzer).
* **test_sensor_servo.c**: Validacion de la logica principal (Deteccion de distancia y movimiento PWM).
* **test_sistema_completo.c**: Test de integracion de todos los componentes de forma simultanea.

## Mapeo de Pines Utilizado
| Componente | GPIO | Pin Fisico |
| :--- | :--- | :--- |
| Servomotor | 9 | 12 |
| Sensor (Trig) | 16 | 21 |
| Sensor (Echo) | 17 | 22 |
| Buzzer | 14 | 19 |
| Pulsador | 22 | 29 |
| LED Rojo | 18 | 24 |
| LED Verde | 19 | 25 |
| LED Amarillo | 20 | 26 |
| LED Naranja | 21 | 27 |

## Instrucciones de Uso
Para ejecutar cualquiera de las pruebas disponibles:
1. Copiar el archivo de interes al archivo principal:
   `cp pruebas_unitarias/nombre_del_test.c main.c`
2. Realizar el proceso de compilacion mediante CMake y Make.
3. Cargar el archivo .uf2 resultante en la Raspberry Pi Pico.
