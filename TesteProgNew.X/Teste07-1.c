#include <xc.h>
#include "delay.h"
#include "CC1101.h"  // Biblioteca para o m?dulo RF CC1101

#pragma config FPLLMUL = MUL_20 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF

#define LED_PIN LATAbits.LATA0     // LED no pino RA0
#define BUTTON_PIN PORTFbits.RF4   // Bot?o no pino RF4

volatile int led_state = 0;        // Estado atual do LED

void setup() {
    DDPCONbits.JTAGEN = 0;

    TRISAbits.TRISA0 = 0; // LED como sa?da
    TRISFbits.TRISF4 = 1; // Bot?o como entrada

    LED_PIN = 0;          // Inicia com o LED apagado

    CC1101_Init();        // Inicializa o m?dulo RF CC1101
}

void loop() {
    static int lastButtonState = 1; // Estado anterior do bot?o

    if (BUTTON_PIN == 0 && lastButtonState == 1) { // Bot?o pressionado
        lastButtonState = 0; // Atualiza o estado do bot?o

        led_state = !led_state;     // Alterna o estado do LED
        LED_PIN = led_state;        // Atualiza o LED

        // Transmite o estado do LED via RF
        uint8_t led_state_8bit = (uint8_t)led_state; // Converte para uint8_t
        CC1101_SendData(&led_state_8bit, sizeof(led_state_8bit));


        delayMs(100); // Debounce do bot?o
    } else if (BUTTON_PIN == 1) {
        lastButtonState = 1; // Atualiza o estado do bot?o
    }
}

int main() {
    setup();
    while (1) {
        loop();
    }
    return 0;
}