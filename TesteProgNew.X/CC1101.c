#include <xc.h>
#include <plib.h> // Include the PIC32 Peripheral Library.
#include "CC1101.h"

#define GDO0 PORTGbits.RG13 // RE8 = INT1
#define GDO1 PORTGbits.RG8 // SO/GDO1
#define CS PORTGbits.RG9

#define BYTES_IN_RXFIFO 0x7F
#define CRC_OK 0x80

// Configura??o do SPI
void SPI_Init(void) {
    
    
    //     Configurar os pinos do SPI2
    //TRISGbits.TRISG6 = 0; // SCK
    //TRISGbits.TRISG7 = 0; // MOSI
    //TRISGbits.TRISG8 = 1; // MISO
    //TRISGbits.TRISG9 = 0; // CS
    //TRISGbits.TRISG13 = 0; // DC
    //TRISGbits.TRISG14 = 0; // RESET
   
    // Configura??o dos pinos do CC1101
    TRISGbits.TRISG9 = 0; // Configura o pino CS como sa?da
    TRISGbits.TRISG7 = 0; // Configura o pino SI como sa?da
    TRISGbits.TRISG8 = 1; // Configura o pino SO/GDO1 como entrada
    TRISGbits.TRISG13 = 1; // Configura o pino GDO0 como entrada
    TRISGbits.TRISG6 = 0; // Configura o pino SCLK como sa?da

    // Inicializa??o dos pinos
    PORTGbits.RG9 = 1; // Inicializa o pino CS em alto
    PORTGbits.RG7 = 0; // Inicializa o pino SI em baixo
    PORTGbits.RG6 = 0; // Inicializa o pino SCLK em baixo
    
    // Configura??o do SPI
    SpiChnOpen(SPI_CHANNEL2, SPI_OPEN_MSTEN | SPI_OPEN_MODE8 | SPI_OPEN_ON, 2);
    //SPI_CHANNEL2: Este ? o canal SPI que est? sendo aberto.
    //SPI_OPEN_MSTEN | SPI_OPEN_MODE8 | SPI_OPEN_ON: Estas s?o as configura??es para 
    //o canal SPI. SPI_OPEN_MSTEN configura o m?dulo SPI como mestre. SPI_OPEN_MODE8 
    //configura o m?dulo SPI para o modo de 8 bits. SPI_OPEN_ON ativa o m?dulo SPI.
    //2: Este ? o divisor de velocidade do rel?gio SPI. A velocidade do rel?gio SPI 
    //? o rel?gio do sistema dividido por este valor.
}

void CC1101_WriteReg(uint8_t addr, uint8_t value) {
    CS = 0; // Ativa o chip selecionando o pino CS

    // Envia o endere?o do registro e o valor para o m?dulo CC1101
    SpiChnPutC(SPI_CHANNEL2, addr);
    SpiChnPutC(SPI_CHANNEL2, value);

    CS = 1; // Desativa o chip desselecionando o pino CS
}

void CC1101_WriteBurstReg(uint8_t addr, uint8_t* data, uint8_t length) {
    uint8_t i;

    CS = 0; // Ativa o chip selecionando o pino CS

    // Envia o endere?o do registro para o m?dulo CC1101
    SpiChnPutC(SPI_CHANNEL2, addr | 0x40); // O bit 6 ? definido para indicar uma opera??o de escrita em burst

    // Envia os dados
    for(i = 0; i < length; i++) {
        SpiChnPutC(SPI_CHANNEL2, data[i]);
    }

    CS = 1; // Desativa o chip desselecionando o pino CS
}

void CC1101_Strobe(uint8_t cmd) {
    CS = 0; // Ativa o chip selecionando o pino CS
    
    // Envia o comando strobe para o m?dulo CC1101
    SpiChnPutC(SPI_CHANNEL2, cmd);
    
    CS = 1; // Desativa o chip desselecionando o pino CS
}


