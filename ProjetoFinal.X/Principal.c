#include <xc.h>
#include <plib.h>
#include <stdio.h>
#include "ILI9341.h"
#include "SPI2.h"
#include "delay.h"

// Configurações do clock principal para 80 MHz (8 MHz FRC com PLL)
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF

#define SYS_FREQ (80000000L)          // Frequência do sistema em 80 MHz
#define VOLTAGE_THRESHOLD 512         // Limite de tensão para acionar o LED2 e o buzzer
 
#define LED1_PIN LATAbits.LATA0       // Define o LED1 no pino RB0
#define LED2_PIN LATAbits.LATA1       // Define o LED2 no pino RB1
#define LED3_PIN LATAbits.LATA2       // Define o LED3 no pino RB2
#define PWR_LED LATAbits.LATA4

#define BUZZER_PIN LATAbits.LATA3     // Define o buzzer no pino RB3
#define BUTTON1_PIN PORTDbits.RD0     // Define o botão 1 no pino RD0 (INT0)
#define BUTTON2_PIN PORTEbits.RE8     // Define o botão 2 no pino RD1 (INT1)
#define BUTTON3_PIN PORTEbits.RE9     // Define o botão 3 no pino RD2 (INT2)

#define ADC_PIN 0                     // Define o pino ADC no pino AN0

void setup() {
    // Desabilita a porta JTAG 
    DDPCONbits.JTAGEN = 0;
    
    // Configuração dos pinos como entrada ou saída
    TRISBbits.TRISB0 = 0;  // LED1 como saída
    TRISBbits.TRISB1 = 0;  // LED2 como saída
    TRISBbits.TRISB2 = 0;  // LED3 como saída
    TRISBbits.TRISB3 = 0;  // Buzzer como saída

    TRISDbits.TRISD0 = 1;  // Botão 1 como entrada
    TRISEbits.TRISE8 = 1;  // Botão 2 como entrada
    TRISEbits.TRISE9 = 1;  // Botão 3 como entrada

    // Inicialização dos estados dos LEDs e Buzzer
    LED1_PIN = 0;
    LED2_PIN = 0;
    LED3_PIN = 0;
    BUZZER_PIN = 0;

    // Configuração do ADC
    AD1CON1 = 0x00E0;       // Auto-convert mode
    AD1CHS = ADC_PIN;       // Selecionar o pino ADC
    AD1CSSL = 0;
    AD1CON3 = 0x1F02;       // Tempo de amostragem manual
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;   // Ligar o ADC

    // Habilitação de interrupções ADC
    IFS1bits.AD1IF = 0;     // Limpar flag de interrupção ADC
    IEC1bits.AD1IE = 1;     // Habilitar interrupção ADC

    // Configuração das interrupções externas
    INTCONbits.INT0EP = 1;  // Interrupção na borda de subida para INT0
    IEC0bits.INT0IE = 1;    // Habilitar interrupção INT0
    IFS0bits.INT0IF = 0;    // Limpar flag de interrupção INT0

    INTCONbits.INT1EP = 1;  // Interrupção na borda de subida para INT1
    IEC0bits.INT1IE = 1;    // Habilitar interrupção INT1
    IFS0bits.INT1IF = 0;    // Limpar flag de interrupção INT1

    INTCONbits.INT2EP = 1;  // Interrupção na borda de subida para INT2
    IEC0bits.INT2IE = 1;    // Habilitar interrupção INT2
    IFS0bits.INT2IF = 0;    // Limpar flag de interrupção INT2
    
    // Configurar os pinos do SPI2
    TRISGbits.TRISG6 = 0; // SCK como saida - FIXO
    TRISGbits.TRISG8 = 0; // SDI do LCD/SDO do PIC como saida - FIXO
    TRISCbits.TRISC2 = 0; // RST como saida (configurar conforme necessidade)
    TRISGbits.TRISG9 = 0; // DC como saida
    //TRISGbits.TRISG7 = 1; // Habilitar se necessário

    // Inicialização do LCD
    SPI2_Initialize();
    ILI9341_Init();
    setOrientation(LANDSCAPE);
    bg(WHITE);

    // Habilitar interrupções globais
    __builtin_enable_interrupts();
}

