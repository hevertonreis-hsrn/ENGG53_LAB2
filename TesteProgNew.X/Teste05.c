#include <xc.h>
#include <stdio.h>
#include <sys/attribs.h>
#include "delay.h"
#include "ILI9341.h"
#include "SPI2.h"

// Configurações do clock principal para 80 MHz (8 MHz FRC com PLL)
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF
 
#define LED1_PIN LATAbits.LATA0       // Define o LED1 no pino RA0
#define LED2_PIN LATAbits.LATA1       // Define o LED1 no pino RA1
#define LED3_PIN LATAbits.LATA2       // Define o LED1 no pino RA2

#define BUZZER_PIN LATAbits.LATA3     // Define o buzzer no pino RA3

#define PWR_LED LATAbits.LATA7        // Define o LED que indica o funcionamento no pino RA7

#define BUTTON1_PIN PORTFbits.RF4     // Define o botão 1 no pino RF4
#define BUTTON2_PIN PORTFbits.RF5     // Define o botão 2 no pino RF5
#define BUTTON3_PIN PORTDbits.RD13     // Define o botão 2 no pino RF5

#define ADC_PIN 0                     // Define o pino ADC no pino AN0 - RB0
#define VOLTAGE_THRESHOLD 512         // Limite de tensão para acionar o LED2 e o buzzer

#define SYS_FREQ 80000000L

// Variáveis observadas
volatile int alarm1_on = 0;
volatile int alarm2_on = 0;
volatile int alarm3_on = 0;
volatile int buzzer_on = 0;
volatile unsigned int adc_value = 0;

void setup() {
    DDPCONbits.JTAGEN = 0;

    TRISAbits.TRISA0 = 0; // LED1 como saída
    TRISAbits.TRISA1 = 0; // LED2 como saída
    TRISAbits.TRISA2 = 0; // LED3 como saída
    TRISAbits.TRISA3 = 0; // BUZZER como saída
    TRISAbits.TRISA7 = 0; // PWR_LED como saída
    TRISFbits.TRISF4 = 1; // BUTTON1 como entrada
    TRISFbits.TRISF5 = 1; // BUTTON2 como entrada

    LED1_PIN = 0;
    LED2_PIN = 0;
    LED3_PIN = 0;
    PWR_LED = 0;
    BUZZER_PIN = 0;
    
    // Configuração do ADC
    AD1CON1 = 0x00E0;       // Auto-convert mode
    AD1CHS = ADC_PIN;       // Selecionar o pino ADC
    AD1CSSL = 0;
    AD1CON3 = 0x1F02;       // Tempo de amostragem manual
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;   // Ligar o ADC

    // Habilite o pull-up interno para os botões
    CNPUEbits.CNPUE17 = 1; // Habilita pull-up para RF4
    CNPUEbits.CNPUE18 = 1; // Habilita pull-up para RF5
    CNPUEbits.CNPUE19 = 1; // Habilita pull-up para RD13
    
    // Configurar os pinos do SPI2
    TRISGbits.TRISG6 = 0; // SCK como saida - FIXO
    TRISGbits.TRISG7 = 0; // SDI do LCD/SDO do PIC como saida - FIXO
    TRISGbits.TRISG8 = 0; // RST como saida (configurar conforme necessidade)
    TRISGbits.TRISG9 = 0; // DC como saida
    
    // Inicialização do LCD
    SPI2_Initialize();
    ILI9341_Init();
    setOrientation(LANDSCAPE);
    bg(WHITE);

    // Habilite as interrupções globais
    __builtin_enable_interrupts();
}

