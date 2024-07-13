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
#define PWR_LED LATAbits.LATA1        // Define o LED que indica o funcionamento no pino RA1

#define BUZZER_PIN LATAbits.LATA2     // Define o buzzer no pino RA2

#define BUTTON1_PIN PORTDbits.RD0     // Define o botão 1 no pino RD0 (INT0)
#define BUTTON2_PIN PORTEbits.RE8     // Define o botão 2 no pino RD1 (INT1)

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

void setup()
{
    DDPCONbits.JTAGEN = 0;

    TRISAbits.TRISA0 = 0; // LED1 como saída
    TRISAbits.TRISA1 = 0; // PWR_LED como saída
    TRISAbits.TRISA2 = 0; // BUZZER como saída
    TRISDbits.TRISD0 = 1;  // Botão 1 como entrada
    TRISEbits.TRISE8 = 1;  // Botão 2 como entrada

    LED1_PIN = 0;
    BUZZER_PIN = 0;

    // Configuração das interrupções externas
    INTCONbits.INT0EP = 1;  // Interrupção na borda de subida para INT0
    IEC0bits.INT0IE = 1;    // Habilitar interrupção INT0
    IFS0bits.INT0IF = 0;    // Limpar flag de interrupção INT0

    INTCONbits.INT1EP = 1;  // Interrupção na borda de subida para INT1
    IEC0bits.INT1IE = 1;    // Habilitar interrupção INT1
    IFS0bits.INT1IF = 0;    // Limpar flag de interrupção INT1

    // Habilite as interrupções globais
    __builtin_enable_interrupts();
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
    
    delayMs(200);  // Debounce do botão
}

void __ISR(_EXTERNAL_1_VECTOR, IPL7AUTO) INT1InterruptHandler(void) {
    IFS0bits.INT2IF = 0;  // Limpar flag de interrupção INT2
    
    BUZZER_PIN = 0;  // Desliga o buzzer
    buzzer_on = 0;

    delayMs(200);  // Debounce do botao
}

int main()
{
    
    while( 1)
    {
        PWR_LED = 1;
        delayMs(500);
    }
    
    return 0;
}*/