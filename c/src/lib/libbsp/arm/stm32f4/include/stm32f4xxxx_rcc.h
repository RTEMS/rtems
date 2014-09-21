/**
 * @file
 * @ingroup stm32f4xxxx_rcc
 * @brief STM32F4XXXX RCC support.
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H

#include <bsp/utility.h>

/**
 * @defgroup stm32f4xxxx_rcc STM32F4XXXX RCC Support
 * @ingroup stm32f4_rcc
 * @brief STM32F4XXXX RCC Support
 * @{
 */

typedef struct {
  uint32_t cr;
  uint32_t pllcfgr;
  uint32_t cfgr;
  uint32_t cir;
  uint32_t ahbrstr[ 3 ];
  uint32_t reserved_1c;
  uint32_t apbrstr[ 2 ];
  uint32_t reserved_28[ 2 ];
  uint32_t ahbenr[ 3 ];
  uint32_t reserved_3c;
  uint32_t apbenr[ 2 ];
  uint32_t reserved_48[ 2 ];
  uint32_t ahblpenr[ 3 ];
  uint32_t reserved_5c;
  uint32_t apblpenr[ 2 ];
  uint32_t reserved_68[ 2 ];
  uint32_t bdcr;
  uint32_t csr;
  uint32_t reserved_78[ 2 ];
  uint32_t sscgr;
  uint32_t plli2scfgr;
} stm32f4_rcc;

/** @} */

#define RCC_CR_HSION BSP_BIT32( 0 )
#define RCC_CR_HSIRDY BSP_BIT32( 1 )
#define RCC_CR_HSITRIM( val ) BSP_FLD32( val, 3, 7 )
#define RCC_CR_HSITRIM_MSK BSP_MSK32( 3, 7 )
#define RCC_CR_HSICAL( val ) BSP_FLD32( val, 8, 15 )
#define RCC_CR_HSICAL_MSK BSP_MSK32( 8, 15 )
#define RCC_CR_HSEON BSP_BIT32( 16 )
#define RCC_CR_HSERDY BSP_BIT32( 17 )
#define RCC_CR_HSEBYP BSP_BIT32( 18 )
#define RCC_CR_CSSON BSP_BIT32( 19 )
#define RCC_CR_PLLON BSP_BIT32( 24 )
#define RCC_CR_PLLRDY BSP_BIT32( 25 )
#define RCC_CR_PLLI2SON BSP_BIT32( 26 )
#define RCC_CR_PLLI2SRDY BSP_BIT32( 27 )

#define RCC_PLLCFGR_PLLM( val ) BSP_FLD32( val, 0, 5 )
#define RCC_PLLCFGR_PLLM_MSK BSP_MSK32( 0, 5 )
#define RCC_PLLCFGR_PLLN( val ) BSP_FLD32( val, 6, 14 )
#define RCC_PLLCFGR_PLLN_MSK BSP_MSK32( 6, 14 )

#define RCC_PLLCFGR_PLLP 16
#define RCC_PLLCFGR_PLLP_MSK BSP_MSK32( 16, 17 )
#define RCC_PLLCFGR_PLLP_BY_2 0
#define RCC_PLLCFGR_PLLP_BY_4 BSP_FLD32( 1, 16, 17 )
#define RCC_PLLCFGR_PLLP_BY_6 BSP_FLD32( 2, 16, 17 )
#define RCC_PLLCFGR_PLLP_BY_8 BSP_FLD32( 3, 16, 17 )

#define RCC_PLLCFGR_PLLSRC_HSE BSP_BIT32( 22 )
#define RCC_PLLCFGR_PLLSRC_HSI 0

#define RCC_PLLCFGR_PLLQ( val ) BSP_FLD32( val, 24, 27 )
#define RCC_PLLCFGR_PLLQ_MSK BSP_MSK32( 24, 27 )

#define RCC_CFGR_SW 0
#define RCC_CFGR_SW_MSK BSP_MSK32( 0, 1 )
#define RCC_CFGR_SW_HSI 0
#define RCC_CFGR_SW_HSE 1
#define RCC_CFGR_SW_PLL 2

#define RCC_CFGR_SWS 2
#define RCC_CFGR_SWS_MSK BSP_MSK32( 2, 3 )
#define RCC_CFGR_SWS_HSI 0
#define RCC_CFGR_SWS_HSE BSP_FLD32( 1, 2, 3 )
#define RCC_CFGR_SWS_PLL BSP_FLD32( 2, 2, 3 )

