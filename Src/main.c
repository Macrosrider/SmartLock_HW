#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <errors.h>
#include <uart_functions.h>
#include <pn532_emulatetag.h>
#include <config.h>
#include "stdio.h"

////////////////////////////////////////////////////////////////////////////////////
/*Pinout:
 * UART: RX->PA9, TX->PA10
 * PN532: NSS->PA4, MOSI->PA7 , MISO-> PA6, SCKL->PA5
 */
///////////////////////////////////////////////////////////////////////////////////


char buffer[100];
uint8_t uid[3] = { 0x12, 0x34, 0x56 };

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
    Delay(100);

    tag_setUid(uid);
    tag_init();
    Delay(100);

    uint32_t versiondata = pn532_getFirmwareVersion();
    if(!versiondata){
        sprintf(buffer, "\rUnable to find PN53x board\n");
        uart_send(buffer);
        while (1); // halt
    }

    sprintf(buffer, "\rFound PN532 chip!\n");
    uart_send(buffer);

    while(1){
        tag_emulate(0);
        if(writeOccured()){
            uint8_t* tag_buf;
            uint16_t length;
            tag_getContent(&tag_buf, &length);
            for(int i = 0; i < length; i++) {
                sprintf(buffer, "%2X ", tag_buf[i]);
                uart_send(buffer);
            }
            sprintf(buffer, "\n");
            uart_send(buffer);
        }
        Delay(100);
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line){
  while(1);
}
#endif