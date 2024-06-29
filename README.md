# SE_P101_PROYG1
##Introducción
##Justificación
La música es una forma universal de expresión que trasciende barreras culturales y lingüísticas. Sin embargo, la educación musical puede ser costosa y requiere acceso a múltiples instrumentos para ofrecer una experiencia completa. Un teclado musical interactivo que permite seleccionar el sonido de diferentes instrumentos (piano, guitarra, batería) a través de una aplicación móvil proporciona una solución económica y accesible. Este proyecto no solo facilita el aprendizaje musical, sino que también fomenta la creatividad y la innovación al integrar tecnología moderna en la enseñanza de la música.

###Descripción
El proyecto consiste en desarrollar un teclado musical interactivo que permita al usuario seleccionar el tipo de instrumento que desea que suene a través de una aplicación móvil y con botones de selección de manera física. La selección de los instrumentos se comunicará al teclado mediante tecnología Wi-Fi, utilizando un módulo ESP32.

El teclado contará con los siguientes elementos:

- Sonidos: Piano, guitarra y batería. Los sonidos serán generados mediante señales PWM en el código y transmitidas mediante el pin DAC del ESP32.
- Aplicación móvil: Permitirá cambiar entre estos sonidos en tiempo real.
- Botón de selección: Permitirá la selección de los tres instrumentos disponibles de forma física.
- Pantalla OLED de 0.96 pulgadas: Mostrará el instrumento seleccionado.
- Switch ON/OFF: Para encender y apagar el dispositivo.
- Batería recargable: Garantizará la portabilidad.
- Monitorización de la batería: El porcentaje de la batería del teclado musical se enviará a una plataforma en la nube como Ubidots, donde se visualizará una gráfica con el comportamiento de descarga del dispositivo.
##Especificaciones
##Alimentación
- Batería: Sí, batería recargable de iones de litio.
- Rango de voltaje de operación: 3.7V a 4.2V (nominal de 3.7V).
- Cálculos de potencia: Consumo estimado de 1W, batería de 2000mAh proporcionará aproximadamente 4 horas de uso continuo.
##Tecnología Utilizada
- Microcontrolador: ESP32.
- Comunicación: WiFi (modo AP), capacidad de almacenar al menos dos redes WiFi en memoria EEPROM.
- Sensores: Pulsadores para las teclas del teclado.
- Aplicación Móvil: Desarrollada para Android, permite seleccionar el tipo de instrumento.
##Presupuesto Inicial para Primer Avance
- Microcontrolador ESP32 (2 en caso de fallas): $30
- Batería de iones de litio 2000mAh: $10
- Pulsadores: $2
- Componentes electrónicos adicionales (resistencias, capacitores, etc.): $10
- Altavoz de 8 Ohm: $7
- Total estimado: $59
  
##Objetivos
##Objetivo General
Diseñar un teclado musical interactivo con capacidad de simular los diferentes instrumentos musicales (piano, guitarra y batería), que permita la manipulación del instrumento emulado mediante una aplicación móvil y/o teclado físico.

##Objetivos Específicos
- Integrar un microcontrolador ESP32 a una pantalla OLED para mostrar en pantalla el instrumento seleccionado.
- Implementar una batería portátil para la alimentación del ESP32, proporcionando facilidad y portabilidad al proyecto.
- Implementar la funcionalidad de selección de instrumentos (piano, guitarra, batería) y la comunicación en tiempo real con el teclado musical a través de WiFi.
- Desarrollar un sistema para enviar y almacenar datos de la batería, permitiendo la visualización del comportamiento de descarga a través de gráficos en tiempo real.
- Configurar el módulo ESP32 para conectarse a una plataforma en la nube como Ubidots para el monitoreo del estado de la batería del teclado musical.