// Variáveis observadas
volatile int alarm1_on = 0;
volatile int alarm2_on = 0;
volatile int alarm3_on = 0;
volatile int buzzer_on = 0;
volatile unsigned int adc_value = 0;

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

void __ISR(_EXTERNAL_0_VECTOR, IPL7AUTO) INT0InterruptHandler(void) {
    IFS0bits.INT0IF = 0;  // Limpar flag de interrupção INT0

    if (alarm1_on && buzzer_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        alarm1_on = 0;
        buzzer_on = 0;
    } else if (alarm1_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        alarm1_on = 0;
        buzzer_on = 0;
    } else {
        LED1_PIN = 1;  // Liga o LED1
        BUZZER_PIN = 1;  // Liga o buzzer
        alarm1_on = 1;
        buzzer_on = 1;
    }

    updateDisplay();  // Atualizar a tela LCD
    delayMs(200);  // Debounce do botão
}

void __ISR(_EXTERNAL_1_VECTOR, IPL7AUTO) INT1InterruptHandler(void) {
    IFS0bits.INT1IF = 0;  // Limpar flag de interrupção INT1
    
    if (alarm2_on && buzzer_on) {
        LED2_PIN = 0;  // Desliga o LED2
        BUZZER_PIN = 0;  // Desliga o buzzer
        alarm2_on = 0;
        buzzer_on = 0;
    } else if (alarm2_on) {
        LED2_PIN = 0;  // Desliga o LED2
        BUZZER_PIN = 0;  // Desliga o buzzer
        alarm2_on = 0;
        buzzer_on = 0;
    } else {
        LED2_PIN = 1;  // Liga o LED2
        BUZZER_PIN = 1;  // Liga o buzzer
        alarm2_on = 1;
        buzzer_on = 1;
    }

    updateDisplay();  // Atualizar a tela LCD
    delayMs(200);  // Debounce do botão
}

void __ISR(_EXTERNAL_2_VECTOR, IPL7AUTO) INT2InterruptHandler(void) {
    IFS0bits.INT2IF = 0;  // Limpar flag de interrupção INT2
    
    BUZZER_PIN = 0;  // Desliga o buzzer
    buzzer_on = 0;

    delayMs(200);  // Debounce do botao
}

void __ISR(_ADC_VECTOR, IPL7SRS) _ADC1Interrupt(void) {
    IFS1bits.AD1IF = 0;  // Limpar flag de interrupção ADC

    adc_value = ADC1BUF0;  // Ler valor ADC

    if (adc_value > VOLTAGE_THRESHOLD) {
        if (!alarm3_on && !buzzer_on) {
            LED3_PIN = 1;  // Liga o LED2
            BUZZER_PIN = 1;  // Liga o buzzer
            alarm3_on = 1;
            buzzer_on = 1;
        } else if (alarm3_on && !buzzer_on) {
            LED3_PIN = 1;  // Liga o LED3
            BUZZER_PIN = 0;  // Liga o buzzer
            alarm3_on = 1;
            buzzer_on = 0;
        }
    } else {
        if (alarm3_on && buzzer_on) {
            LED3_PIN = 0;  // Desliga o LED3
            BUZZER_PIN = 0;  // Desliga o buzzer
            alarm3_on = 0;
            buzzer_on = 0;
        } else if (alarm3_on) {
            LED3_PIN = 0;  // Desliga o LED3
            BUZZER_PIN = 0;  // Desliga o buzzer
            alarm3_on = 0;
            buzzer_on = 0;
        }
    }

    updateDisplay();  // Atualizar a tela LCD
}

int main() {
    
    setup();
    
    setupDisplay();
    
    PWR_LED = 1;

    while (1) {
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        delayMs(1);  // Aguardar 1 ms
        AD1CON1bits.SAMP = 0;  // Iniciar conversao

        while (!AD1CON1bits.DONE);  // Aguardar a conversao terminar
    }

    return 0;
}