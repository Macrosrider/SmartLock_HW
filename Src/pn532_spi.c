
#include "pn532_spi.h"
#include <config.h>
#include "string.h"
#include "stdio.h"


#define STATUS_READ     2
#define DATA_WRITE      1
#define DATA_READ       3

uint8_t command;
char buffer[100];

void spi_begin(SPI_TypeDef * SPIx, uint8_t ss){
    command = 0;

    spiInit(SPI1);

}

void spi_wakeup(){
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    Delay(2);
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
}



int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    command = header[0];
    writeFrame(header, hlen, body, blen);

    uint8_t timeout = PN532_ACK_WAIT_TIME;
    while (!isReady()) {
        Delay(1);
        timeout--;
        if (0 == timeout) {
            sprintf(buffer, "Time out when waiting for ACK\n");
            uart_send(buffer);
            return -2;
        }
    }
    if (readAckFrame()) {
        sprintf(buffer, "Invalid ACK\n");
        uart_send(buffer);
        return PN532_INVALID_ACK;
    }
    return 0;
}

int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint16_t time = 0;
    while (!isReady()) {
        Delay(1);
        time++;
        if (timeout > 0 && time > timeout) {
            return PN532_TIMEOUT;
        }
    }

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    Delay(1);

    int16_t result;
    do {
        write(DATA_READ);

        if (0x00 != read()      ||       // PREAMBLE
                0x00 != read()  ||       // STARTCODE1
                0xFF != read()           // STARTCODE2
           ) {

            result = PN532_INVALID_FRAME;
            break;
        }

        uint8_t length = read();
        if (0 != (uint8_t)(length + read())) {   // checksum of length
            result = PN532_INVALID_FRAME;
            break;
        }

        uint8_t cmd = command + 1;               // response command
        if (PN532_PN532TOHOST != read() || (cmd) != read()) {
            result = PN532_INVALID_FRAME;
            break;
        }

        sprintf(buffer, "read: %2X ", cmd);
        uart_send(buffer);
        memset(buffer, 0, sizeof(buffer));

        length -= 2;
        if (length > len) {
            for (uint8_t i = 0; i < length; i++) {
                read();                 // dump message
            }
            //DMSG("\nNot enough space\n");
            read();
            read();
            result = PN532_NO_SPACE;  // not enough space
            break;
        }

        uint8_t sum = PN532_PN532TOHOST + cmd;
        for (uint8_t i = 0; i < length; i++) {
            buf[i] = read();
            sum += buf[i];
            sprintf(buffer, " %2X ", buf[i]);
            uart_send(buffer);
//            memset(buffer, 0, sizeof(buffer));
        }
        sprintf(buffer, "\n");
        uart_send(buffer);

        uint8_t checksum = read();
        if (0 != (uint8_t)(sum + checksum)) {
            sprintf(buffer, "checksum is not ok\n");
            uart_send(buffer);
            result = PN532_INVALID_FRAME;
            break;
        }
        read();         // POSTAMBLE

        result = length;
    } while (0);

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);;

    return result;
}

int isReady()
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);

    write(STATUS_READ);
    uint8_t status = read() & 1;
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    return status;
}

void writeFrame(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    Delay(2);               // wake up PN532

    write(DATA_WRITE);
    write(PN532_PREAMBLE);
    write(PN532_STARTCODE1);
    write(PN532_STARTCODE2);

    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    write(length);
    write(~length + 1);         // checksum of length

    write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA

    sprintf(buffer, "write: ");
    uart_send(buffer);
    memset(buffer, 0, sizeof(buffer));

    for (uint8_t i = 0; i < hlen; i++) {
        write(header[i]);
        sum += header[i];

        sprintf(buffer, " %2X ", header[i]);
        uart_send(buffer);
    }
    for (uint8_t i = 0; i < blen; i++) {
        write(body[i]);
        sum += body[i];

        sprintf(buffer, " %2X ", body[i]);
        uart_send(buffer);
    }

    uint8_t checksum = ~sum + 1;        // checksum of TFI + DATA
    write(checksum);
    write(PN532_POSTAMBLE);

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

    sprintf(buffer, "\n");
    uart_send(buffer);
}

int8_t readAckFrame(){
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};

    uint8_t ackBuf[sizeof(PN532_ACK)];

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    Delay(1);
    write(DATA_READ);

    for (uint8_t i = 0; i < sizeof(PN532_ACK); i++) {
        ackBuf[i] = read();
    }

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

    return memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK));
}
