#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include "spi_functions.h"
#include <uart_functions.h>
#include "pn532_spi.h"
#include <config.h>
#include <string.h>
#include <stdio.h>

#define STATUS_READ     2
#define DATA_WRITE      1
#define DATA_READ       3

uint8_t command;
char buffer[100];

void begin(){
    spiInit(SPI1);
}

void wakeup() {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    Delay(2);
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
}



int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen){
  command = header[0];
  writeFrame(header, hlen, body, blen);

  uint8_t timeout = PN532_ACK_WAIT_TIME;
  while (!isReady()) {
      Delay(1);
      timeout--;
      if (0 == timeout) {
          return -2;
      }
  }
  if (readAckFrame()) {
      return PN532_INVALID_ACK;
  }
  return 0;
}

int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout){
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

      length -= 2;
      if (length > len) {
          for (uint8_t i = 0; i < length; i++) {
              read();                 // dump message
          }
          read();
          read();
          result = PN532_NO_SPACE;  // not enough space
          break;
      }

      uint8_t sum = PN532_PN532TOHOST + cmd;
      for (uint8_t i = 0; i < length; i++) {
          buf[i] = read();
          sum += buf[i];

      }

      uint8_t checksum = read();
      if (0 != (uint8_t)(sum + checksum)) {
          result = PN532_INVALID_FRAME;
          break;
      }
      read();         // POSTAMBLE

      result = length;
  } while (0);

  GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

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

    for (uint8_t i = 0; i < hlen; i++) {
        write(header[i]);
        sum += header[i];

    }
    for (uint8_t i = 0; i < blen; i++) {
        write(body[i]);
        sum += body[i];

    }

    uint8_t checksum = ~sum + 1;        // checksum of TFI + DATA
    write(checksum);
    write(PN532_POSTAMBLE);

    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);

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
