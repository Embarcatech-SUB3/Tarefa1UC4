#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"


#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define BUZZER_PIN_A 21
#define BUZZER_PIN_B 10
#define BUZZER_FREQUENCY_A 10000 // mhz
#define BUZZER_FREQUENCY_B 100 // mhz


// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin,int buzzeer_frequency) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (buzzeer_frequency * 4096)); // Divisor de clock
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



void menu(){
    char escolha = 'n';
    printf("\n");
    printf("Deseja fazer o que?\n");
    printf("[1] [2] [3] [4] [5] [6] [7]\n");
    scanf("%c",&escolha);
    if (escolha == '1')
    {
        printf("Led vermelho ligado");
        gpio_put(LED_GREEN_PIN, false);
        gpio_put(LED_BLUE_PIN, false);
        gpio_put(LED_RED_PIN, true);
    }else if(escolha == '2'){
        printf("Led verde ligado");
        gpio_put(LED_BLUE_PIN, false);
        gpio_put(LED_RED_PIN, false);
        gpio_put(LED_GREEN_PIN, true);
    }
    else if(escolha == '3'){
        printf("Led azul ligado");
        gpio_put(LED_GREEN_PIN, false);
        gpio_put(LED_RED_PIN, false);
        gpio_put(LED_BLUE_PIN, true);
    }else if (escolha == '4'){
        printf("Os 3 leds ligado formando a cor branca");
        gpio_put(LED_BLUE_PIN, true);
        gpio_put(LED_GREEN_PIN, true);
        gpio_put(LED_RED_PIN, true);
    }else if(escolha == '5'){
        printf("Buzzer A emitindo sinal");
        beep(BUZZER_PIN_A, 2000);
    }else if(escolha == '6'){
        printf("Buzzer B emitindo sinal");
        beep(BUZZER_PIN_B,2000);
    }
    else if(escolha == '7'){
        printf("Todos os leds desligados");
        gpio_put(LED_BLUE_PIN, false);
        gpio_put(LED_GREEN_PIN, false);
        gpio_put(LED_RED_PIN, false);
    }
    else{
        printf("Por favor escolha uma das opções acima!!!");
        sleep_ms(2000);
    }
    printf("\n");
    sleep_ms(200);// debouncing
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
    gpio_init(BUZZER_PIN_A);
    gpio_init(BUZZER_PIN_B);
    gpio_set_dir(BUZZER_PIN_A, GPIO_OUT);
    gpio_set_dir(BUZZER_PIN_B, GPIO_OUT);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN_A,BUZZER_FREQUENCY_A);
    pwm_init_buzzer(BUZZER_PIN_B,BUZZER_FREQUENCY_B);

    while (true)
    {
        menu();
    }
    
}
