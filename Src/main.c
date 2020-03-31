#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <errors.h>
#include <uart_functions.h>
#include <pn532.h>
#include <config.h>
#include "stdio.h"



int main(){

    SystemInit();
    SysTick_Config(SystemCoreClock / 1000);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = { GPIO_Pin_13,
                                            GPIO_Speed_50MHz,
                                            GPIO_Mode_Out_PP };

    GPIO_Init(GPIOC, &GPIO_InitStructure);
    Delay(100);


    uartInit();
    char buffer[100];
    char serial[96];
    Delay(100);

    nfcBegin();
    Delay(100);



    uint32_t versiondata = getFirmwareVersion();
    if(!versiondata){
        sprintf(buffer, "\rUnable to find PN53x board\n");
        uart_send(buffer);
        while (1); // halt
    }

    sprintf(buffer, "\rFound PN532 chip!\n");
    uart_send(buffer);

    if(SAMConfig() != 0){
        sprintf(buffer, "\rUnable to setup SAMConfig\n");
        uart_send(buffer);
    }

    while(1){

        int success;

        uint8_t responseLength = 32;

        sprintf(buffer, "\rWaiting for NFC\n");
        uart_send(buffer);

        success = inListPassiveTarget();

        if(success == 0) {
            sprintf(buffer, "\rFound NFC device\n");
            uart_send(buffer);


            uint8_t selectApdu[] = {0x00, /* CLA */
                                    0xA4, /* INS */
                                    0x04, /* P1  */
                                    0x00, /* P2  */
                                    0x07, /* Length of AID  */
                                    0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                                    0x00  /* Le  */ };

            uint8_t response[32];

            success = inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);

            if (success) {
                sprintf(buffer, "\rResponse code: %2X\n", response);
                uart_send(buffer);

                GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
                Delay(500);
                GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
                Delay(500);

            } else {
                sprintf(buffer, "\rFailed sending SELECT AID\n");
                uart_send(buffer);
            }
        }else{
            sprintf(buffer, "\rDidn't find anything\n");
            uart_send(buffer);
        }

        Delay(1000);
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line){
  while(1);
}
#endif