uint8_t CC1101_ReadReg(uint8_t addr) {
    uint8_t value;

    CS = 0; // Ativa o chip selecionando o pino CS

    // Envia o endere?o do registro para o m?dulo CC1101
    SpiChnPutC(SPI_CHANNEL2, addr | 0x80); // O bit mais significativo ? definido para indicar uma opera??o de leitura

    // L? o valor do registro
    value = SpiChnGetC(SPI_CHANNEL2);

    CS = 1; // Desativa o chip desselecionando o pino CS

    return value;
}

void CC1101_ReadBurstReg(uint8_t addr, uint8_t* data, uint8_t length) {
    uint8_t i;

    CS = 0; // Ativa o chip selecionando o pino CS

    // Envia o endere?o do registro para o m?dulo CC1101
    SpiChnPutC(SPI_CHANNEL2, addr | 0xC0); // O bit mais significativo e o segundo bit mais significativo s?o definidos para indicar uma opera??o de leitura em burst

    // L? os valores dos registros
    for(i = 0; i < length; i++) {
        data[i] = SpiChnGetC(SPI_CHANNEL2);
    }

    CS = 1; // Desativa o chip desselecionando o pino CS
}

// Inicializa??o do CC1101
void CC1101_Init(void) {
    // Configura??o do CC1101
    CC1101_WriteReg(CC1101_IOCFG2,  CC1101_DEFVAL_IOCFG2);
    CC1101_WriteReg(CC1101_IOCFG1,  CC1101_DEFVAL_IOCFG1);
    CC1101_WriteReg(CC1101_IOCFG0,  CC1101_DEFVAL_IOCFG0);
    CC1101_WriteReg(CC1101_FIFOTHR,  CC1101_DEFVAL_FIFOTHR);
    CC1101_WriteReg(CC1101_PKTLEN,  CC1101_DEFVAL_PKTLEN);
    CC1101_WriteReg(CC1101_PKTCTRL1,  CC1101_DEFVAL_PKTCTRL1);
    CC1101_WriteReg(CC1101_PKTCTRL0,  CC1101_DEFVAL_PKTCTRL0);
    CC1101_WriteReg(CC1101_FSCTRL1,  CC1101_DEFVAL_FSCTRL1);
    CC1101_WriteReg(CC1101_FSCTRL0,  CC1101_DEFVAL_FSCTRL0);
    CC1101_WriteReg(CC1101_MDMCFG3,  CC1101_DEFVAL_MDMCFG3);
    CC1101_WriteReg(CC1101_MDMCFG2,  CC1101_DEFVAL_MDMCFG2);
    CC1101_WriteReg(CC1101_MDMCFG1,  CC1101_DEFVAL_MDMCFG1);
    CC1101_WriteReg(CC1101_MDMCFG0,  CC1101_DEFVAL_MDMCFG0);
    CC1101_WriteReg(CC1101_DEVIATN,  CC1101_DEFVAL_DEVIATN);
    CC1101_WriteReg(CC1101_MCSM2,  CC1101_DEFVAL_MCSM2);
    CC1101_WriteReg(CC1101_MCSM1,  CC1101_DEFVAL_MCSM1);
    CC1101_WriteReg(CC1101_MCSM0,  CC1101_DEFVAL_MCSM0);
    CC1101_WriteReg(CC1101_FOCCFG,  CC1101_DEFVAL_FOCCFG);
    CC1101_WriteReg(CC1101_BSCFG,  CC1101_DEFVAL_BSCFG);
    CC1101_WriteReg(CC1101_AGCCTRL2,  CC1101_DEFVAL_AGCCTRL2);
    CC1101_WriteReg(CC1101_AGCCTRL1,  CC1101_DEFVAL_AGCCTRL1);
    CC1101_WriteReg(CC1101_AGCCTRL0,  CC1101_DEFVAL_AGCCTRL0);
    CC1101_WriteReg(CC1101_WOREVT1,  CC1101_DEFVAL_WOREVT1);
    CC1101_WriteReg(CC1101_WOREVT0,  CC1101_DEFVAL_WOREVT0);
    CC1101_WriteReg(CC1101_WORCTRL,  CC1101_DEFVAL_WORCTRL);
    CC1101_WriteReg(CC1101_FREND1,  CC1101_DEFVAL_FREND1);
    CC1101_WriteReg(CC1101_FREND0,  CC1101_DEFVAL_FREND0);
    CC1101_WriteReg(CC1101_FSCAL3,  CC1101_DEFVAL_FSCAL3);
    CC1101_WriteReg(CC1101_FSCAL2,  CC1101_DEFVAL_FSCAL2);
    CC1101_WriteReg(CC1101_FSCAL1,  CC1101_DEFVAL_FSCAL1);
    CC1101_WriteReg(CC1101_FSCAL0,  CC1101_DEFVAL_FSCAL0);
    CC1101_WriteReg(CC1101_RCCTRL1,  CC1101_DEFVAL_RCCTRL1);
    CC1101_WriteReg(CC1101_RCCTRL0,  CC1101_DEFVAL_RCCTRL0);
    CC1101_WriteReg(CC1101_FSTEST,  CC1101_DEFVAL_FSTEST);
    CC1101_WriteReg(CC1101_PTEST,  CC1101_DEFVAL_PTEST);
    CC1101_WriteReg(CC1101_AGCTEST,  CC1101_DEFVAL_AGCTEST);
    CC1101_WriteReg(CC1101_TEST2,  CC1101_DEFVAL_TEST2);
    CC1101_WriteReg(CC1101_TEST1,  CC1101_DEFVAL_TEST1);
    CC1101_WriteReg(CC1101_TEST0,  CC1101_DEFVAL_TEST0);
}

