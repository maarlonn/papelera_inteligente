# Sistema de Control de Acceso con Test de Latencia

Este proyecto implementa un control de barrera mediante servo y sensor ultrasónico utilizando la Raspberry Pi Pico SDK.

## Explicación del Test de Rendimiento (Testing)

El código incluye un módulo de instrumentación para medir el rendimiento en tiempo real del bucle de control ("Hardware in the loop"). El objetivo es validar que el sistema reacciona dentro de los márgenes de tiempo esperados.

### Métricas capturadas:
1.  **Latencia de Decisión:** Se mide el tiempo exacto en microsegundos (`us`) desde el inicio del bucle de lectura (`t_inicio_lectura`) hasta el momento en que se toma la decisión de abrir la barrera (`t_final_decision`).
2.  **Jitter (Variación):** Se calcula la diferencia entre la latencia máxima y mínima registrada durante la ejecución (`test_max_lat - test_min_lat`).
3.  **Trigger de Hardware (Probe):** El **GPIO 26 (`PIN_PROBE_TEST`)** actúa como una sonda física. Se pone en ALTO (High) justo antes de mover el servo y en BAJO (Low) inmediatamente después. Esto permite conectar un **osciloscopio** para medir externamente el tiempo de respuesta eléctrica y contrastarlo con los logs de software.

### Salida de datos:
Los resultados se imprimen por puerto serie (USB) con el formato:
`>> [TEST] Latencia: X us | Jitter: Y us | Muestras: Z`