void setupDisplay(){
    // Definido cor de fundo
    bg(CUSTOM_DARK_GRAY);
    
    // Cabecalho
    fillRectangle(0, 0, 320, 30, CUSTOM_BLUE);
    putString("PIC CLP - Tela de Status", 15, 7, 2, WHITE);
    fillRectangle(0, 30, 320, 5, BLACK);
    
    // Moldura dos Cards 
    fillRoundRect(5, 42, 308, 192, 6, CUSTOM_GRAY);
    
    // Card Principal
    fillRoundRect(10, 52, 140, 175, 6, WHITE);
    fillRoundRect(10, 48, 140, 28, 6, CUSTOM_DARK_BLUE);
    drawRoundRect(10, 48, 140, 28, 6, BLACK);
    putString("Equipe 02", 23, 56, 2, WHITE);
    
    // Card CLP Externo 01
    fillRoundRect(156, 52, 150, 82, 6, WHITE);
    fillRoundRect(156, 48, 150, 28, 6, CUSTOM_DARK_BLUE);
    drawRoundRect(156, 48, 150, 28, 6, BLACK);
    putString("Equipe 01", 180, 56, 2, WHITE);
    
    // Card CLP Externo 02
    fillRoundRect(156, 144, 150, 82, 6, WHITE);
    fillRoundRect(156, 140, 150, 28, 6, CUSTOM_DARK_BLUE);
    drawRoundRect(156, 140, 150, 28, 6, BLACK);
    putString("Equipe 03", 180, 148, 2, WHITE);

    // Titulos dos LEDs na tela
    putString("SENSOR1", 14, 90, 2, BLACK);
    putString("SENSOR2", 14, 118, 2, BLACK);
    putString("SENSOR3", 14, 145, 2, BLACK);
    
    // Titulos dos LEDs na tela - CLP Externo 01
    putString("SENSOR1", 162, 85, 1, BLACK);
    
    fillRectangle(208, 83, 21, 11, GREEN);
    putString("OK", 212, 85, 1, WHITE);
    
    putString("SENSOR2", 162, 101, 1, BLACK);
    
    fillRectangle(208, 99, 21, 11, GREEN);
    putString("OK", 212, 101, 1, WHITE);
    
    putString("SENSOR3", 162, 117, 1, BLACK);
    
    fillRectangle(208, 115, 21, 11, RED);
    putString("OUT", 209, 117, 1, WHITE);
    
    // Titulos dos LEDs na tela - CLP Externo 02
    putString("SENSOR1", 162, 176, 1, BLACK);
    
    fillRectangle(208, 174, 21, 11, RED);
    putString("OUT", 209, 176, 1, WHITE);
    
    putString("SENSOR2", 162, 192, 1, BLACK);
    
    fillRectangle(208, 190, 21, 11, RED);
    putString("OUT", 209, 192, 1, WHITE);
    
    putString("SENSOR3", 162, 208, 1, BLACK);
    
    fillRectangle(208, 206, 21, 11, GREEN);
    putString("OK", 212, 208, 1, WHITE);
}

void updateDisplay() {
    char buffer[16];

    // Atualizar SENSOR1
    if (alarm1_on) {
        fillRectangle(103, 87, 41, 19, RED);
        putString("OUT", 105, 89, 2, WHITE);
    } else {
        fillRectangle(103, 87, 41, 19, GREEN);
        putString("OK", 110, 90, 2, WHITE);
    }

    // Atualizar SENSOR2
    if (alarm2_on) {
        fillRectangle(103, 116, 41, 19, RED);
        putString("OUT", 105, 119, 2, WHITE);
    } else {
        fillRectangle(103, 116, 41, 19, GREEN);
        putString("OK", 110, 118, 2, WHITE);
    }

    // Atualizar SENSOR3
    if (alarm3_on) {
        fillRectangle(103, 143, 41, 19, RED);
        putString("OUT", 105, 145, 2, WHITE);
    } else {
        fillRectangle(103, 143, 41, 19, GREEN);
        putString("OK", 110, 145, 2, WHITE);
    }

    // Atualizar valor ADC
    sprintf(buffer, "ADC: %u", adc_value);
    fillRectangle(20, 178, 120, 40, BLACK);
    putString(buffer, 30, 190, 2, YELLOW);
}

