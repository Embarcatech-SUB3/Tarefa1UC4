#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definição dos pinos do teclado matricial
const uint8_t colunas[4] = {5, 4, 3, 2}; // C1 - GP5; C2 - GP4; C3 - GP3; C4 - GP2
const uint8_t linhas[4] = {9, 8, 7, 6}; // R1 - GP9; R2 - GP8; R3 - GP7; R4 - GP6

// Mapeamento do teclado
const char teclado[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Definição dos pinos do LED RGB
const uint8_t led_red = 13;   // GP13
const uint8_t led_green = 12; // GP12
const uint8_t led_blue = 11;  // GP11

// Pino do buzzer
const uint8_t buzzer = 27;

// Definições para as cores do LED
#define LED_RED 'R'
#define LED_GREEN 'G'
#define LED_BLUE 'B'

// Frequência do clock do sistema em Hz
#define SYS_CLOCK_HZ 125000000

// Declaração das funções
char leitura_teclado();
void configurar_pinos();
void controlar_led(char cor);
void desligar_led();
void buzzer_tone_pwm(uint frequency, uint duration_ms);

int main() {
    stdio_init_all();
    configurar_pinos();

    while (true) {
        char tecla = leitura_teclado();

        if (tecla != 'n') { // Verifica se uma tecla foi pressionada
            printf("Tecla pressionada: %c\n", tecla);

            switch (tecla) {
                case 'A':
                    controlar_led(LED_RED); // Liga o LED vermelho
                    sleep_ms(1000);
                    desligar_led();
                    break;
                case 'B':
                    controlar_led(LED_BLUE); // Liga o LED azul
                    sleep_ms(1000);
                    desligar_led();
                    break;
                case 'C':
                    controlar_led(LED_GREEN); // Liga o LED verde
                    sleep_ms(1000);
                    desligar_led();
                    break;
                case 'D':
                    buzzer_tone_pwm(1000, 500); // Toca um som de 1 kHz por 500 ms
                    break;
            }
        }

        sleep_ms(200); // Intervalo entre leituras
    }

    return 0;
}

// Configuração inicial dos pinos
void configurar_pinos() {
    for (int i = 0; i < 4; i++) {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_OUT);
        gpio_put(colunas[i], 1);

        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_IN);
        gpio_pull_up(linhas[i]);
    }

    gpio_init(led_red);
    gpio_set_dir(led_red, GPIO_OUT);
    gpio_put(led_red, 0);

    gpio_init(led_green);
    gpio_set_dir(led_green, GPIO_OUT);
    gpio_put(led_green, 0);

    gpio_init(led_blue);
    gpio_set_dir(led_blue, GPIO_OUT);
    gpio_put(led_blue, 0);

    gpio_set_function(buzzer, GPIO_FUNC_PWM); // Configura o pino do buzzer para PWM
    uint slice_num = pwm_gpio_to_slice_num(buzzer); // Obtém o slice do PWM para o buzzer
    pwm_set_enabled(slice_num, false); // Inicialmente desabilitado
}

// Função para desligar todos os LEDs
void desligar_led() {
    gpio_put(led_red, 0);
    gpio_put(led_green, 0);
    gpio_put(led_blue, 0);
}

// Função para controlar o LED RGB
void controlar_led(char cor) {
    desligar_led(); // Garante que apenas uma cor ficará ativa

    switch (cor) {
        case LED_RED:
            gpio_put(led_red, 1); // Liga o LED vermelho
            break;
        case LED_GREEN:
            gpio_put(led_green, 1); // Liga o LED verde
            break;
        case LED_BLUE:
            gpio_put(led_blue, 1); // Liga o LED azul
            break;
    }
}

// Função para emitir som no buzzer usando PWM
void buzzer_tone_pwm(uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(buzzer); // Obtém o slice do PWM para o pino do buzzer
    uint clock_divider = 4; // Divisor do clock (ajusta precisão)
    uint32_t top = (SYS_CLOCK_HZ / (clock_divider * frequency)) - 1; // Calcula o TOP do PWM

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_divider);
    pwm_config_set_wrap(&config, top);

    pwm_init(slice_num, &config, true); // Inicializa o PWM
    pwm_set_gpio_level(buzzer, top / 2); // Define o duty cycle para 50% (som contínuo)

    sleep_ms(duration_ms); // Duração do som

    pwm_set_gpio_level(buzzer, 0); // Desliga o som
    pwm_set_enabled(slice_num, false); // Desativa o PWM
}

// Função para ler o teclado matricial
char leitura_teclado() {
    char tecla = 'n'; // Valor padrão indicando nenhuma tecla pressionada

    for (int i = 0; i < 4; i++) {
        gpio_put(colunas[i], 0);

        for (int j = 0; j < 4; j++) {
            if (gpio_get(linhas[j]) == 0) {
                tecla = teclado[j][i];
                while (gpio_get(linhas[j]) == 0) {
                    sleep_ms(10); // Debouncing
                }
                break;
            }
        }

        gpio_put(colunas[i], 1);

        if (tecla != 'n') {
            break;
        }
    }

    return tecla;
}
