#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"


#define LED_GREEN_PIN 11 // Pino para led Verde
#define LED_BLUE_PIN 12 // Pino para led azul
#define LED_RED_PIN 13 // Pino para led Vermelho

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 4000

const uint8_t colunas[4] = {1, 2, 3, 4}; // Pinos das colunas
const uint8_t linhas[4] = {5, 6, 7, 8};  // Pinos das linhas

const char teclado[4][4] = 
{
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}

// Função para ler o teclado matricial
char leitura_teclado()
{
    char numero = 'n'; // Valor padrão para quando nenhuma tecla for pressionada

    // Desliga todos os pinos das colunas
    for (int i = 0; i < 4; i++)
    {
        gpio_put(colunas[i], 1);
    }

    for (int coluna = 0; coluna < 4; coluna++)
    {
        // Ativa a coluna atual (coloca o pino da coluna como 1)
        gpio_put(colunas[coluna], 0);

        for (int linha = 0; linha < 4; linha++)
        {
            // Verifica o estado da linha. Se estiver em 0, a tecla foi pressionada
            if (gpio_get(linhas[linha]) == 0)
            {
                numero = teclado[linha][coluna]; // Mapeia a tecla pressionada
                // Aguarda a tecla ser liberada (debounce)
                while (gpio_get(linhas[linha]) == 0)
                {
                    sleep_ms(10); // Aguarda a tecla ser liberada
                }
                break; // Sai do laço após detectar a tecla
            }
        }

        // Desativa a coluna atual (coloca o pino da coluna como 0)
        gpio_put(colunas[coluna], 1);

        if (numero != 'n') // Se uma tecla foi pressionada, sai do laço de colunas
        {
            break;
        }
    }

    return numero; // Retorna a tecla pressionada
}
int main() 
{
    // Inicializa a UART (Serial)
    stdio_init_all();

    gpio_init(LED_RED_PIN); //inicializa o pino do LED_RED_PIN
    gpio_set_dir(LED_RED_PIN, GPIO_OUT); //define o pino como saída
    gpio_put(LED_RED_PIN, false);

    gpio_init(LED_GREEN_PIN); //inicializa o pino do LED_GREEN_PIN
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT); //define o pino como saída
    gpio_put(LED_GREEN_PIN, false);

    gpio_init(LED_BLUE_PIN); //inicializa o pino do LED_BLUE_PIN
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT); //define o pino como saída
    gpio_put(LED_BLUE_PIN, false);

    // Configuração do GPIO para o buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // Configuração dos pinos das colunas como saídas digitais
    for (int i = 0; i < 4; i++)
    {
        gpio_init(colunas[i]);
        gpio_set_dir(colunas[i], GPIO_OUT);
        gpio_put(colunas[i], 1); // Inicializa todas as colunas como baixo
    }

    // Configuração dos pinos das linhas como entradas digitais
    for (int i = 0; i < 4; i++)
    {
        gpio_init(linhas[i]);
        gpio_set_dir(linhas[i], GPIO_IN);
        gpio_pull_up(linhas[i]); // Habilita pull-up para as linhas
    }

    while (true) 
    {
        char tecla = leitura_teclado();
        
        if (tecla != 'n') // Só exibe se uma tecla foi pressionada
        {
            printf("Tecla pressionada: %c\n", tecla);
            if(tecla == 'A')
            {
              gpio_put(LED_RED_PIN, true); // Liga o led vermelho por 1s
              sleep_ms(1000);
              gpio_put(LED_RED_PIN, false);
            }
            else if(tecla == 'B'){ // Liga o led verde por 1s
              gpio_put(LED_GREEN_PIN, true);
              sleep_ms(1000);
              gpio_put(LED_GREEN_PIN, false);
            }
            else if (tecla == 'C'){ // Liga o led azul por 1s
              gpio_put(LED_BLUE_PIN, true);
              sleep_ms(1000);
              gpio_put(LED_BLUE_PIN, false);
            }
            else if (tecla == 'D'){ // Liga todos os 3 leds rgb por 3s
              gpio_put(LED_BLUE_PIN, true);
              gpio_put(LED_GREEN_PIN, true);
              gpio_put(LED_RED_PIN, true);
              sleep_ms(3000);
              gpio_put(LED_BLUE_PIN, false);
              gpio_put(LED_GREEN_PIN, false);
              gpio_put(LED_RED_PIN, false);
            }
            else if(tecla == '#'){ // Emite sinal sonoro pelo buzzer
              beep(BUZZER_PIN, 1000);
            }
            else if (tecla == '*'){ // Limpa terminal
                printf("\ec\e[3J");
            }
        }

        sleep_ms(100); // Intervalo de tempo menor para uma leitura mais rápida
    }

    return 0;
}