// Enviar dados
void CC1101_SendData(uint8_t* data, uint8_t length) {
    
    CC1101_WriteReg(CC1101_TXFIFO, length);
    CC1101_WriteBurstReg(CC1101_TXFIFO, data, length); // Escreve os dados a serem enviados
    CC1101_Strobe(CC1101_STX); // Envia o comando para transmitir
    while(!GDO0);// Espera o GDO0 chegar no n?vel alto -> sync
    while(GDO0);// Aguarda enquanto GDO0 estiver alto -> aguarda transmissao
    CC1101_Strobe(CC1101_SFTX);
}

// Verificar o buffer de recep??o
uint8_t CC1101_ReadStatus(uint8_t addr) {
    uint8_t value;

    CS = 0; // Ativa o chip selecionando o pino CS

    // Envia o endere?o do registro de status para o m?dulo CC1101
    // O bit mais significativo e o segundo bit mais significativo s?o definidos para indicar uma opera??o de leitura de status
    SpiChnPutC(SPI_CHANNEL2, addr | 0xC0);

    // L? o valor do registro de status
    value = SpiChnGetC(SPI_CHANNEL2);

    CS = 1; // Desativa o chip desselecionando o pino CS

    return value;
}

// Receber dados
uint8_t CC1101_ReceiveData(uint8_t* rxBuffer, uint8_t* length) {
    uint8_t status[2];
    uint8_t packetLength;

    // Coloca o dispositivo em modo de recep??o
    CC1101_Strobe(CC1101_SRX);

    // Entra em um loop at? que a recep??o de um pacote comece e termine
    while (!GDO1);
    while (GDO1);

    // Verifica se h? bytes no buffer de recep??o
    if ((CC1101_ReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)) {
        // L? o tamanho do pacote
        packetLength = CC1101_ReadReg(CC1101_RXFIFO);

        // Se o tamanho do pacote for menor ou igual ao tamanho fornecido
        if (packetLength <= *length) {
            // L? o pacote do buffer de recep??o para o rxBuffer
            CC1101_ReadBurstReg(CC1101_RXFIFO, rxBuffer, packetLength);
            *length = packetLength; // Modifica o comprimento para o tamanho atual dos dados

            // L? os dois bytes de status
            CC1101_ReadBurstReg(CC1101_RXFIFO, status, 2);

            // Limpa o buffer de recep??o
            CC1101_Strobe(CC1101_SFRX);

            // Retorna se a verifica??o CRC foi bem-sucedida
            return (status[1] & CRC_OK);
        } else {
            *length = packetLength;
            CC1101_Strobe(CC1101_SFRX); // Limpa o buffer de recep??o
            return 0;
        }
    } else {
        return 0;
    }
}