//
// Created by macros on 20.03.20.
//
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>

static const char* hexdigits = "0123456789ABCDEF";

void uartInit(){

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStructure;

    USART_StructInit(&USART_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void uart_send(char* buffer){
    while (*buffer)
    {
        USART_SendData(USART1, *buffer++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
}

void data2hex(const void* data, uint32_t len, char* str)
{
    const uint8_t* cdata = (uint8_t*)data;
    for (uint32_t i = 0; i < len; i++) {
        str[i * 2] = hexdigits[(cdata[i] >> 4) & 0xF];
        str[i * 2 + 1] = hexdigits[cdata[i] & 0xF];
    }
    str[len * 2] = 0;
}

