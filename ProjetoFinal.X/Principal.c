#include <xc.h>
#include <plib.h>

// Configuração dos clock principal para 80 MHz
#pragma config FNOSC = PRIPLL // Seleção do oscilador
#pragma config POSCMOD = HS // Modo do oscilador primário
#pragma config FPLLIDIV = DIV_2 // Divisor de entrada do PLL
#pragma config FPLLMUL = MUL_20 // Multiplicador do PLL
#pragma config FPLLODIV = DIV_1 // Divisor de saída do PLL
#pragma config FPBDIV = DIV_1 // Divisor do clock do barramento periférico
#pragma config FWDTEN = OFF // Timer do watchdog desativado

#define LED1_PIN LATAbits.LATA0     // Define o LED1 no pino RA0
#define LED2_PIN LATAbits.LATA2     // Define o LED2 no pino RA2
#define LED3_PIN LATAbits.LATA3     // Define o LED3 no pino RA3

#define BUZZER_PIN LATAbits.LATA1   // Define o buzzer no pino RA1
#define BUTTON1_PIN PORTFbits.RF8   // Define o botão 1 no pino RF8 ### Corrigir para porta do pino 72 (INT0)
#define BUTTON2_PIN PORTFbits.RF3   // Define o botão 2 no pino RF3 ### Corrigir para porta do pino 18 (INT1)
#define BUTTON3_PIN PORTFbits.RF5   // Define o botão 3 no pino RF4 ### Corrigir para porta do pino 19 (INT2)

#define ADC_PIN 0                   // Define o pino ADC no pino AN0

#define SYS_FREQ (80000000L)        // Frequência do sistema em 32 MHz
#define VOLTAGE_THRESHOLD 512       // Limite de tensão para acionar o LED2 e o buzzer

void delayMs(int ms) {
    // Configura o TIMER1
    T1CON = 0x8000; // Liga o TIMER1, prescaler 1:1
    TMR1 = 0; // Limpa o contador

    while (ms > 0) {
        // Configura o TIMER1 para contar 1ms
        PR1 = (SYS_FREQ / 2) / 1000 - 1;
        TMR1 = 0; // Limpa o contador
        IFS0bits.T1IF = 0; // Limpa a flag de interrupção do TIMER1

        // Aguarda até que o TIMER1 conte 1ms
        while (IFS0bits.T1IF == 0);

        ms--; // Decrementa a contagem de ms
    }
}

void __delay_ms(unsigned int ms) {
    while (ms--) {
        delayMs(1);  // 1 ms delay, delayMs é uma função do XC32 para um atraso de 1 ms
    }
}

void setup() {
    TRISAbits.TRISA0 = 0;  // Configura o pino do LED1 como saída
    TRISAbits.TRISA2 = 0;  // Configura o pino do LED2 como saída
    TRISAbits.TRISA3 = 0;  // Configura o pino do LED3 como saída
    
    TRISAbits.TRISA1 = 0;  // Configura o pino do buzzer como saída
    
    TRISFbits.TRISF8 = 1;  // Configura o pino do botão 1 como entrada
    TRISFbits.TRISF3 = 1;  // Configura o pino do botão 2 como entrada
    TRISFbits.TRISF5 = 1;  // Configura o pino do botão 3 como entrada

    LED1_PIN = 0;  // Desliga o LED1
    LED2_PIN = 0;  // Desliga o LED2
    LED3_PIN = 0;  // Desliga o LED3
    BUZZER_PIN = 0;  // Desliga o buzzer

    // Configurar ADC para operação automática com interrupções
    AD1CON1 = 0x04E0;  // Configurar ADC para operação automática com interrupções
    AD1CHS = ADC_PIN;  // Selecionar o pino ADC
    AD1CSSL = 0;
    AD1CON3 = 0x1F02;  // Configurar ADC para amostragem manual
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;  // Habilitar ADC

    // Habilitar interrupção ADC
    IFS1bits.AD1IF = 0;  // Limpar flag de interrupção ADC
    IEC1bits.AD1IE = 1;  // Habilitar interrupção ADC

    // Configurar INT0
    INTCONbits.INT0EP = 1;  // Interrupção na borda de subida
    IFS0bits.INT0IF = 0;     // Limpar flag de interrupção
    IEC0bits.INT0IE = 1;     // Habilitar interrupção

    // Configurar INT1
    INTCONbits.INT1EP = 1;  // Interrupção na borda de subida
    IFS0bits.INT1IF = 0;     // Limpar flag de interrupção
    IEC0bits.INT1IE = 1;     // Habilitar interrupção

    // Configurar INT2
    INTCONbits.INT2EP = 1;  // Interrupção na borda de subida
    IFS0bits.INT2IF = 0;     // Limpar flag de interrupção
    IEC0bits.INT2IE = 1;     // Habilitar interrupção

    INTEnableSystemMultiVectoredInt();  // Habilitar interrupções globais
}

