/**
 *@file main.c
 *@brief Register Level Programming Simple Blink Project
 *@include main.c
 **/

#include <stdint.h>
#include "peripherals.h"

#define MODER 2
#define pin5 5
#define BUFFER_LENGTH 8

/**
 * @brief Struct Pointer for RCC Peripherals assigned with fixed address specified in reference manual
 **/
RCC_t   * const RCC     = (RCC_t    *)  0x40023800; 
/**
 * @brief Struct Pointer for GPIOA Peripherals assigned with fixed address specified in reference manual
 **/
GPIOx_t * const GPIOA   = (GPIOx_t  *)  0x40020000;
GPIOx_t * const GPIOB   = (GPIOx_t  *)  0x40020400;

SPIx_t * const SPI1     = (SPIx_t   *)    0x40013000;
SPIx_t * const SPI2     = (SPIx_t   *)    0x40003800;

/**
 *@brief This is a simple delay function implementation, that waits for <time>ms.
 *
 *In this implementation the inner for loop, cycles for 1600 CLK Cycles which results in around 1ms delay, depending on the parameter <time>, the amount of ms delay can be adjusted.
 *
 *@param[in] time | number of ms the processor should wait
 **/
void wait_ms(int time){
    for(int i = 0; i < time; i++){
        for(int j = 0; j < 1600; j++);
    }
}

uint8_t txBuffer[BUFFER_LENGTH] = {0x06, 0x07, 0x04, 0x0c, 0x04, 0x05, 0x06, 0x07};
uint8_t rxBuffer[BUFFER_LENGTH];

/**
 *@brief Main entry point for blinking project.
 *
 *GPIOA Peripherals are configured to OUTPUT, with LED connected to PA5 being toggled every 100ms
 **/


int main(void){

    //Enable Clock to GPIOA Peripheral
    RCC->RCC_AHB1ENR |= (1 << 0); 
    RCC->RCC_AHB1ENR |= (1 << 1); 
    
    SPI1_Init(RCC, SPI1, GPIOA);
    SPI2_Init(RCC, SPI2, GPIOB);
    SPI_TransmitReceive(SPI1, SPI2, txBuffer, rxBuffer, BUFFER_LENGTH);

    int success = 1;
    for(int i = 0; i < BUFFER_LENGTH; i++) {
        if(txBuffer[i] != rxBuffer[i]) {
            success = 0; 
            break;
        }
    }

    GPIOA->GPIOx_MODER &= ~(3 << (5 * MODER));
    GPIOA->GPIOx_MODER |= (1 << (5 * MODER));
    if(success) {
        for(;;){
            GPIOA->GPIOx_ODR ^= (1 << pin5);
            wait_ms(1000);
        }
    } else {
        for(;;){
            GPIOA->GPIOx_ODR ^= (1 << pin5);
            wait_ms(100);
        }
    }
}
