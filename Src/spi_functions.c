//
// Created by macros on 23.03.20.
//

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include <spi_functions.h>


void spiInit(SPI_TypeDef *SPIx){
    SPI_InitTypeDef SPI_InitStructure ;
    GPIO_InitTypeDef GPIO_InitStructure ;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_StructInit (&GPIO_InitStructure);
    SPI_StructInit (&SPI_InitStructure);

    if(SPIx == SPI1){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);


//        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
//        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

    }else{
        //Todo setup for SPI2
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;

    SPI_Init(SPIx, & SPI_InitStructure);
    SPI_Cmd(SPIx, ENABLE);
}

void write(uint8_t data){
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    SPI_I2S_ReceiveData(SPI1);
}

uint8_t read(){
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, 0x00);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

    return SPI_I2S_ReceiveData(SPI1);
}

