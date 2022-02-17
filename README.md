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

### HAL library interrupts

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

## Functionnal overview of the code

### Where to find the code

### How is built the library and how to use it

## Requirements and notes

- The targeted device is an STM32F030C6Tx
- The code was developed using the HAL libraries developed by ST and available in STM32CUBEIDE

## Bibliography

[^1]: https://en.wikipedia.org/wiki/Interrupt
[^2]: https://embedded-lab.com/blog/stm32-external-interrupt/
[^3]: https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwiW4Y3Uxof2AhVFz4UKHXP9DdEQFnoECAkQAQ&url=https%3A%2F%2Fwww.st.com%2Fresource%2Fen%2Fuser_manual%2Fdm00122015-description-of-stm32f0-hal-and-lowlayer-drivers-stmicroelectronics.pdf&usg=AOvVaw2XcS2yGTb4AukxdzNfwFxu
