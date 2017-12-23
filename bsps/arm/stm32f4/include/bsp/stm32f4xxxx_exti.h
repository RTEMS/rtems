/*
 * Copyright (c) 2013 Chris Nott.  All rights reserved.
 *
 *  Virtual Logic
 *  21-25 King St.
 *  Rockdale NSW 2216
 *  Australia
 *  <rtems@vl.com.au>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_EXTI_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_EXTI_H

#include <bsp/utility.h>

#define EXTI_PORTA 0
#define EXTI_PORTB 1
#define EXTI_PORTC 2
#define EXTI_PORTD 3
#define EXTI_PORTE 4
#define EXTI_PORTF 5
#define EXTI_PORTG 6
#define EXTI_PORTH 7
#define EXTI_PORTI 8

#define STM32F4_EXTI_LINE22							BSP_BIT32(21)
#define STM32F4_EXTI_LINE21							BSP_BIT32(21)
#define STM32F4_EXTI_LINE20							BSP_BIT32(20)
#define STM32F4_EXTI_LINE19							BSP_BIT32(19)
#define STM32F4_EXTI_LINE18							BSP_BIT32(18)
#define STM32F4_EXTI_LINE17							BSP_BIT32(17)
#define STM32F4_EXTI_LINE16							BSP_BIT32(16)
#define STM32F4_EXTI_LINE15							BSP_BIT32(15)
#define STM32F4_EXTI_LINE14							BSP_BIT32(14)
#define STM32F4_EXTI_LINE13							BSP_BIT32(13)
#define STM32F4_EXTI_LINE12							BSP_BIT32(12)
#define STM32F4_EXTI_LINE11							BSP_BIT32(11)
#define STM32F4_EXTI_LINE10							BSP_BIT32(10)
#define STM32F4_EXTI_LINE9							BSP_BIT32(9)
#define STM32F4_EXTI_LINE8							BSP_BIT32(8)
#define STM32F4_EXTI_LINE7							BSP_BIT32(7)
#define STM32F4_EXTI_LINE6							BSP_BIT32(6)
#define STM32F4_EXTI_LINE5							BSP_BIT32(5)
#define STM32F4_EXTI_LINE4							BSP_BIT32(4)
#define STM32F4_EXTI_LINE3							BSP_BIT32(3)
#define STM32F4_EXTI_LINE2							BSP_BIT32(2)
#define STM32F4_EXTI_LINE1							BSP_BIT32(1)
#define STM32F4_EXTI_LINE0							BSP_BIT32(0)

struct stm32f4_exti_s {
	uint32_t imr;		// Interrupt mask
	uint32_t emr;		// Event mask
	uint32_t rtsr;	// Rising trigger selection
	uint32_t ftsr;	// Falling trigger selection
	uint32_t swier;	// Software interrupt event
	uint32_t pr;		// Pending
} __attribute__ ((packed));
typedef struct stm32f4_exti_s stm32f4_exti;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_EXTI_H */
