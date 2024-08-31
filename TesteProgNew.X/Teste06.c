#include <xc.h>
#include <stdio.h>
#include "ILI9341.h"
#include "SPI2.h"
#include "delay.h"

// Configurações do clock principal para 80 MHz
#pragma config FPLLMUL = MUL_20
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF

// DEVCFG0
#pragma config DEBUG = ON               // Background Debugger Enable (Debugger is enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#define SYS_FREQ 80000000L

#define LED_PIN LATAbits.LATA0       // Define o LED no pino RA0
#define BUTTON_PIN PORTFbits.RF4     // Define o botão no pino RF4
#define PWR_LED LATAbits.LATA7

// Variaeis observadas
volatile int alarm_on = 0;

void updateDisplay() {
    int cardWidth = 100;
    int cardHeight = 60;
    int cardX = 110;  // Coordenada X do cartão (ajustada para centralizar)
    int cardY = 80;   // Coordenada Y do cartão (ajustada para centralizar)
    int color;
    char* text;

    if (alarm_on) {
        color = RED;
        text = "OUT";
    } else {
        color = GREEN;
        text = "OK";
    }

    fillRoundRect(cardX, cardY, cardWidth, cardHeight, 10, color);
    putString(text, cardX + 10, cardY + 15, 3, WHITE); // Ajuste da posição do texto
}

void setup() {
    
    DDPCONbits.JTAGEN = 0; // Desabilitar JTAG
    
    TRISAbits.TRISA0 = 0;  // Configurar LED como saída
    TRISAbits.TRISA7 = 0;  // Configurar LED como saída
    
    TRISFbits.TRISF4 = 1;  // Configurar botão como entrada
    
    LED_PIN = 0;           // Inicializar LED apagado
    PWR_LED = 0;

    // Configurar os pinos do SPI2
    TRISGbits.TRISG6 = 0; // SCK
    TRISGbits.TRISG7 = 0; // MOSI
    TRISGbits.TRISG8 = 1; // MISO
    TRISGbits.TRISG9 = 0; // CS
    TRISGbits.TRISG13 = 0; // DC
    TRISGbits.TRISG14 = 0; // RESET
    
     // Habilite o pull-up interno para os botoes
    CNPUEbits.CNPUE17 = 1; // Habilita pull-up para RF4

    // Inicialização do LCD
    SPI2_Initialize();    
    ILI9341_Init();
    setOrientation(LANDSCAPE);
    bg(WHITE);

    // Exibir estado inicial no display
    updateDisplay();
}

int main() {
    
    setup();

    int lastButtonState = 0;  
  
    while (1) {
        PWR_LED = 1;
       
        // Verifique o estado do botao 1
        if (BUTTON_PIN == 0 && lastButtonState == 1) { // Botao 1 pressionado
            lastButtonState = 0; // Atualize o estado do botao 1

            if (alarm_on) {
                LED_PIN = 0; // Desliga o LED1
                alarm_on = 0;
            } else {
                LED_PIN = 1; // Liga o LED1
                alarm_on = 1;
            }
            
            updateDisplay();  // Atualizar a tela LCD
            delayMs(100); // Debounce do botao
        } else if (BUTTON_PIN == 1) {
            lastButtonState = 1; // Atualize o estado do botao 1
        }
    }

    return 0;
}