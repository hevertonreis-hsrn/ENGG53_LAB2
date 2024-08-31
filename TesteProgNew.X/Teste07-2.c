#include <xc.h>
#include "CC1101.h"  // Biblioteca para o m?dulo RF CC1101

#pragma config FPLLMUL = MUL_20 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLODIV = DIV_1
#pragma config POSCMOD = HS
#pragma config FNOSC = PRIPLL
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF

#define LED_PIN LATAbits.LATA0     // LED no pino RA0

void setup() {
    DDPCONbits.JTAGEN = 0;

    TRISAbits.TRISA0 = 0; // LED como sa?da
    LED_PIN = 0;          // Inicia com o LED apagado

    CC1101_Init();        // Inicializa o m?dulo RF CC1101
}

void loop() {
    int received_state;
    uint8_t data_length = sizeof(received_state);  // Define o comprimento m?ximo esperado

    if (CC1101_ReceiveData((uint8_t*)&received_state, &data_length)) {
        // received_state agora cont?m os dados recebidos
    }

}

int main() {
    setup();
    while (1) {
        loop();
    }
    return 0;
}