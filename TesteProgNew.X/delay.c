#include "delay.h"
#include <xc.h>

#define SYS_FREQ 80000000L // Defina o clock do sistema para 80 MHz

void delayMs(unsigned t) {
    unsigned long startTime = _CP0_GET_COUNT(); // Obtenha o valor inicial do Core Timer
    unsigned long delayTicks = (SYS_FREQ / 2) / 1000; // 1 ms em ticks do Core Timer

    while (t--) {
        while (_CP0_GET_COUNT() - startTime < delayTicks); // Aguarde 1 ms
        startTime += delayTicks; // Atualize o valor inicial para o próximo atraso
    }
}