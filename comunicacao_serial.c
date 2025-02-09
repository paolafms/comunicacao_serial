#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A_GPIO 5    // Botão A
#define BUTTON_B_GPIO 6    // Botão B
#define BUTTON_JOY_GPIO 22 // Botão do joystick
#define LED_G_GPIO 11      // Pino do LED verde
#define LED_B_GPIO 12      // Pino do LED azul

#define NUM_PIXELS 25      // Número de LEDs na matriz 5x5
#define WS2812_PIN 7       // Pino onde a matriz de LEDs está conectada

bool led_green_state = false; // Estado do LED verde
bool led_blue_state = false;  // Estado do LED azul

// Matriz de números para a matriz de LEDs
bool numeros[10][NUM_PIXELS] = {
   {0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0}, // 0

   {0, 0, 1, 0, 0,  
    0, 0, 1, 0, 0,  
    0, 0, 1, 0, 0,  
    0, 1, 1, 0, 0,  
    0, 0, 1, 0, 0}, // 1

   {0, 1, 1, 1, 0,  
    0, 1, 0, 0, 0,  
    0, 1, 1, 1, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0}, // 2

   {0, 1, 1, 1, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0}, // 3

   {0, 1, 0, 0, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 0, 1, 0}, // 4

   {0, 1, 1, 1, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 1, 0, 0, 0,  
    0, 1, 1, 1, 0}, // 5

   {0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 1, 0, 0, 0,  
    0, 1, 1, 0, 0}, // 6

   {0, 1, 0, 0, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 0, 0, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0}, // 7

   {0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0}, // 8

   {0, 1, 1, 1, 0,  
    0, 0, 0, 1, 0,  
    0, 1, 1, 1, 0,  
    0, 1, 0, 1, 0,  
    0, 1, 1, 1, 0}  // 9
};

// Controle do debounce nas interrupções
static uint32_t last_time_a = 0;
static uint32_t last_time_b = 0;
static uint32_t last_time_joy = 0;
static const uint32_t DEBOUNCE_DELAY = 200; // 200 ms

// Display e WS2812 na ISR (Interrupt Service Routine)
static ssd1306_t ssd;
static PIO pio_global;
static uint sm_global;

// Função para exibir mensagem no display
void display_message(ssd1306_t *ssd, const char *message) {
    ssd1306_fill(ssd, false);  // Limpa a tela
    ssd1306_draw_string(ssd, message, 10, 25); // Exibe a mensagem
    ssd1306_send_data(ssd);
}

// Função para acender os LEDs na matriz WS2812 conforme o número digitado
void display_number_on_leds(int number, PIO pio, uint sm, uint32_t *led_buffer) {
    if (number >= 0 && number <= 9) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (numeros[number][i]) {
                led_buffer[i] = 0x00FF00;  // Verde
            } else {
                led_buffer[i] = 0x000000;  // Apagado
            }
        }
        // Envia os dados para a matriz de LEDs
        for (int i = 0; i < NUM_PIXELS; i++) {
            pio_sm_put_blocking(pio, sm, led_buffer[i]);
        }
    }
}

// Função para apagar todos os LEDs da matriz WS2812
void apagar_leds(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, 0x000000);
    }
}

// Rotina de interrupção para os botões
void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    if (gpio == BUTTON_A_GPIO) {
        if (current_time - last_time_a < DEBOUNCE_DELAY) return;
        last_time_a = current_time;
        led_green_state = !led_green_state;
        gpio_put(LED_G_GPIO, led_green_state);
        const char *message = led_green_state ? "Verde Ligado" : "Verde Apagado";
        printf("%s\n", message);
        display_message(&ssd, message);
    } else if (gpio == BUTTON_B_GPIO) {
        if (current_time - last_time_b < DEBOUNCE_DELAY) return;
        last_time_b = current_time;
        led_blue_state = !led_blue_state;
        gpio_put(LED_B_GPIO, led_blue_state);
        const char *message = led_blue_state ? "Azul Ligado" : "Azul Apagado";
        printf("%s\n", message);
        display_message(&ssd, message);
    } else if (gpio == BUTTON_JOY_GPIO) {
        if (current_time - last_time_joy < DEBOUNCE_DELAY) return;
        last_time_joy = current_time;
        apagar_leds(pio_global, sm_global);
        printf("Matriz LED apagada\n");
        display_message(&ssd, "Matriz Apagada");
    }
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial
    sleep_ms(300);     // Aguarda estabilização da serial

    // Inicializa o display SSD1306 via I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração dos pinos dos botões e LEDs
    gpio_init(BUTTON_A_GPIO);
    gpio_set_dir(BUTTON_A_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_A_GPIO);

    gpio_init(BUTTON_B_GPIO);
    gpio_set_dir(BUTTON_B_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_B_GPIO);

    gpio_init(BUTTON_JOY_GPIO);
    gpio_set_dir(BUTTON_JOY_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_JOY_GPIO);

    gpio_init(LED_G_GPIO);
    gpio_set_dir(LED_G_GPIO, GPIO_OUT);
    gpio_put(LED_G_GPIO, 0);

    gpio_init(LED_B_GPIO);
    gpio_set_dir(LED_B_GPIO, GPIO_OUT);
    gpio_put(LED_B_GPIO, 0);

    // Inicializa a matriz de LEDs WS2812
    pio_global = pio0;
    sm_global = 0;
    uint offset = pio_add_program(pio_global, &ws2812_program);
    ws2812_program_init(pio_global, sm_global, offset, WS2812_PIN, 800000, false);

    uint32_t led_buffer[NUM_PIXELS] = {0};

    // Configura as interrupções para os botões
    // Registra o callback para o BUTTON_A_GPIO e habilita IRQ para os demais botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_GPIO, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    gpio_set_irq_enabled(BUTTON_B_GPIO, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BUTTON_JOY_GPIO, GPIO_IRQ_EDGE_FALL, true);

    char received_char = '\0';

    while (true) {
        // Processa a entrada de caracteres via Serial Monitor
        if (stdio_usb_connected()) {
            int ch = getchar_timeout_us(0);
            if (ch != PICO_ERROR_TIMEOUT) {
                received_char = (char) ch;
                printf("Caractere recebido: %c\n", received_char);

                // Exibe o caractere no display
                char message[2] = {received_char, '\0'};
                display_message(&ssd, message);

                // Se o caractere for um número, acende os LEDs correspondentes na matriz WS2812
                if (received_char >= '0' && received_char <= '9') {
                    int number = received_char - '0';
                    display_number_on_leds(number, pio_global, sm_global, led_buffer);
                }
            }
        }
        sleep_ms(10);
    }
}
