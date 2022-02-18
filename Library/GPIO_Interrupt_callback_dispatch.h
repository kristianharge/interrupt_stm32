/*
 * GPIO_Interrupt_callback_dispatch.h
 *
 *  Created on: Feb 17, 2022
 *      Author: ir-coaster-soft
 */

#ifndef GPIO_INTERRUPT_CALLBACK_DISPATCH_H_
#define GPIO_INTERRUPT_CALLBACK_DISPATCH_H_

#define NUM_GPIOS				16

typedef enum {
	TRIGGER_MODE_RISING=0,
	TRIGGER_MODE_FALLING=1,
	TRIGGER_MODE_BOTH=2
} TRIGGER_MODES;

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
void initGpioInterrupt(uint16_t GPIOPin, uint32_t triggerMode, uint32_t PreemptPriority, uint32_t SubPriority);

//~ Function : debounceFilterTimerInit
//~ ----------------------------
//~ Initialize the debounce filter timer
//~
//~ input : uint16_t period_us; the period of the debounce filter
//~
//~ output : int -1 if error, 1 if success
int debounceFilterTimerInit(uint16_t period_us);

//~ Function : createCallback
//~ ----------------------------
//~ Create the callback
//~
//~ input : uint16_t GPIOPin; the gpio pin, void (*callback)(void); the callback fonction
//~
//~ output : int -1 if error, 1 if success
int createCallback(uint16_t GPIOPin, void (*callback)(void));

#endif /* GPIO_INTERRUPT_CALLBACK_DISPATCH_H_ */
