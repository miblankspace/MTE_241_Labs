/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!

/**
  * @brief  The application entry point.
  * @retval int
  */

uint32_t* stackptr;
extern void runFirstThread(void);

#define RUN_FIRST_THREAD 0x3

int main(void)
{

  /* MCU Configuration: Don't change this or the whole chip won't work!*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* MCU Configuration is now complete. Start writing your code below this line */
  uint32_t* MSP_INIT_VAL = *(uint32_t**)0x0;
  printf("MSP Init is: %p\r\n",MSP_INIT_VAL); //note the %p to print a pointer. It will be in hex

  uint32_t PSP_val = (uint32_t)MSP_INIT_VAL - 0x400;
  __set_PSP(PSP_val);
  __set_CONTROL(0x2);

  print_continuously();
  jumpAssembly((void*)print_continuously);

  *(--stackptr) = 1<<24; //A magic number, this is xPSR
  *(--stackptr) = (uint32_t)print_continuously; //the function name
  *(--stackptr) = 0xA; //An arbitrary number

  for(i=0;i<15;i++)
  {
	  *(--stackptr)=0xA;
  }
}

/* Infinite loop */
/* USER CODE BEGIN WHILE */
void print_continuously()
{
	  while (1)
	  {
		printf("Hello, PC!\r\n");
	  }
}

void jumpAssembly(void* fcn)
{
	__asm("MOV PC, R0");
}

void print_chill()
{
	__asm("SVC #0");
}

void print_stressed()
{
	__asm("SVC #1");
}

void SVC_Handler_Main( unsigned int *svc_args )
{
	unsigned int svc_number;
	/*
	* Stack contains:
	* r0, r1, r2, r3, r12, r14, the return address and xPSR
	* First argument (r0) is svc_args[0]
	*/
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
		case 0:
			printf("Engineering is so cool and so chill. I love engineering.\r\n");
			break;
		case 1:
			printf("I AM CRASHING OUT! GET ME OUT.\r\n");
			break;
		case RUN_FIRST_THREAD:
			__set_PSP(stackptr);
			runFirstThread();
			break;
		case 17: //17 is sort of arbitrarily chosen
			printf("Success!\r\n");
			break;
		default: /* unknown SVC */
			break;
	}
}
/* USER CODE END 3 */

