#include <U8g2lib.h>
#include <Wire.h>
#include <Arduino.h>
#include <pgmspace.h>
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"
#include <BluetoothSerial.h>
#include <esp_bt_device.h>

// Definición del OLED (pantalla de 128x64 píxeles, con controlador SH1106, utilizando I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); // Crear objeto OLED

// Creación de objetos para el reproductor de audio DFPlayer Mini y para la comunicación Bluetooth
DFRobotDFPlayerMini player;  // Objeto para el control del DFPlayer Mini
BluetoothSerial SerialBT;    // Objeto para la comunicación Bluetooth

// Definición de modos y variables globales
int modos[] = {0, 1, 2}; // Array de modos: 0 = PIANO, 1 = BATERIA, 2 = GUITARRA
int modoactual = modos[0]; // Modo actual (inicialmente PIANO)
volatile bool actualizar_lcd = false; // Bandera para indicar si se necesita actualizar la pantalla OLED
int notas[] = {2, 26, 4, 5, 14, 15, 13}; // Pines GPIO conectados a los pulsadores que representan las notas
volatile int nota_presionada = -1; // Variable para almacenar la nota presionada (inicialmente ninguna)

// ISR (Interrupción de Servicio Rápido) para cada pulsador de nota, asignando el valor correspondiente a nota_presionada
void IRAM_ATTR isr_nota_presionada_0() { nota_presionada = 0; }
void IRAM_ATTR isr_nota_presionada_1() { nota_presionada = 1; }
void IRAM_ATTR isr_nota_presionada_2() { nota_presionada = 2; }
void IRAM_ATTR isr_nota_presionada_3() { nota_presionada = 3; }
void IRAM_ATTR isr_nota_presionada_4() { nota_presionada = 4; }
void IRAM_ATTR isr_nota_presionada_5() { nota_presionada = 5; }
void IRAM_ATTR isr_nota_presionada_6() { nota_presionada = 6; }

// Función de inicialización del DFPlayer Mini
void setup_player() {
    Serial2.begin(9600); // Iniciar la comunicación serial con el DFPlayer Mini a 9600 baudios
    delay(100); // Esperar 100 ms para asegurar la conexión

    // Intentar conectarse al DFPlayer Mini
    while (!player.begin(Serial2)) {
        Serial.println("Error al conectar con DFPlayer Mini"); // Mensaje de error si no se conecta
    }

    // Configurar el volumen y mostrar mensaje de éxito
    player.volume(25); // Configurar el volumen a 25 (máximo es 30)
    Serial.println("DFPlayer conectado exitosamente!");
}

// Código para leer el voltaje de la batería
const int batteryPin = 35; // GPIO35 (ADC1_CH0) utilizado para leer el voltaje de la batería
const float voltageDividerFactor = 1.4; // Factor de corrección del divisor de voltaje

// Función para leer el voltaje de la batería
float readBatteryVoltage() {
    int analogValue = analogRead(batteryPin); // Leer valor analógico del pin de la batería
    float voltage = analogValue * (3.3 / 4095.0) * voltageDividerFactor; // Calcular el voltaje real
    return voltage;
}

// Función para calcular el porcentaje de batería basado en el voltaje leído
int calculateBatteryPercentage(float voltage) {
    const float minVoltage = 3.0; // Voltaje mínimo (batería vacía)
    const float maxVoltage = 4.2; // Voltaje máximo (batería cargada)

    // Convertir el voltaje en porcentaje de batería
    if (voltage <= minVoltage) {
        return 0; // Batería vacía
    } else if (voltage >= maxVoltage) {
        return 100; // Batería cargada
    } else {
        return (int)((voltage - minVoltage) / (maxVoltage - minVoltage) * 100); // Calcular porcentaje
    }
}

// Función de inicialización del Bluetooth
void setup_bluetooth() {
    SerialBT.begin("ESP32_Piano"); // Iniciar Bluetooth con el nombre "ESP32_Piano"
    
    // Esperar hasta que un cliente se conecte
    while (!SerialBT.hasClient()) {
        Serial.println("Esperando Bluetooth..."); // Mostrar mensaje de espera
    }
    Serial.println("Conectado"); // Mostrar mensaje cuando se conecte un cliente
}

// Función para actualizar la pantalla OLED con el modo actual
void actualizarLCD() {
    u8g2.clearBuffer(); // Limpiar el buffer de la pantalla
    u8g2.setFont(u8g2_font_logisoso16_tr); // Configurar la fuente de texto

    // Mostrar el modo actual en la pantalla
    if (modoactual == 0) {
        u8g2.drawStr(5, 35, "PIANO");
    } else if (modoactual == 1) {
        u8g2.drawStr(5, 35, "BATERIA");
    } else if (modoactual == 2) {
        u8g2.drawStr(5, 35, "GUITARRA");
    }

    u8g2.sendBuffer(); // Enviar el buffer a la pantalla para actualizarla
}

