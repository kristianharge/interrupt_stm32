/*
 * GPIO_Interrupt_callback_dispatch.c
 *
 *  Created on: Feb 15, 2022
 *      Author: ir-coaster-soft
 */

#include <stdbool.h>
#include "stm32f0xx_hal.h"

#include "GPIO_Interrupt_callback_dispatch.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//////////GPIO interrupt callback dispatch private variables////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//an array containing the callback functions, the index is the gpio number
static void (*callback_func[NUM_GPIOS])(void);
//an array containing the enabled callbacks, the index is the gpio number
static bool enabled_callbacks[NUM_GPIOS];
//an array containing the gpiopins, the index is the gpio number
static uint16_t gpiopins[NUM_GPIOS] = {
 GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
 GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};
//an array containing the trigger mode of the gpios. 0 -> rising edge, 1 -> falling edge, 2 -> both, the index is the gpio number
static uint8_t  trigger_mode[NUM_GPIOS];
//the timer hanle
static TIM_HandleTypeDef htim1;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//////////GPIO interrupt callback dispatch private functions////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//~ Function : EXTI4_15_IRQHandler
//~ ----------------------------
//~ The interrupt handler for gpios 4 to 15
//~
//~ input : void
//~
//~ output : void
void EXTI4_15_IRQHandler(void){
	HAL_TIM_Base_Start_IT(&htim1);
}// end function EXTI4_15_IRQHandler

//~ Function : EXTI2_3_IRQHandler
//~ ----------------------------
//~ The interrupt handler for gpios 2 and 3
//~
//~ input : void
//~
//~ output : void
void EXTI2_3_IRQHandler(void){
	HAL_TIM_Base_Start_IT(&htim1);
}// end function EXTI2_3_IRQHandler

//~ Function : EXTI0_1_IRQHandler
//~ ----------------------------
//~ The interrupt handler for gpios 0 and 1
//~
//~ input : void
//~
//~ output : void
void EXTI0_1_IRQHandler(void){
	HAL_TIM_Base_Start_IT(&htim1);
}// end function EXTI0_1_IRQHandler

//~ Function : HAL_TIM_PeriodElapsedCallback
//~ ----------------------------
//~ The callback of the timer used for the debounce filter
//~
//~ input : void
//~
//~ output : void
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);

	for(int i = 2; i < NUM_GPIOS; i++){
		if(enabled_callbacks[i]){
			if(__HAL_GPIO_EXTI_GET_FLAG(gpiopins[i])){
				(*callback_func)();
				HAL_GPIO_EXTI_IRQHandler(gpiopins[i]);
			}
		}
	}
}// end function HAL_TIM_PeriodElapsedCallback

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//////////GPIO interrupt callback dispatch public functions/////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//~ Function : init_GPIO_interrupt
//~ ----------------------------
//~ Initialize the gpio interrupts
//~
//~ input : uint16_t pin; is the pin containing the interruption, uint32_t triggerMode; the edge on the interrupt,
//~ 	uint32_t PreemptPriority; the priority of the interruption from 0 to 15,
//~ 	uint32_t SubPriority: the subpriority of the interruption from 0 to 15.
//~
//~ output : void
void initGpioInterrupt(uint16_t GPIOPin, uint32_t triggerMode, uint32_t PreemptPriority, uint32_t SubPriority){
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIOPin;
	GPIO_InitStruct.Mode = triggerMode;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	//Enable the interruption
	if(GPIOPin > 3){
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, PreemptPriority, SubPriority);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	}
	else if(GPIOPin > 1){
		HAL_NVIC_SetPriority(EXTI2_3_IRQn, PreemptPriority, SubPriority);
		HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
	}
	else {
		HAL_NVIC_SetPriority(EXTI0_1_IRQn, PreemptPriority, SubPriority);
		HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
	}// end enable the interruption

	//set the trigger mode
	if(triggerMode == GPIO_MODE_IT_RISING){
		trigger_mode[GPIOPin] = TRIGGER_MODE_RISING;
	}
	else if(triggerMode == GPIO_MODE_IT_FALLING){
		trigger_mode[GPIOPin] = TRIGGER_MODE_FALLING;
	}
	else if(triggerMode == GPIO_MODE_IT_RISING_FALLING){
		trigger_mode[GPIOPin] = TRIGGER_MODE_BOTH;
	}// end set the trigger mode
}// end function init_GPIO_interrupt

//~ Function : debounceFilterTimerInit
//~ ----------------------------
//~ Initialize the debounce filter timer
//~
//~ input : uint32_t period_us; the period of the debounce filter
//~
//~ output : int -1 if error, 1 if success
int debounceFilterTimerInit(uint32_t period_us){
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	//we multiply the period with the frequency of the clock. Here 8 MHz
	htim1.Init.RepetitionCounter = period_us*8 - 1;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		return -1;
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		return -1;
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		return -1;
	}
	return 1;
}// end of function debounceFilterTimerInit

//~ Function : createCallback
//~ ----------------------------
//~ Create the callback
//~
//~ input : uint16_t GPIOPin; the gpio pin, void (*callback)(void); the callback fonction
//~
//~ output : int -1 if error, 1 if success
int createCallback(uint16_t GPIOPin, void (*callback)(void)){
	if(GPIOPin >= NUM_GPIOS || GPIOPin < 0){
		return -1;
	}

	if(callback == NULL){
		return -1;
	}

	callback_func[GPIOPin] = callback;
	enabled_callbacks[GPIOPin] = 1;

	return 1;
}// end function createCallback

