#include "user_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <xc.h>
#include <stdio.h>

// Definição do pipe (queue)
QueueHandle_t xQueuePipe;

// Configuração dos LEDs de teste
void led_init() {
    TRISBbits.TRISB1 = 0; // LED Severidade - Saída
    TRISBbits.TRISB2 = 0; // LED Moderado - Saída
    TRISBbits.TRISB3 = 0; // LED Normal - Saída

    LATBbits.LATB1 = 0; // Apaga LED Severidade
    LATBbits.LATB2 = 0; // Apaga LED Moderado
    LATBbits.LATB3 = 0; // Apaga LED Normal
}

// Configuração do ADC
void adc_init() {
    AD1CON1 = 0x00E0; // Modo automático, ligado
    AD1CON2 = 0;      // Tensão de referência interna
    AD1CON3 = 0x1F02; // Amostragem manual, clock de conversão
    AD1CHS = 0;       // Canal 0 (AN0)
    AD1PCFGbits.PCFG0 = 0; // AN0 como entrada analógica
    AD1CSSL = 0;      // Sem scan
    AD1CON1bits.ADON = 1; // Liga o ADC
}

// Função para leitura do ADC
uint16_t adc_read(){
    AD1CON1bits.SAMP = 1;       // Inicia a amostragem
    vTaskDelay(pdMS_TO_TICKS(1)); // Aguarda tempo de amostragem
    AD1CON1bits.SAMP = 0;       // Finaliza a amostragem e inicia a conversão
    while (!AD1CON1bits.DONE);  // Aguarda a conversão terminar
    return ADC1BUF0;            // Retorna o valor convertido
}

