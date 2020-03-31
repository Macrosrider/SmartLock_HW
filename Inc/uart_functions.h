//
// Created by macros on 20.03.20.
//



#ifndef UART_FUNCTIONS_H
#define UART_FUNCTIONS_H

void uartInit();

void uart_send(char* buffer);

void data2hex(const void* data, uint32_t len, char* str);

#endif
