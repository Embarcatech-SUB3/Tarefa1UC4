#include <stdio.h>
#include <stdlib.h> 
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LEDR 13
#define LEDB 12
#define LEDG 11
#define BUZZER 21

uint colunas[4] = {4, 3, 2, 28};
uint linhas[4] = {8, 7, 6, 5};

char MAPA_TECLAS[16] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};

uint _colunas[4];
uint _linhas[4];
char _valores_matriz[16];
uint mascara_todas_colunas = 0x0;
uint mascara_coluna[4];

void imprimir_binario(int numero) {
    for (int i = 31; i >= 0; i--) {
        (numero & (1 << i)) ? printf("1") : printf("0");
    }
    printf("\n");
}

void inicializar_teclado(uint colunas[4], uint linhas[4], char valores_matriz[16]) {
    for (int i = 0; i < 16; i++) {
        _valores_matriz[i] = valores_matriz[i];
    }

    for (int i = 0; i < 4; i++) {
        _colunas[i] = colunas[i];
        _linhas[i] = linhas[i];

        gpio_init(_colunas[i]);
        gpio_init(_linhas[i]);

        gpio_set_dir(_colunas[i], GPIO_IN);
        gpio_set_dir(_linhas[i], GPIO_OUT);

        gpio_put(_linhas[i], 1);

        mascara_todas_colunas += (1 << _colunas[i]);
        mascara_coluna[i] = 1 << _colunas[i];
    }
}

char obter_tecla() {
    int linha;
    uint32_t colunas_ativas;

    colunas_ativas = gpio_get_all() & mascara_todas_colunas;
    imprimir_binario(colunas_ativas);

    if (colunas_ativas == 0x0) {
        return 0;
    }

    for (int j = 0; j < 4; j++) {
        gpio_put(_linhas[j], 0);
    }

    for (linha = 0; linha < 4; linha++) {
        gpio_put(_linhas[linha], 1);
        busy_wait_us(10000);
        colunas_ativas = gpio_get_all() & mascara_todas_colunas;
        gpio_put(_linhas[linha], 0);
        if (colunas_ativas != 0x0) {
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        gpio_put(_linhas[i], 1);
    }

    for (int i = 0; i < 4; i++) {
        if (colunas_ativas == mascara_coluna[i]) {
            return (char)_valores_matriz[linha * 4 + i];
        }
    }
    return 0;
}

void emitir_som(uint pin, uint freq, uint duracao_ms) {
    uint periodo_us = 1000000 / freq; 
    uint duracao_total = duracao_ms * 1000; 
    uint tempo_passado = 0;

    while (tempo_passado < duracao_total) {
        gpio_put(pin, 1);
        busy_wait_us(periodo_us / 2);
        gpio_put(pin, 0);
        busy_wait_us(periodo_us / 2);
        tempo_passado += periodo_us;
    }
}

int main() {
    stdio_init_all();
    inicializar_teclado(colunas, linhas, MAPA_TECLAS);
    char tecla;

    gpio_init(LEDR);
    gpio_set_dir(LEDR, GPIO_OUT);
    gpio_init(LEDB);
    gpio_set_dir(LEDB, GPIO_OUT);
    gpio_init(LEDG);
    gpio_set_dir(LEDG, GPIO_OUT);
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);


    while (true) {
        tecla = obter_tecla();
        printf("\nTecla pressionada: %c\n", tecla);

        if (tecla == 'A') {
            gpio_put(LEDR, true);
            printf("LED vermelho ligado\n");
        } else {
            gpio_put(LEDR, false);
        }

        if (tecla == 'B') {
            gpio_put(LEDB, true);
            printf("LED azul ligado\n");
        } else {
            gpio_put(LEDB, false);
        }

        if (tecla == 'C') {
            gpio_put(LEDG, true);
            printf("LED verde ligado\n");
        } else {
            gpio_put(LEDG, false);
        }

        if (tecla == '#') {
            emitir_som(BUZZER, 500, 200); 
        } else {
            gpio_put(BUZZER, 0);
        }

        if (tecla == 'D') {
            gpio_put(LEDR, true);
            gpio_put(LEDB, true);
            gpio_put(LEDG, true);
        } else {
            gpio_put(LEDR, false);
            gpio_put(LEDB, false);
            gpio_put(LEDG, false);
        }

        if (tecla == '*') {
            printf("\nSessão terminada!!!\n");
            exit(0);
        }
        sleep_ms(2000);
    }
}