// Task de leitura do sensor'
void vTaskReadSensor(void *pvParameters) {
    uint16_t adcValue;
    char level;
    while (1) {
        // Lê o valor do ADC
        adcValue = adc_read();
        float temperature = (adcValue * 3.3 / 1023.0) * 100.0;

        // Determina o nível de criticidade
        if (temperature > 70.0) {
            level = 'S'; // Severidade alta
        } else if (temperature > 40.0) {
            level = 'M'; // Moderada
        } else {
            level = 'N'; // Normal
        }

        // Envia o nível para o pipe
        xQueueSend(xQueuePipe, &level, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Executa a cada 1 segundo
    }
}

// Task de controle geral
void vTaskControl(void *pvParameters) {
    char receivedLevel;
    while (1) {
        // Recebe o nível de criticidade do pipe
        if (xQueueReceive(xQueuePipe, &receivedLevel, portMAX_DELAY) == pdPASS) {
            // Define o estado dos LEDs
            switch (receivedLevel) {
                case 'S':
                    LATBbits.LATB0 = 1; // Liga LED Severidade
                    LATBbits.LATB1 = 0; // Desliga LED Moderado
                    LATBbits.LATB2 = 0; // Desliga LED Normal
                    break;
                case 'M':
                    LATBbits.LATB0 = 0; // Desliga LED Severidade
                    LATBbits.LATB1 = 1; // Liga LED Moderado
                    LATBbits.LATB2 = 0; // Desliga LED Normal
                    break;
                case 'N':
                    LATBbits.LATB0 = 0; // Desliga LED Severidade
                    LATBbits.LATB1 = 0; // Desliga LED Moderado
                    LATBbits.LATB2 = 1; // Liga LED Normal
                    break;
                default:
                    LATBbits.LATB0 = 0; // Desliga todos em caso de erro
                    LATBbits.LATB1 = 0;
                    LATBbits.LATB2 = 0;
                    break;
            }
        }
    }
}

//SemaphoreHandle_t s_contador;
//
//void config_user_app()
//{
//    TRISDbits.TRISD0 = 0;
//    
//    s_contador = xSemaphoreCreateCounting(5, 5);
//
//}
//
//void teste_sem_contador()
//{
//    while (1) {
//        xSemaphoreTake(s_contador, portMAX_DELAY);
//        PORTDbits.RD0 = 1;
//        vTaskDelay(20);        
//        PORTDbits.RD0 = 0;
//        vTaskDelay(20);        
//    }
//}

/*
 * Problema dos canibais

#define MAX_PORCOES 10
#define DELAY       20

// Variável compartilhada
unsigned int panela = MAX_PORCOES;

// Semáforos de controle
SemaphoreHandle_t s_cozinheiro, s_panela;


void config_user_app()
{   
    s_cozinheiro = xSemaphoreCreateBinary();
    s_panela = xSemaphoreCreateBinary();

    xSemaphoreGive(s_panela);
    
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
}

void cozinheiro()
{
    while (1) {
        // Cozinheiro acorda ou dorme
        xSemaphoreTake(s_cozinheiro, portMAX_DELAY);
        PORTDbits.RD1 = 1;
        // Cozinhar
        xSemaphoreTake(s_panela, portMAX_DELAY);
        panela = MAX_PORCOES;
        vTaskDelay(DELAY);
        PORTDbits.RD0 = 0;
        xSemaphoreGive(s_panela);        
        PORTDbits.RD1 = 0;
    }
}

void canibal_1()
{
    while (1) {
        xSemaphoreTake(s_panela, portMAX_DELAY);
        PORTDbits.RD2 = 1;

        panela--;
        if (panela == 0) {
            PORTDbits.RD0 = 1;
            xSemaphoreGive(s_cozinheiro);
        } 
        xSemaphoreGive(s_panela);

        vTaskDelay(DELAY);
        PORTDbits.RD2 = 0;
        vTaskDelay(DELAY);
    }
}

void canibal_2()
{
    while (1) {
        xSemaphoreTake(s_panela, portMAX_DELAY);
        PORTDbits.RD3 = 1;

        panela--;
        if (panela == 0) {
            PORTDbits.RD0 = 1;
            xSemaphoreGive(s_cozinheiro);
        }
        xSemaphoreGive(s_panela);

        vTaskDelay(DELAY);
        PORTDbits.RD3 = 0;
        vTaskDelay(DELAY);
    }        
}

void canibal_3()
{
    while (1) {
        xSemaphoreTake(s_panela, portMAX_DELAY);
        PORTDbits.RD4 = 1;

        panela--;
        if (panela == 0) {
            PORTDbits.RD0 = 1;
            xSemaphoreGive(s_cozinheiro);
        }
        xSemaphoreGive(s_panela);

        vTaskDelay(DELAY);
        PORTDbits.RD4 = 0;
        vTaskDelay(DELAY);        
    }        
}

 */

/*

QueueHandle_t fila;

void config_user_app()
{
    fila = xQueueCreate(3, sizeof(int));
    
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    
    TRISCbits.TRISC1 = 1;
    TRISCbits.TRISC2 = 1;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
}

void task_1()
{
    char msg[] = {'B', 'G', 'R', 'Y', 'A'};
    
    while (1) {
        if (PORTCbits.RC1 == 1) {
            xQueueSend(fila, &msg[0], portMAX_DELAY);
        }
        else if (PORTCbits.RC2 == 1) {
            xQueueSend(fila, &msg[1], portMAX_DELAY);
        }
        else if (PORTCbits.RC3 == 1) {
            xQueueSend(fila, &msg[2], portMAX_DELAY);
        }
        else if (PORTCbits.RC4 == 1) {
            xQueueSend(fila, &msg[3], portMAX_DELAY);
        }
        else {
            xQueueSend(fila, msg[4], portMAX_DELAY);
        }
       
        vTaskDelay(10);
        
        
    }
}

void task_2()
{
    char msg;
    
    while (1) {
        xQueueReceive(fila, (char*)&msg, portMAX_DELAY);
        
        if (msg == 'B') {
            LATDbits.LATD0 = 1;
        }
        else if (msg == 'G') {
            LATDbits.LATD1 = 1;
        }
        else if (msg == 'R') {
            LATDbits.LATD2 = 1;
        }
        else if (msg == 'Y') {
            LATDbits.LATD3 = 1;
        }
        else {
            LATDbits.LATD0 = 0;
            LATDbits.LATD1 = 0;
            LATDbits.LATD2 = 0;
            LATDbits.LATD3 = 0;
        }
        
        vTaskDelay(10);
    }
}

/*

/*

SemaphoreHandle_t sem_1, sem_2, sem_3;


void config_user_app()
{
    sem_1 = xSemaphoreCreateBinary();
    sem_2 = xSemaphoreCreateBinary();
    sem_3 = xSemaphoreCreateBinary();
    
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
}

void task_1()
{
    
    while (1) {
        xSemaphoreGive(sem_2);        
        LATDbits.LATD0 = ~PORTDbits.RD0;
        xSemaphoreTake(sem_1, portMAX_DELAY);
        
    }
}

void task_2()
{
    while (1) {
        xSemaphoreGive(sem_3);        
        LATDbits.LATD1 = ~PORTDbits.RD1;
        xSemaphoreTake(sem_2, portMAX_DELAY);
    }
}

void task_3()
{
    while (1) {
        //xSemaphoreGive(sem_1);        
        LATDbits.LATD2 = ~PORTDbits.RD2;
        xSemaphoreTake(sem_3, portMAX_DELAY);
    }
}

*/