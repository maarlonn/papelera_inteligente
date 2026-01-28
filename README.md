#  Papelera Inteligente (SCTR)

Proyecto de sistema embebido basado en **Raspberry Pi Pico** que automatiza la apertura de una papelera y gestiona su estado de llenado mediante una arquitectura de control en tiempo real.

##  Integrantes
* **Marlon Barco Bernal** - Responsable de Software / Integración
* **Mario Lago Fernández** - Responsable de Hardware / Software
* **Pablo Martínez Fernández** - Responsable de Hardware / Documentación
* **Xavier Pardo Guerreiro** - Responsable de Documentación / Validación

##  Funcionalidades 
* **Apertura Dual:** Detección por ultrasonidos (Auto) o pulsador físico (Manual).
* **Sistema de Bloqueo Progresivo:** El sistema contabiliza los usos y entra en modo de seguridad (Bloqueo) al alcanzar los **4 usos** para evitar el desbordamiento.
* **Reset de Seguridad:** Reinicio del contador de usos y desbloqueo mediante pulsación larga de **5 segundos**.
* **Indicadores LED de Estado:**
    * 🟢 **Verde:** Sistema operativo y con capacidad disponible (GP19).
    * 🟡 **Amarillo:** Apertura automática detectada (GP20).
    * 🟠 **Naranja:** Apertura manual detectada (GP21).
    * 🔴 **Rojo:** Papelera **LLENA**. Bloqueo de seguridad activo (GP18).
* **Feedback Sonoro:** Buzzer activo (GP14) durante 2 segundos en cada apertura y pitido de confirmación tras reset.

##  Conexión de Hardware (Pinout Final)

| Componente | Pin GPIO | Pin Físico | Función Técnica |
| :--- | :--- | :--- | :--- |
| **Servomotor** | **GPIO 15** | **20** | Control de posición (PWM) |
| **HC-SR04 (Trig)** | **GPIO 16** | **21** | Disparo de pulso sónico |
| **HC-SR04 (Echo)** | **GPIO 17** | **22** | Recepción de eco |
| **Buzzer** | **GPIO 14** | **19** | Alerta acústica |
| **LED Rojo** | **GPIO 18** | **24** | Indicador de Bloqueo / Llena |
| **LED Verde** | **GPIO 19** | **25** | Indicador de Disponibilidad |
| **LED Amarillo**| **GPIO 20** | **26** | Indicador Apertura Auto |
| **LED Naranja** | **GPIO 21** | **27** | Indicador Apertura Manual |
| **Pulsador** | **GPIO 22** | **29** | Entrada Manual / Reset (Pull-up) |



##  Lógica de Implementación
El firmware utiliza un bucle de control no bloqueante para garantizar la respuesta del sistema:
1. **Detección de Distancia:** Rango operativo configurado entre 1.0 cm y 15.0 cm con protección por *timeout* de 20ms para evitar cuelgues del sensor.
2. **Control de Servo:** Configuración de PWM con un ciclo de trabajo de 500us (cerrado) a 2400us (abierto).
3. **Temporización:** Cierre automático tras 3 segundos de inactividad.

##  Estructura del Proyecto
* **Rama `main.c`**: Código definitivo con la lógica de control integrada (4 usos y reset de 5s).
* **Rama `pruebas_unitarias/`**: Tests individuales para validación de cada componente hardware.
* **Rama `estudio-latencia`**: Contiene el código de telemetría y el informe técnico sobre Jitter y tiempos de respuesta.

