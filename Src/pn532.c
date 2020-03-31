//
// Created by macros on 30.03.20.
//
#include <pn532_spi.h>
#include <pn532.h>
#include <stdint.h>
#include <stdio.h>
#include <uart_functions.h>


uint8_t pn532_packetbuffer[64];
uint8_t inListedTag;
char buffer[100];

void nfcBegin(){
    begin();
    wakeup();
}

uint32_t getFirmwareVersion(void){
    uint32_t response;

    pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

    if (writeCommand(pn532_packetbuffer, 1, 0, 0)) {
        return 0;
    }

    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 1000);
    if (0 > status) {
        return 0;
    }

    response = pn532_packetbuffer[0];
    response <<= 8;
    response |= pn532_packetbuffer[1];
    response <<= 8;
    response |= pn532_packetbuffer[2];
    response <<= 8;
    response |= pn532_packetbuffer[3];

    return response;
}

int SAMConfig(){
    pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    pn532_packetbuffer[1] = 0x01;
    pn532_packetbuffer[2] = 0x14;
    pn532_packetbuffer[3] = 0x01;

    if (writeCommand(pn532_packetbuffer, 4, 0, 0)){
        return 1;
    }
    return (0 < readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 1000));
}

int inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength){
    uint8_t i;

    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = inListedTag;

    if (writeCommand(pn532_packetbuffer, 2, send, sendLength)) {
        return 1;
    }

    int16_t status = readResponse(response, *responseLength, 1000);
    if (status < 0) {
        return 1;
    }

    if ((response[0] & 0x3f) != 0) {
    //    DMSG("Status code indicates an error\n");
        return 1;
    }

    uint8_t length = status;
    length -= 1;

    if (length > *responseLength) {
        length = *responseLength;
    }

    for (uint8_t i = 0; i < length; i++) {
        response[i] = response[i + 1];
    }
    *responseLength = length;

    return 0;
}

int inListPassiveTarget(){
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;
    pn532_packetbuffer[2] = 0;

    if(writeCommand(pn532_packetbuffer, 3, 0, 0)){
        sprintf(buffer, "\rUnable to send request\n");
        uart_send(buffer);
        return 1;
    }

    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer), 30000);
    if(status < 0){
        sprintf(buffer, "\rDevice is not ready\n");
        uart_send(buffer);
        return 1;
    }

    if(pn532_packetbuffer[0] != 1){
        sprintf(buffer, "\rpacketbuffer[0] != 1\n");
        uart_send(buffer);
        return 1;
    }

    inListedTag = pn532_packetbuffer[1];

    return 0;
}