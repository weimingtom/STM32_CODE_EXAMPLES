#include "main.h"


// API for arm cortex processor (not STM32) peripherals defined in stm32f4xx_hal_cortex.c

void HAL_MspInit(void) {
	// configure processor interrupt group priority
	// preemption priority and subpriority bits distributed in 4bits
	// if two interrupts with same preemption priority are pending, then interrupt with lower
	// sub-priority wins.
	//Default is NVIC_PRIORITY_GROUP_4 : 4 bits for preemption priority, 0 bits for subpriority
	// this is set already in HAL_Init()
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	// processor system exceptions
	// System Control Block (SCB) -> SHCSR has the enable bits
	// enable usage, memory and bus fault exceptions
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;
	// default priority for these  exceptions is 0 (highest priority),
	// subpriority is ignored as group priority 4 is already set
	// SysTick irq and priority is already set up in HAL_Init()
	// as examples
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	// configure MPU, FPU, sleep mode etc
}


void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim_base){
	// low level initialization
	if(htim_base->Instance == TIM2)  {
		// enable clock for TIM2 peripheral
		__HAL_RCC_TIM2_CLK_ENABLE();
		// enable clock to GPIOA so we can use PA5 as input channel
		//__HAL_RCC_GPIOA_CLK_ENABLE();  this was already done in GPIO_Init

		// PA5 can be used for TIM2_CH1
		// configure PA5 in AF1 mode
		GPIO_InitTypeDef config;
		config.Pin = GPIO_PIN_5;
		config.Mode = GPIO_MODE_AF_PP;
	    config.Pull = GPIO_NOPULL;
	    config.Speed = GPIO_SPEED_FREQ_LOW;
		config.Alternate = GPIO_AF1_TIM2;
		HAL_GPIO_Init(GPIOA, &config);

		// set irq priority
		HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
		// enable IRQ (also  define the handler TIM2_IRQHandler in it.c)
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
		}
	}


void HAL_UART_MspInit(UART_HandleTypeDef* huart){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART1)  {
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		}
	}