volatile int led1_on = 0;
volatile int led2_on = 0;
volatile int led3_on = 0;
volatile int buzzer_on = 0;

void __ISR(_EXTERNAL_0_VECTOR, IPL7SRS) _INT0Interrupt(void) {
    IFS0bits.INT0IF = 0;  // Limpar flag de interrupção

    if (led1_on && buzzer_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led1_on = 0;
        buzzer_on = 0;
    } else if (led1_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led1_on = 0;
        buzzer_on = 0;
    } else {
        LED1_PIN = 1;  // Liga o LED1
        BUZZER_PIN = 1;  // Liga o buzzer
        led1_on = 1;
        buzzer_on = 1;
    }
    __delay_ms(200);  // Debounce do botão
}

void __ISR(_EXTERNAL_1_VECTOR, IPL7SRS) _INT1Interrupt(void) {
    IFS0bits.INT1IF = 0;  // Limpar flag de interrupção

    BUZZER_PIN = 0;  // Desliga o buzzer
    buzzer_on = 0;
    __delay_ms(200);  // Debounce do botão
}

void __ISR(_EXTERNAL_2_VECTOR, IPL7SRS) _INT2Interrupt(void) {
    IFS0bits.INT2IF = 0;  // Limpar flag de interrupção

    if (led3_on && buzzer_on) {
        LED3_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led3_on = 0;
        buzzer_on = 0;
    } else if (led3_on) {
        LED3_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led3_on = 0;
        buzzer_on = 0;
    } else {
        LED3_PIN = 1;  // Liga o LED1
        BUZZER_PIN = 1;  // Liga o buzzer
        led3_on = 1;
        buzzer_on = 1;
    }
    __delay_ms(200);  // Debounce do botão
}

void __ISR(_ADC_VECTOR, IPL7SRS) _ADC1Interrupt(void) {
    IFS1bits.AD1IF = 0;  // Limpar flag de interrupção ADC

    if (ADC1BUF0 > VOLTAGE_THRESHOLD) {
        
        if (!led2_on && !buzzer_on) {
            LED2_PIN = 1;  // Liga o LED2
            BUZZER_PIN = 1;  // Liga o buzzer
            led2_on = 1;
            buzzer_on = 1;
        } else if (led2_on && !buzzer_on) {
            LED2_PIN = 1;  // Liga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 1;
            buzzer_on = 0;
        }
    } else {
        
        if (led2_on && buzzer_on) {
            LED2_PIN = 0;  // Desliga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 0;
            buzzer_on = 0;
        } else if (led2_on) {
            LED2_PIN = 0;  // Desliga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 0;
            buzzer_on = 0;
        }
    }
}

int main() {
    setup();

    while (1) {
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        __delay_ms(1);  // Aguardar 1 ms
        AD1CON1bits.SAMP = 0;  // Iniciar conversão

        while (!AD1CON1bits.DONE);  // Aguardar a conversão terminar
    }

    return 0;
}