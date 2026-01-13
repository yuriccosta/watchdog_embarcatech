#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h" 

// Mapeamento de Hardware (BitDogLab) 
#define BUTTON_A_PIN 5  // Liga/Desliga
#define BUTTON_B_PIN 6  // Muda Velocidade / Simula Falha

#define LED_G_PIN 11   
#define LED_B_PIN 12    
#define LED_R_PIN 13   

//  Código de Erro
#define ERRO_FALHA_CRITICA 0xDEAD 


volatile bool system_on = false;
volatile int speed_level = 1; 


void set_led_color(bool r, bool g, bool b) {
    gpio_put(LED_R_PIN, r);
    gpio_put(LED_G_PIN, g);
    gpio_put(LED_B_PIN, b);
}

// Callback dos Botões (Interrupção) 
void button_callback(uint gpio, uint32_t events) {
    static uint32_t last_press_time = 0;
    uint32_t now = time_us_32();

    // Debounce de 200ms para evitar leituras falsas
    if ((now - last_press_time) > 200000) {
        
        if (gpio == BUTTON_A_PIN) {
            system_on = !system_on;
        } 
        else if (gpio == BUTTON_B_PIN && system_on) {
            speed_level++;
            if (speed_level > 3) speed_level = 1;
        }
        
        last_press_time = now;
    }
}

int main() {
    stdio_init_all(); 

    gpio_init(LED_G_PIN); gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN); gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_init(LED_R_PIN); gpio_set_dir(LED_R_PIN, GPIO_OUT);

    gpio_init(BUTTON_A_PIN); gpio_set_dir(BUTTON_A_PIN, GPIO_IN); gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN); gpio_set_dir(BUTTON_B_PIN, GPIO_IN); gpio_pull_up(BUTTON_B_PIN);


    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    set_led_color(1, 1, 1); sleep_ms(1000); 
    set_led_color(0, 0, 0);

    // Verifica se o sistema acabou de voltar de um travamento
    if (watchdog_caused_reboot()) {
        printf("\nRECUPERADO DE TRAVAMENTO!\n");
        
        // Lê o registrador Scratch 4
        uint32_t motivo = watchdog_hw->scratch[4];
        
        if (motivo == ERRO_FALHA_CRITICA) {
            printf("--> Causa identificada: Travamento forcado no Botao B (Erro 0xDEAD)\n");
        } else {
            printf("--> Causa desconhecida (Watchdog padrao).\n");
        }

        // Pisca Vermelho 3x para alertar visualmente o erro
        for(int i=0; i<3; i++) {
            set_led_color(1, 0, 0); sleep_ms(200); 
            set_led_color(0, 0, 0); sleep_ms(200);
        }
        
        // Limpa o registrador para o próximo teste
        watchdog_hw->scratch[4] = 0; 
        
    } else {
        printf("\nInicializacao normal (Power on). Tudo pronto.\n");
    }

    // Ativa Watchdog (Timeout de 4 segundos)
    watchdog_enable(4000, 1);


    while (true) {
        // Alimenta o Watchdog para evitar reset durante operação normal
        watchdog_update();

        // Controle dos LEDs (Estados do Sistema)
        if (!system_on) {
            set_led_color(0, 0, 0);
        } else {
            switch (speed_level) {
                case 1: set_led_color(0, 1, 0); break; // Verde
                case 2: set_led_color(0, 0, 1); break; // Azul
                case 3: set_led_color(1, 0, 0); break; // Vermelho
            }
        }

        // SIMULAÇÃO DE FALHA
        // Se segurar o Botão B, força o travamento
        if (!gpio_get(BUTTON_B_PIN) && system_on) {
            sleep_ms(1000); // Diferencia clique curto de longo
            
            if (!gpio_get(BUTTON_B_PIN)) {
                printf("\nSIMULANDO TRAVAMENTO CRITICO...\n");
                set_led_color(1, 1, 1); // Fica branco indicando "congelado"
                
                // Grava o código de erro no registrador que sobrevive ao reset
                watchdog_hw->scratch[4] = ERRO_FALHA_CRITICA;
                
                // Entra em Loop Infinito sem alimentar o Watchdog
                while(true) {
                    // O processador fica preso aqui até estourar os 4 segundos
                }
            }
        }

        sleep_ms(50);
    }
}