//
// Created by macros on 30.03.20.
//

#ifndef PN532_H
#define PN532_H

void nfcBegin();
uint32_t getFirmwareVersion(void);
int SAMConfig();
int inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength);
int inListPassiveTarget();

#endif