// Función para cambiar el modo del instrumento
void cambiarModo() {
    if (modoactual == 2) {
        modoactual = 0; // Si el modo actual es GUITARRA, volver a PIANO
    } else {
        modoactual++; // Cambiar al siguiente modo
    }
    actualizar_lcd = true; // Indicar que la pantalla necesita ser actualizada
}

// Función para enviar el porcentaje de batería vía Bluetooth
void enviarBateriaBlutu() {
    float batteryVoltage = readBatteryVoltage(); // Leer el voltaje de la batería
    int batteryPercentage = calculateBatteryPercentage(batteryVoltage); // Calcular el porcentaje de batería
    SerialBT.print("B"); // Enviar indicador de batería
    SerialBT.println(batteryPercentage); // Enviar el porcentaje de batería
}

// Función para actualizar el modo vía Bluetooth
void actualizarModoBlutu() {
    // Verificar si hay datos disponibles en el Bluetooth
    if (SerialBT.available()) {
        char incomingChar = SerialBT.read(); // Leer el carácter entrante

        // Si el carácter es 'C', cambiar el modo
        if (incomingChar == 'C') {
            cambiarModo();
        }
    }
}

// ISR (Interrupción de Servicio Rápido) para cambiar el modo con un pulsador
void IRAM_ATTR isr() {
    cambiarModo(); // Cambiar el modo cuando se detecte una interrupción
}

// Configuración inicial del sistema
void setup() {
    Serial.begin(115200); // Iniciar el puerto serie para depuración (opcional)
    Serial.println("Iniciando..."); // Mensaje de inicio

    // Inicialización de la pantalla OLED
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(5, 35, "Bluetooth...");
    u8g2.sendBuffer();

    // Inicializar componentes
    setup_bluetooth(); // Configurar Bluetooth
    u8g2.clearBuffer();
    u8g2.drawStr(5, 35, "Iniciando...");
    u8g2.sendBuffer();
    setup_player(); // Configurar DFPlayer Mini
    actualizarLCD(); // Mostrar el modo inicial en la pantalla OLED

    // Configuración de pines
    pinMode(pulsadorCambioModo, INPUT_PULLUP); // Configurar pin para cambio de modo
    for (int i = 0; i < 7; i++) {
        pinMode(notas[i], INPUT_PULLUP); // Configurar pines de los pulsadores de notas
    }

    // Configuración de interrupciones para los pulsadores
    attachInterrupt(digitalPinToInterrupt(pulsadorCambioModo), isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[0]), isr_nota_presionada_0, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[1]), isr_nota_presionada_1, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[2]), isr_nota_presionada_2, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[3]), isr_nota_presionada_3, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[4]), isr_nota_presionada_4, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[5]), isr_nota_presionada_5, FALLING);
    attachInterrupt(digitalPinToInterrupt(notas[6]), isr_nota_presionada_6, FALLING);

    Serial.println("Setup completado."); // Mensaje al completar la configuración
}

// Bucle principal del programa
void loop() {
    actualizarModoBlutu(); // Verificar si hay comandos para cambiar el modo vía Bluetooth

    // Si es necesario, actualizar la pantalla OLED
    if (actualizar_lcd) {
        actualizarLCD(); // Actualizar la pantalla OLED
        actualizar_lcd = false; // Restablecer la bandera de actualización de pantalla
    }

    // Si una nota fue presionada, reproducir el sonido correspondiente
    if (nota_presionada != -1) {
        int nota = nota_presionada; // Obtener la nota presionada
        nota_presionada = -1; // Restablecer la variable de nota presionada

        // Reproducir el sonido basado en el modo actual
        if (modoactual == 0) {
            player.play(nota + 2); // PIANO
        } else if (modoactual == 1) {
            player.play(nota + 9); // BATERIA
        } else if (modoactual == 2) {
            player.play(nota + 16); // GUITARRA
        }
    }

    // Enviar el porcentaje de batería cada 5 segundos
    static unsigned long lastBatteryCheck = 0;
    if (millis() - lastBatteryCheck > 5000) {
        lastBatteryCheck = millis();
        enviarBateriaBlutu(); // Enviar el porcentaje de batería vía Bluetooth
    }

    // Mostrar el voltaje y porcentaje de batería en el monitor serie
    float batteryVoltage = readBatteryVoltage();
    int batteryPercentage = calculateBatteryPercentage(batteryVoltage);
    Serial.print("Battery Voltage: ");
    Serial.print(batteryVoltage);
    Serial.print(" V, Battery Percentage: ");
    Serial.print(batteryPercentage);
    Serial.println(" %");

    delay(200); // Retardo para reducir la frecuencia de actualización
}
