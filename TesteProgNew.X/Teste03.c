/*#include <xc.h>
#include <stdio.h>

// Configurações do clock principal para 80 MHz (8 MHz FRC com PLL)
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF
 
#define LED1_PIN LATAbits.LATA0       // Define o LED1 no pino RA0
#define PWR_LED LATAbits.LATA1        // Define o LED que indica o funcionamento no pino RA1
#define BUZZER_PIN LATAbits.LATA2     // Define o buzzer no pino RA2
#define BUTTON1_PIN PORTFbits.RF4     // Define o botão 1 no pino RF4
#define BUTTON2_PIN PORTFbits.RF5     // Define o botão 2 no pino RF5

#define SYS_FREQ 80000000L

// Variáveis observadas
volatile int alarm1_on = 0;
volatile int buzzer_on = 0;

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
    TRISAbits.TRISA1 = 0; // PWR_LED como saída
    TRISAbits.TRISA2 = 0; // BUZZER como saída
    TRISFbits.TRISF4 = 1; // BUTTON1 como entrada
    TRISFbits.TRISF5 = 1; // BUTTON2 como entrada

    LED1_PIN = 0;
    PWR_LED = 0;
    BUZZER_PIN = 0;

    // Habilite o pull-up interno para os pinos RF4 e RF5
    CNPUEbits.CNPUE17 = 1; // Habilita pull-up para RF4
    CNPUEbits.CNPUE18 = 1; // Habilita pull-up para RF5

    // Habilite as interrupções globais
    __builtin_enable_interrupts();
}

int main() {
    setup();

    int lastButton1State = 1; // Estado anterior do botão 1
    int lastButton2State = 1; // Estado anterior do botão 2

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

        // Verifique o estado do botão 2
        if (BUTTON2_PIN == 0 && lastButton2State == 1) { // Botão 2 pressionado
            lastButton2State = 0; // Atualize o estado do botão 2

            BUZZER_PIN = 0; // Desliga o buzzer
            buzzer_on = 0;

            delayMs(200); // Debounce do botão
        } else if (BUTTON2_PIN == 1) {
            lastButton2State = 1; // Atualize o estado do botão 2
        }
    }

    return 0;
}*/