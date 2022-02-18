# GPIO Interrupt callback dispatch

## Index

1. Introduction to interrupts
1. Functionnal overview of the code
1. Requirements and notes
1. Bibliography

## Introduction to interrupts

### What is an interrupt

In digital computers, an interrupt (sometimes referred to as a trap) is a request for the processor to interrupt currently executing code (when permitted), so that the event can be processed in a timely manner. If the request is accepted, the processor will suspend its current activities, save its state, and execute a function called an interrupt handler (or an interrupt service routine, ISR) to deal with the event. This interruption is often temporary, allowing the software to resume normal activities after the interrupt handler finishes, although the interrupt could instead indicate a fatal error.[^1]

### Hardware interrupts

A hardware interrupt is a condition related to the state of the hardware that may be signaled by an external hardware device, e.g., an interrupt request (IRQ) line on a PC, or detected by devices embedded in processor logic (e.g., the CPU timer in IBM System/370), to communicate that the device needs attention from the operating system (OS) or, if there is no OS, from the "bare-metal" program running on the CPU. Such external devices may be part of the computer (e.g., disk controller) or they may be external peripherals. For example, pressing a keyboard key or moving a mouse plugged into a PS/2 port triggers hardware interrupts that cause the processor to read the keystroke or mouse position.

Hardware interrupts can arrive asynchronously with respect to the processor clock, and at any time during instruction execution. Consequently, all incoming hardware interrupt signals are conditioned by synchronizing them to the processor clock, and acted upon only at instruction execution boundaries.[^1]

### How does a hardware interrupt works in an STM32 microcontroller

There are 16 external interrupt lines with separate interrupt vector addresses that are connected with GPIO pins. Thus there are 16 multiplexers connected to the NVIC and are named as External Interrupt/Event Controllers, EXTI0, EXTI1, etc. up to EXTI15. GPIO pins of the same order are grouped together and connected to an EXTI channel. For instance EXTI1 is connected to PA1, PB1, etc. and not like PA0, PA1, etc. This is why at any given instance we can have an external interrupt in only one of connected GPIO pins of that EXTI mux. For example when we need to use EXTI2, we can use either PA2, PB2, PC2, etc. but not PA2, PB2, etc. simultaneously. Thus an entire GPIO port or port pins from different GPIO ports can be configured as external interrupts. Interrupts in all pins is an unnecessary stuff. We can also decide when to sense an interrupt – on rising/falling or both edges. All these settings are separately stored, allowing high flexibility.[^2]

### HAL library interrupts and timers

#### Interrupts

How to use the interrups with the HAL library[^3]:

1. Enable the GPIO AHB clock using the following function : __HAL_RCC_GPIOx_CLK_ENABLE().
2. Configure the GPIO pin(s) using HAL_GPIO_Init().
- Configure the IO mode using "Mode" member from GPIO_InitTypeDef structure
- Activate Pull-up, Pull-down resistor using "Pull" member from GPIO_InitTypeDef structure.
- In case of Output or alternate function mode selection: the speed is configured through "Speed"
member from GPIO_InitTypeDef structure.
- In alternate mode is selection, the alternate function connected to the IO is configured through
"Alternate" member from GPIO_InitTypeDef structure.
- Analog mode is required when a pin is to be used as ADC channel or DAC output.
- In case of external interrupt/event selection the "Mode" member from GPIO_InitTypeDef structure
select the type (interrupt or event) and the corresponding trigger event (rising or falling or both).
3. In case of external interrupt/event mode selection, configure NVIC IRQ priority mapped to the EXTI line
using HAL_NVIC_SetPriority() and enable it using HAL_NVIC_EnableIRQ().
4. HAL_GPIO_DeInit allows to set register values to their reset value. It's also recommended to use it to
unconfigure pin which was used as an external interrupt or in event mode. That's the only way to reset
corresponding bit in EXTI & SYSCFG registers.

#### Timers

How to use the timers with the HAL library[^3]:

