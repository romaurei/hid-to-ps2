/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD21 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define PA04		GPIO(GPIO_PORTA, 4)  //SERCOM 0 Pad 0 > A3 in Metro Express
#define PA05		GPIO(GPIO_PORTA, 5)  //SERCOM 0 Pad 1 > A4 in Metro Express
#define PA24		GPIO(GPIO_PORTA, 24)  //D-
#define PA25		GPIO(GPIO_PORTA, 25)  //D+
#define LED0		GPIO(GPIO_PORTA, 17)
#define USB_HOST_EN GPIO(GPIO_PORTA, 28)  //USB_HOST_EN Line in Metro Express

#endif // ATMEL_START_PINS_H_INCLUDED
