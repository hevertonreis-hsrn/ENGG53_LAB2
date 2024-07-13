/*#include <xc.h>
#include <stdio.h>
#include <sys/attribs.h>

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

void delayMs(unsigned t) {
    unsigned long startTime = _CP0_GET_COUNT(); // Obtenha o valor inicial do Core Timer
    unsigned long delayTicks = (SYS_FREQ / 2) / 1000; // 1 ms em ticks do Core Timer

    while (t--) {
        while (_CP0_GET_COUNT() - startTime < delayTicks); // Aguarde 1 ms
        startTime += delayTicks; // Atualize o valor inicial para o próximo atraso
    }
}

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

    // Habilite o pull-up interno para os pinos RF4 e RF5
    CNPUEbits.CNPUE17 = 1; // Habilita pull-up para RF4
    CNPUEbits.CNPUE18 = 1; // Habilita pull-up para RF5
    CNPUEbits.CNPUE19 = 1; // Habilita pull-up para RD13

    // Habilite as interrupções globais
    __builtin_enable_interrupts();
}

int main() {
    setup();

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

            delayMs(200); // Debounce do botão
        } else if (BUTTON1_PIN == 1) {
            lastButton1State = 1; // Atualize o estado do botão 1
        }
        
        // Verifique o estado do botão 
        if (BUTTON2_PIN == 0 && lastButton2State == 1) { // Botão 1 pressionado
            lastButton2State = 0; // Atualize o estado do botão 1

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

            delayMs(200); // Debounce do botão
        } else if (BUTTON2_PIN == 1) {
            lastButton2State = 1; // Atualize o estado do botão 1
        }

        // Verifique o estado do botão 3
        if (BUTTON3_PIN == 0 && lastButton3State == 1) { // Botão 3 pressionado
            lastButton3State = 0; // Atualize o estado do botão 3

            BUZZER_PIN = 0; // Desliga o buzzer
            buzzer_on = 0;

            delayMs(200); // Debounce do botão
        } else if (BUTTON3_PIN == 1) {
            lastButton3State = 1; // Atualize o estado do botão 2
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
        }
    }

    return 0;
}*/