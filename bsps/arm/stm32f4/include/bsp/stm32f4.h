/**
 * @file
 * @ingroup stm32f4_reg
 * @brief Register definitions.
 */

/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4_H
#define LIBBSP_ARM_STM32F4_STM32F4_H

#include <bsp/utility.h>
#include <bspopts.h>

#define STM32F4_BASE 0x00

#ifdef STM32F4_FAMILY_F4XXXX

/**
 * @defgroup stm32f4_reg Register Defintions
 * @ingroup RTEMSBSPsARMSTM32F4
 * @brief Register Definitions
 * @{
 */

#define STM32F4_APB1_BASE (STM32F4_BASE + 0x40000000)
#define STM32F4_APB2_BASE (STM32F4_BASE + 0x40010000)
#define STM32F4_AHB1_BASE (STM32F4_BASE + 0x40020000)
#define STM32F4_AHB2_BASE (STM32F4_BASE + 0x50000000)

/**
 * @name STM32f4XXXX GPIO
 * @{
 */

#include <bsp/stm32f4xxxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40020000) + (i))

/** @} */

/**
 * @name STM32F4XXXX RCC
 * @{
 */

#include <bsp/stm32f4xxxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_AHB1_BASE + 0x3800))

/** @} */

/**
 * @name STM32F4XXXX FLASH
 * @{
 */

#include <bsp/stm32f4xxxx_flash.h>
#define STM32F4_FLASH ((volatile stm32f4_flash *) (STM32F4_BASE + 0x40023C00))

/** @} */

#include <bsp/stm32_i2c.h>

/**
 * @name STM32 I2C
 * @{
 */

#define STM32F4_I2C3 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005C00))
#define STM32F4_I2C2 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005800))
#define STM32F4_I2C1 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005400))

/** @} */

/**
 * @name STM32 USART
 * @{
 */

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011000))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))
#define STM32F4_USART_6 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011400))

/** @} */

/**
 * @name STM32f4XXXX PWR
 * @{
 */

#include <bsp/stm32f4xxxx_pwr.h>
#define STM32F4_PWR ((volatile stm32f4_pwr *) (STM32F4_APB1_BASE + 0x7000))

/** @} */

/**
 * @name STM32f4XXXX EXTI
 * @{
 */

#include <bsp/stm32f4xxxx_exti.h>
#define STM32F4_EXTI ((volatile stm32f4_exti *) (STM32F4_APB2_BASE + 0x3c00))

/** @} */

/**
 * @name STM32f4XXXX SYSCFG
 * @{
 */

#include <bsp/stm32f4xxxx_syscfg.h>
#define STM32F4_SYSCFG ((volatile stm32f4_syscfg *) (STM32F4_APB2_BASE + 0x3800))

/** @} */

/**
 * @name STM32f4XXXX FLASH
 * @{
 */

#include <bsp/stm32f4xxxx_flash.h>
#define STM32F4_FLASH ((volatile stm32f4_flash *) (STM32F4_AHB1_BASE + 0x3c00))

/** @} */

/**
 * @name STM32f4XXXX TIM
 * @{
 */

#include <bsp/stm32f4xxxx_tim.h>
#define STM32F4_TIM1 ((volatile stm32f4_tim *) (STM32F4_APB2_BASE + 0x0000))
#define STM32F4_TIM2 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x0000))
#define STM32F4_TIM3 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x0400))
#define STM32F4_TIM4 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x0800))
#define STM32F4_TIM5 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x0c00))
#define STM32F4_TIM6 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x1000))
#define STM32F4_TIM7 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x1400))
#define STM32F4_TIM8 ((volatile stm32f4_tim *) (STM32F4_APB2_BASE + 0x0400))
#define STM32F4_TIM9 ((volatile stm32f4_tim *) (STM32F4_APB2_BASE + 0x4000))
#define STM32F4_TIM10 ((volatile stm32f4_tim *) (STM32F4_APB2_BASE + 0x4400))
#define STM32F4_TIM11 ((volatile stm32f4_tim *) (STM32F4_APB2_BASE + 0x4800))
#define STM32F4_TIM12 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x1800))
#define STM32F4_TIM13 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x1c00))
#define STM32F4_TIM14 ((volatile stm32f4_tim *) (STM32F4_APB1_BASE + 0x2000))

/** @} */

/**
 * @name STM32f4XXXX ADC
 * @{
 */

#include <bsp/stm32f4xxxx_adc.h>
#define STM32F4_ADC1 ((volatile stm32f4_adc_chan *) (STM32F4_APB2_BASE + 0x2000))
#define STM32F4_ADC2 ((volatile stm32f4_adc_chan *) (STM32F4_APB2_BASE + 0x2100))
#define STM32F4_ADC3 ((volatile stm32f4_adc_chan *) (STM32F4_APB2_BASE + 0x2200))
#define STM32F4_ADC_COMMON ((volatile stm32f4_adc_com *) (STM32F4_APB2_BASE + 0x2300))

/** @} */

/**
 * @name STM32f4XXXX OTGFS
 * @{
 */

#include <bsp/stm32f4xxxx_otgfs.h>
#define STM32F4_OTGFS_BASE (STM32F4_AHB2_BASE + 0x0000)
#define STM32F4_OTGFS_CORE ((volatile stm32f4_otgfs *) (STM32F4_OTGFS_BASE + 0x000))
#define STM32F4_OTGFS_DEV ((volatile stm32f4_otgfs_dregs *) (STM32F4_OTGFS_BASE + 0x800))
#define STM32F4_OTGFS_INEP ((volatile stm32f4_otgfs_inepregs *) (STM32F4_OTGFS_BASE + 0x900))
#define STM32F4_OTGFS_OUTEP ((volatile stm32f4_otgfs_outepregs *) (STM32F4_OTGFS_BASE + 0xb00))
#define STM32F4_OTGFS_PWRCTL ((volatile stm32f4_otgfs_pwrctlregs *) (STM32F4_OTGFS_BASE + 0xe00))

#define STM32F4_OTGFS_FIFO_BASE (STM32F4_OTGFS_BASE + USB_FIFO_BASE)

/** @} */

#endif /* STM32F4_FAMILY_F4XXXX */

#ifdef STM32F4_FAMILY_F10XXX

/**
 * @name STM32F10 EXTI
 * @{
 */

#include <bsp/stm32f10xxx_exti.h>
#define STM32F4_EXTI ((volatile stm32f4_exti *) (STM32F4_BASE + 0x40010400))

/** @} */

/**
 * @name STM32F10XXX GPIO
 * @{
 */

#include <bsp/stm32f10xxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40010800 + i * 0x400))
#define STM32F4_AFIO ((volatile stm32f4_afio *) (STM32F4_BASE + 0x40010000))

/** @} */

/**
 * @name STM32F10XXX RCC
 * @{
 */

#include <bsp/stm32f10xxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40021000))

/** @} */

/**
 * @name STM32 I2C
 * @{
 */

#include <bsp/stm32_i2c.h>
#define STM32F4_I2C2 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005800))
#define STM32F4_I2C1 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005400))

/** @} */

/**
 * @name STM32 USART
 * @{
 */

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40013800))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))

/** @} */

/** @} */

#endif /* STM32F4_FAMILY_F10XXX */

#endif /* LIBBSP_ARM_STM32F4_STM32F4_H */
