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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_SYSCFG_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_SYSCFG_H

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

struct stm32f4_syscfg_s {
  uint32_t memrmp;  // Memory remap
#define STM32F4_SYSCFG_MEM_MODE(val)        BSP_FLD32(val, 0, 1)
#define STM32F4_SYSCFG_MEM_MODE_GET(reg)    BSP_FLD32GET(reg, 0, 1)
#define STM32F4_SYSCFG_MEM_MODE_SET(reg, val) BSP_FLD32SET(reg, val, 0, 1)
  uint32_t pmc;     // Peripheral mode configuration
#define STM32F4_SYSCFG_RMII_SEL             BSP_BIT32(23)
  uint32_t exticr[4]; // External interrupt configuration
#define STM32F4_SYSCFG_EXTI0_IDX        0
#define STM32F4_SYSCFG_EXTI0(val)       BSP_FLD32(val, 0, 3)
#define STM32F4_SYSCFG_EXTI0_GET(reg)   BSP_FLD32GET(reg, 0, 3)
#define STM32F4_SYSCFG_EXTI0_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define STM32F4_SYSCFG_EXTI1_IDX        0
#define STM32F4_SYSCFG_EXTI1(val)       BSP_FLD32(val, 4, 7)
#define STM32F4_SYSCFG_EXTI1_GET(reg)   BSP_FLD32GET(reg, 4, 7)
#define STM32F4_SYSCFG_EXTI1_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define STM32F4_SYSCFG_EXTI2_IDX        0
#define STM32F4_SYSCFG_EXTI2(val)       BSP_FLD32(val, 8, 11)
#define STM32F4_SYSCFG_EXTI2_GET(reg)   BSP_FLD32GET(reg, 8, 11)
#define STM32F4_SYSCFG_EXTI2_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define STM32F4_SYSCFG_EXTI3_IDX        0
#define STM32F4_SYSCFG_EXTI3(val)       BSP_FLD32(val, 12, 15)
#define STM32F4_SYSCFG_EXTI3_GET(reg)   BSP_FLD32GET(reg, 12, 15)
#define STM32F4_SYSCFG_EXTI3_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
#define STM32F4_SYSCFG_EXTI4_IDX        1
#define STM32F4_SYSCFG_EXTI4(val)       BSP_FLD32(val, 0, 3)
#define STM32F4_SYSCFG_EXTI4_GET(reg)   BSP_FLD32GET(reg, 0, 3)
#define STM32F4_SYSCFG_EXTI4_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define STM32F4_SYSCFG_EXTI5_IDX        1
#define STM32F4_SYSCFG_EXTI5(val)       BSP_FLD32(val, 4, 7)
#define STM32F4_SYSCFG_EXTI5_GET(reg)   BSP_FLD32GET(reg, 4, 7)
#define STM32F4_SYSCFG_EXTI5_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define STM32F4_SYSCFG_EXTI6_IDX        1
#define STM32F4_SYSCFG_EXTI6(val)       BSP_FLD32(val, 8, 11)
#define STM32F4_SYSCFG_EXTI6_GET(reg)   BSP_FLD32GET(reg, 8, 11)
#define STM32F4_SYSCFG_EXTI6_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define STM32F4_SYSCFG_EXTI7_IDX        1
#define STM32F4_SYSCFG_EXTI7(val)       BSP_FLD32(val, 12, 15)
#define STM32F4_SYSCFG_EXTI7_GET(reg)   BSP_FLD32GET(reg, 12, 15)
#define STM32F4_SYSCFG_EXTI7_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
#define STM32F4_SYSCFG_EXTI8_IDX        2
#define STM32F4_SYSCFG_EXTI8(val)       BSP_FLD32(val, 0, 3)
#define STM32F4_SYSCFG_EXTI8_GET(reg)   BSP_FLD32GET(reg, 0, 3)
#define STM32F4_SYSCFG_EXTI8_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define STM32F4_SYSCFG_EXTI9_IDX        2
#define STM32F4_SYSCFG_EXTI9(val)       BSP_FLD32(val, 4, 7)
#define STM32F4_SYSCFG_EXTI9_GET(reg)   BSP_FLD32GET(reg, 4, 7)
#define STM32F4_SYSCFG_EXTI9_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define STM32F4_SYSCFG_EXTI10_IDX       2
#define STM32F4_SYSCFG_EXTI10(val)        BSP_FLD32(val, 8, 11)
#define STM32F4_SYSCFG_EXTI10_GET(reg)    BSP_FLD32GET(reg, 8, 11)
#define STM32F4_SYSCFG_EXTI10_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define STM32F4_SYSCFG_EXTI11_IDX       2
#define STM32F4_SYSCFG_EXTI11(val)        BSP_FLD32(val, 12, 15)
#define STM32F4_SYSCFG_EXTI11_GET(reg)    BSP_FLD32GET(reg, 12, 15)
#define STM32F4_SYSCFG_EXTI11_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
#define STM32F4_SYSCFG_EXTI12_IDX       3
#define STM32F4_SYSCFG_EXTI12(val)        BSP_FLD32(val, 0, 3)
#define STM32F4_SYSCFG_EXTI12_GET(reg)    BSP_FLD32GET(reg, 0, 3)
#define STM32F4_SYSCFG_EXTI12_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define STM32F4_SYSCFG_EXTI13_IDX       3
#define STM32F4_SYSCFG_EXTI13(val)        BSP_FLD32(val, 4, 7)
#define STM32F4_SYSCFG_EXTI13_GET(reg)    BSP_FLD32GET(reg, 4, 7)
#define STM32F4_SYSCFG_EXTI13_SET(reg, val) BSP_FLD32SET(reg, val, 4, 7)
#define STM32F4_SYSCFG_EXTI14_IDX       3
#define STM32F4_SYSCFG_EXTI14(val)        BSP_FLD32(val, 8, 11)
#define STM32F4_SYSCFG_EXTI14_GET(reg)    BSP_FLD32GET(reg, 8, 11)
#define STM32F4_SYSCFG_EXTI14_SET(reg, val) BSP_FLD32SET(reg, val, 8, 11)
#define STM32F4_SYSCFG_EXTI15_IDX       3
#define STM32F4_SYSCFG_EXTI15(val)        BSP_FLD32(val, 12, 15)
#define STM32F4_SYSCFG_EXTI15_GET(reg)    BSP_FLD32GET(reg, 12, 15)
#define STM32F4_SYSCFG_EXTI15_SET(reg, val) BSP_FLD32SET(reg, val, 12, 15)
  uint32_t cmpcr;   // Compensation cell control register
#define STM32F4_SYSCFG_CMPCR_READY        BSP_BIT32(8)
#define STM32F4_SYSCFG_CMPCR_PD           BSP_BIT32(0)
} __attribute__ ((packed));
typedef struct stm32f4_syscfg_s stm32f4_syscfg;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_SYSCFG_H */