int main() {
    
    setup();
    
    setupDisplay();

    int lastButton1State = 1; // Estado anterior do botão 1
    int lastButton2State = 1; // Estado anterior do botão 2
    int lastButton3State = 1; // Estado anterior do botão 3

    while (1) {
        PWR_LED = 1;
        delayMs(500);

        // Verifique o estado do botão 1
        if (BUTTON1_PIN == 0 && lastButton1State == 1) { // Botão 1 pressionado
            lastButton1State = 0; // Atualize o estado do botão 1

            if (alarm1_on && buzzer_on) {
                LED1_PIN = 0; // Desliga o LED1
                BUZZER_PIN = 0; // Desliga o buzzer
                alarm1_on = 0;
                buzzer_on = 0;
            } else if (alarm1_on) {
                LED1_PIN = 0; // Desliga o LED1
                BUZZER_PIN = 0; // Desliga o buzzer
                alarm1_on = 0;
                buzzer_on = 0;
            } else {
                LED1_PIN = 1; // Liga o LED1
                BUZZER_PIN = 1; // Liga o buzzer
                alarm1_on = 1;
                buzzer_on = 1;
            }

            updateDisplay();  // Atualizar a tela LCD
            delayMs(200); // Debounce do botão
        } else if (BUTTON1_PIN == 1) {
            lastButton1State = 1; // Atualize o estado do botão 1
        }
        
        // Verifique o estado do botão 
        if (BUTTON2_PIN == 0 && lastButton2State == 1) { // Botão 2 pressionado
            lastButton2State = 0; // Atualize o estado do botão 2

            if (alarm2_on && buzzer_on) {
                LED2_PIN = 0; // Desliga o LED1
                BUZZER_PIN = 0; // Desliga o buzzer
                alarm2_on = 0;
                buzzer_on = 0;
            } else if (alarm2_on) {
                LED2_PIN = 0; // Desliga o LED1
                BUZZER_PIN = 0; // Desliga o buzzer
                alarm2_on = 0;
                buzzer_on = 0;
            } else {
                LED2_PIN = 1; // Liga o LED1
                BUZZER_PIN = 1; // Liga o buzzer
                alarm2_on = 1;
                buzzer_on = 1;
            }

            updateDisplay();  // Atualizar a tela LCD
            delayMs(200); // Debounce do botão
        } else if (BUTTON2_PIN == 1) {
            lastButton2State = 1; // Atualize o estado do botão 2
        }

        // Verifique o estado do botão 3
        if (BUTTON3_PIN == 0 && lastButton3State == 1) { // Botão 3 pressionado
            lastButton3State = 0; // Atualize o estado do botão 3

            BUZZER_PIN = 0; // Desliga o buzzer
            buzzer_on = 0;

            delayMs(200); // Debounce do botão
        } else if (BUTTON3_PIN == 1) {
            lastButton3State = 1; // Atualize o estado do botão 3
        }
        
        // Iniciar a amostragem e conversão do ADC
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        delayMs(1);  // Aguardar 1 ms
        AD1CON1bits.SAMP = 0;  // Iniciar conversão

        while (!AD1CON1bits.DONE);  // Aguardar a conversão terminar
        adc_value = ADC1BUF0;  // Ler valor ADC

        if (adc_value > VOLTAGE_THRESHOLD) {
            if (!alarm3_on && !buzzer_on) {
                LED3_PIN = 1;  // Liga o LED2
                BUZZER_PIN = 1;  // Liga o buzzer
                alarm3_on = 1;
                buzzer_on = 1;
            } else if (alarm3_on && !buzzer_on) {
                LED3_PIN = 1;  // Liga o LED2
                BUZZER_PIN = 0;  // Desliga o buzzer
                alarm3_on = 1;
                buzzer_on = 0;
            }
            
            updateDisplay();  // Atualizar a tela LCD
        } else {
            if (alarm3_on && buzzer_on) {
                LED3_PIN = 0;  // Desliga o LED2
                BUZZER_PIN = 0;  // Desliga o buzzer
                alarm3_on = 0;
                buzzer_on = 0;
            } else if (alarm3_on) {
                LED3_PIN = 0;  // Desliga o LED2
                BUZZER_PIN = 0;  // Desliga o buzzer
                alarm3_on = 0;
                buzzer_on = 0;
            }
            
            updateDisplay();  // Atualizar a tela LCD
        }
    }

    return 0;
}