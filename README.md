# Link do vídeo: https://drive.google.com/file/d/1wCkYE6Cse_xFmHuDovuxqz2HyK546rOW/view?usp=sharing

# Controle de LEDs, Matriz WS2812 e Display SSD1306 com Raspberry Pi Pico W na placa BitDogLab

# Descrição:
- Este projeto implementa um sistema de controle de LEDs, display OLED SSD1306 e matriz de LEDs WS2812 utilizando a placa Raspberry Pi Pico W. O sistema permite interação através de botões para acionar LEDs individuais, digitação de caracteres (maiúsculos, minúsculos e numéricos), além de exibir mensagens no display SSD1306, no serial monitor e na matriz LED WS2812.

# Componentes Utilizados:
- Raspberry Pi Pico W.
- Display OLED SSD1306 (conectado via I2C).
- Matriz de LEDs WS2812 (5x5).
- LEDs GPIO 11 (verde) e GPIO 12 (azul).
- Joystick GPIO 22.
- Botão A (GPIO 5) e Botão B (GPIO 6).
- Serial Monitor.
- Resistores de pull-up (integrados via software para botões).

# Funcionalidades:
- Botão A pressionado: a luz Verde liga/desliga e aparece uma mensagem tanto no serial monitor quanto no display SSD1306 informando seu estado.
- Botão B pressionado: a luz Azul liga/desliga e aparece uma mensagem tanto no serial monitor quanto no display SSD1306 informando seu estado.
- Os caracteres (maiúsculos/minúsculos) quando digitados, um de cada vez, aparecem tanto no serial monitor quanto no display SSD1306.
- Quando os caracteres numéricos são digitados, o número correspondente aparece no serial monitor, no display SSD1306 e também na matriz de LEDs WS2812.
- Botão do Joystick pressionado: apaga a Matriz led WS2812 com o número correspondente e aparece uma mensagem no serial monitor e no display SSD1306 informando seu estado.