1. Initialize the TIM low level resources by implementing the following functions depending from feature used :
- Time Base : HAL_TIM_Base_MspInit()
- Input Capture : HAL_TIM_IC_MspInit()
- Output Compare : HAL_TIM_OC_MspInit()
- PWM generation : HAL_TIM_PWM_MspInit()
- One-pulse mode output : HAL_TIM_OnePulse_MspInit()
- Encoder mode output : HAL_TIM_Encoder_MspInit()
2. Initialize the TIM low level resources :
a. Enable the TIM interface clock using __HAL_RCC_TIMx_CLK_ENABLE();
b. TIM pins configuration
- Enable the clock for the TIM GPIOs using the following function: __HAL_RCC_GPIOx_CLK_ENABLE();
- Configure these TIM pins in Alternate function mode using HAL_GPIO_Init();
3. The external Clock can be configured, if needed (the default clock is the internal clock from the APBx), using
the following function: HAL_TIM_ConfigClockSource, the clock configuration should be done before any
start function.
4. Configure the TIM in the desired functioning mode using one of the Initialization function of this driver:
- HAL_TIM_Base_Init: to use the Timer to generate a simple time base
- HAL_TIM_OC_Init and HAL_TIM_OC_ConfigChannel: to use the Timer to generate an Output
Compare signal.
- HAL_TIM_PWM_Init and HAL_TIM_PWM_ConfigChannel: to use the Timer to generate a PWM signal.
- HAL_TIM_IC_Init and HAL_TIM_IC_ConfigChannel: to use the Timer to measure an external signal.
- HAL_TIM_OnePulse_Init and HAL_TIM_OnePulse_ConfigChannel: to use the Timer in One Pulse
Mode.
- HAL_TIM_Encoder_Init: to use the Timer Encoder Interface.
5. Activate the TIM peripheral using one of the start functions depending from the feature used:
- Time Base : HAL_TIM_Base_Start(), HAL_TIM_Base_Start_DMA(), HAL_TIM_Base_Start_IT()
- Input Capture : HAL_TIM_IC_Start(), HAL_TIM_IC_Start_DMA(), HAL_TIM_IC_Start_IT()
- Output Compare : HAL_TIM_OC_Start(), HAL_TIM_OC_Start_DMA(), HAL_TIM_OC_Start_IT()
- PWM generation : HAL_TIM_PWM_Start(), HAL_TIM_PWM_Start_DMA(), HAL_TIM_PWM_Start_IT()
- One-pulse mode output : HAL_TIM_OnePulse_Start(), HAL_TIM_OnePulse_Start_IT()
- Encoder mode output : HAL_TIM_Encoder_Start(), HAL_TIM_Encoder_Start_DMA(),
HAL_TIM_Encoder_Start_IT().
6. The DMA Burst is managed with the two following functions: HAL_TIM_DMABurst_WriteStart()
HAL_TIM_DMABurst_ReadStart()

## Functionnal overview of the code

### Where to find the code

The whole project was added to this git repository is a complete project initialized from STM32CUBEIDE. I used this in order to test the compilation of the library, but they are only two files that are important to respond to the assignment. They are **GPIO_interrupt_callback_dispatch.c** and **GPIO_interrupt_callback_dispatch.h** and can be found in the **Library** folder.

### How to use the library

There are 3 importants steps to follow to use the library:

1. Initialize the GPIO interrupts:
In order to initialize the GPIO interrupts, I created the following function:
```c
void initGpioInterrupt(uint16_t GPIOPin, uint32_t triggerMode, uint32_t PreemptPriority, uint32_t SubPriority);
```
This function allows the initialization of interrupts on a specific pin with a configurable triggerMode and priority.

2. Initialize the debounce filter:
In order to initialize the debounce filter, I created the following function:
```c
int debounceFilterTimerInit(uint16_t period_us);
```
This function allows us to configure the period of the debounce filter in microseconds.

3. Add your callback function:
In order to add you callback function, I created the following function:
```c
int createCallback(uint16_t GPIOPin, void (*callback)(void));
```
This function allows to add a previously defined callback function to the specified GPIO pin.

### How is built the library

Basically what our code does is that it creates an interupt thanks to the HAL library, and when the interrupt occurs, it launches a timer that will elapse after a previously set period and then execute the callback function.

To handle the callback functions, I created an array of 16 function pointers that contains the function pointes of their relative callback function. We can access to the callback function by using the GPIO pin number as array index.

*Exampe:*
```c
//the array declaration
static void (*callback_func[NUM_GPIOS])(void);
//To access the gpio pin 15
callback_func[15];
```

I also added a bool array (`static bool enabled_callbacks[NUM_GPIOS];`) that contains all the GPIO pins that have an enabled callback function, this avoids calling a callback function if the interupt was enabled but not the callback function.

So to enable a callback function, the only thing that we do is to copy the function pointer to our `callback_func` array and set the callback as enabled to the `enabled_callbacks` array.

When an interruption arrives, the `EXTIN_M_IRQHandler` function is called (N_M can be 0_1 for pins 0 and 1, 2_3 for pins 2 and 3 and 4_15 for pins 4 to 15). This function has the task of launching the timer with the previously set period and disabling the interrupt.

Once the timer elapsed, we execute the callback function and re enable the interrupt.

## Requirements and notes

#### Requirements:
- Install STM32CUBEIDE
- Start a project with the STM32F030C6Tx microcontroller selected
- Enable de HAL library

#### Notes
- The targeted device is an STM32F030C6Tx
- The code was developed using the HAL libraries developed by ST and available in STM32CUBEIDE
- I did not had the hardware to effectively test this library so it is something to be done before considering the library as functionnal

## Bibliography

[^1]: https://en.wikipedia.org/wiki/Interrupt
[^2]: https://embedded-lab.com/blog/stm32-external-interrupt/
[^3]: https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwiW4Y3Uxof2AhVFz4UKHXP9DdEQFnoECAkQAQ&url=https%3A%2F%2Fwww.st.com%2Fresource%2Fen%2Fuser_manual%2Fdm00122015-description-of-stm32f0-hal-and-lowlayer-drivers-stmicroelectronics.pdf&usg=AOvVaw2XcS2yGTb4AukxdzNfwFxu