#define RCC_CFGR_HPRE 4
#define RCC_CFGR_HPRE_BY_1 0
#define RCC_CFGR_HPRE_BY_2 BSP_FLD32( 8, 4, 7 )
#define RCC_CFGR_HPRE_BY_4 BSP_FLD32( 9, 4, 7 )
#define RCC_CFGR_HPRE_BY_8 BSP_FLD32( 10, 4, 7 )
#define RCC_CFGR_HPRE_BY_16 BSP_FLD32( 11, 4, 7 )
#define RCC_CFGR_HPRE_BY_64 BSP_FLD32( 12, 4, 7 )
#define RCC_CFGR_HPRE_BY_128 BSP_FLD32( 13, 4, 7 )
#define RCC_CFGR_HPRE_BY_256 BSP_FLD32( 14, 4, 7 )
#define RCC_CFGR_HPRE_BY_512 BSP_FLD32( 15, 4, 7 )

#define RCC_CFGR_PPRE1 10
#define RCC_CFGR_PPRE1_BY_1 0
#define RCC_CFGR_PPRE1_BY_2 BSP_FLD32( 4, 10, 12 )
#define RCC_CFGR_PPRE1_BY_4 BSP_FLD32( 5, 10, 12 )
#define RCC_CFGR_PPRE1_BY_8 BSP_FLD32( 6, 10, 12 )
#define RCC_CFGR_PPRE1_BY_16 BSP_FLD32( 7, 10, 12 )

#define RCC_CFGR_PPRE2 13
#define RCC_CFGR_PPRE2 BSP_MSK32( 13, 15 )
#define RCC_CFGR_PPRE2_BY_1 0
#define RCC_CFGR_PPRE2_BY_2 BSP_FLD32( 4, 13, 15 )
#define RCC_CFGR_PPRE2_BY_4 BSP_FLD32( 5, 13, 15 )
#define RCC_CFGR_PPRE2_BY_8 BSP_FLD32( 6, 13, 15 )
#define RCC_CFGR_PPRE2_BY_16 BSP_FLD32( 7, 13, 15 )

#define RCC_CFGR_RTCPRE( val ) BSP_FLD32( val, 16, 20 )
#define RCC_CFGR_RTCPRE_MSK BSP_MSK32( 16, 20 )

#define RCC_CFGR_MCO1 21
#define RCC_CFGR_MCO1_MSK BSP_MSK32( 21, 22 )
#define RCC_CFGR_MCO1_HSI 0
#define RCC_CFGR_MCO1_LSE BSP_FLD32( 1, 21, 22 )
#define RCC_CFGR_MCO1_HSE BSP_FLD32( 2, 21, 22 )
#define RCC_CFGR_MCO1_PLL BSP_FLD32( 3, 21, 22 )

#define RCC_CFGR_I2SSRC BSP_BIT32( 23 )

#define RCC_CFGR_MCO1PRE 24
#define RCC_CFGR_MCO1PRE_MSK BSP_MSK32( 24, 26 )
#define RCC_CFGR_MCO1PRE_BY_1 0
#define RCC_CFGR_MCO1PRE_BY_2 BSP_FLD32( 4, 24, 26 )
#define RCC_CFGR_MCO1PRE_BY_3 BSP_FLD32( 5, 24, 26 )
#define RCC_CFGR_MCO1PRE_BY_4 BSP_FLD32( 6, 24, 26 )
#define RCC_CFGR_MCO1PRE_BY_5 BSP_FLD32( 7, 24, 26 )

#define RCC_CFGR_MCO2PRE 27
#define RCC_CFGR_MCO2PRE_MSK BSP_MSK32( 27, 29 )
#define RCC_CFGR_MCO2PRE_BY_1 0
#define RCC_CFGR_MCO2PRE_BY_2 BSP_FLD32( 4, 27, 29 )
#define RCC_CFGR_MCO2PRE_BY_3 BSP_FLD32( 5, 27, 29 )
#define RCC_CFGR_MCO2PRE_BY_4 BSP_FLD32( 6, 27, 29 )
#define RCC_CFGR_MCO2PRE_BY_5 BSP_FLD32( 7, 27, 29 )

#define RCC_CFGR_MCO2 30
#define RCC_CFGR_MCO2_MSK BSP_MSK32( 30, 31 )
#define RCC_CFGR_MCO2_SYSCLK 0
#define RCC_CFGR_MCO2_PLLI2S BSP_FLD32( 1, 30, 31 )
#define RCC_CFGR_MCO2_HSE BSP_FLD32( 2, 30, 31 )
#define RCC_CFGR_MCO2_PLL BSP_FLD32( 3, 30, 31 )

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H */