/*
 * main.h
 *
 *  Created on: 23 февр. 2019 г.
 *      Author: admin
 */

#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include "stm32f4xx.h"

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_M      4
#define PLL_N      168

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      4

static void clockConfig(void);

static void clockConfig(void)
{
	RCC->CR |= (uint32_t)RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){};

    /* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK / 1*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* PCLK1 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0){}

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){};
}

//-------------------------------
// FUNCTIONS PROTOTYPES
//-------------------------------
void gpio_init(void);
void SPI_Init(void);
void SPI_Write(uint8_t data);

//-------------------------------
// FUNCTIONS
//-------------------------------

void gpio_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE4_0;
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4);
}


void SPI_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODE5_1 | GPIO_MODER_MODE3_1;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED3;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_0;

	SPI1->CR1 &= ~(SPI_CR1_CPHA);
	SPI1->CR1 &= ~ SPI_CR1_CPOL;
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0; //168
	//SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0; //16
	SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);
	SPI1->CR1 |= SPI_CR1_SSI;
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 &= ~(SPI_CR1_RXONLY);
	SPI1->CR1 &= ~(SPI_CR1_DFF);
	SPI1->CR1 &= ~(SPI_CR1_CRCNEXT);
	SPI1->CR1 &= ~(SPI_CR1_CRCEN);
	SPI1->CR1 &= ~(SPI_CR1_BIDIOE);
	SPI1->CR1 &= ~(SPI_CR1_BIDIMODE);
}


void SPI_Write(uint8_t data)
{
   	if(!(SPI1->CR1 & SPI_CR1_SPE))
	{
	    SPI1->CR1 |= SPI_CR1_SPE;
	}
	GPIOD->BSRR |= GPIO_BSRR_BR4;

	SPI1->DR = data;
	while(!(SPI1->SR & SPI_SR_TXE));

	while(SPI1->SR & SPI_SR_BSY);

	GPIOD->BSRR |= GPIO_BSRR_BS4;
}



#endif /* APP_MAIN_H_ */
