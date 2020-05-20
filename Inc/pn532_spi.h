
#ifndef __PN532_SPI_H__
#define __PN532_SPI_H__

#include "spi_functions.h"

#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_SPACE                (-4)



void spi_begin(SPI_TypeDef * SPIx, uint8_t ss);
void spi_wakeup();
int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);

int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);

int isReady();
void writeFrame(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
int8_t readAckFrame();

//void write(uint8_t data);
//uint8_t read();

#endif
