# 🗑️ Papelera Inteligente (SCTR)

Proyecto de sistema embebido basado en **Raspberry Pi Pico** que automatiza la apertura de una papelera y gestiona su estado de llenado.

## 👥 Integrantes
* **Marlon Barco Bernal** - Responsable de Software / Integración
* **Mario Lago Fernández** - Responsable de Hardware / Software
* **Pablo Martínez Fernández** - Responsable de Hardware / Documentación
* **Xavier Pardo Guerreiro** - Responsable de Documentación / Validación

## 🚀 Funcionalidades
* **Apertura Dual:** Mediante sensor de ultrasonidos (Auto) o pulsador (Manual).
* **Indicadores LED:**
    * 🟢 Verde: Lista / Vacía.
    * 🟡 Amarillo: Apertura automática detectada.
    * 🟠 Naranja: Apertura manual detectada.
    * 🔴 Rojo: Papelera LLENA (Bloqueo tras 3 usos).
* **Lógica FSM:** Implementación basada en tablas (**Table-Driven FSM**).

## 🔌 Conexión de Hardware (Pinout)
| Componente | Pin GPIO |
| :--- | :--- |
| Servomotor | GPIO 15 |
| HC-SR04 (Trig) | GPIO 16 |
| HC-SR04 (Echo) | GPIO 17 |
| LED Rojo | GPIO 18 |
| LED Verde | GPIO 19 |
| LED Amarillo | GPIO 20 |
| LED Naranja | GPIO 21 |
| Pulsador | GPIO 22 |

## 🛠️ Instalación y Compilación
1. Clonar el repositorio:
   ```bash
   git clone [URL_DE_TU_REPO]
