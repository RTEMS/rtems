/**
 * @file
 *
 * @ingroup stm32f4_flash
 *
 * @brief STM32F4XXXX FLASH support.
 *
 * Contains structure desribing registers responsible for the flash memory
 * configuration.
 */

/*
 * Copyright (c) 2014 Tomasz Gregorek.  All rights reserved.
 *
 *  <tomasz.gregorek@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H

#include <bsp/utility.h>

/**
 * @defgroup stm32f10xxx_flash STM32F4XXXX FLASH Support
 * @ingroup stm32f4_flash
 * @brief STM32F4FXXX FLASH Support
 * @{
 */

typedef struct {
  uint32_t acr;
  uint32_t keyr;
  uint32_t optkeyr;
  uint32_t sr;
  uint32_t cr;
  uint32_t optcr;
  uint32_t optcr1;
} stm32f4_flash;

/** @} */

#define FLASH_ACR_LATENCY( val ) BSP_FLD32( val, 0, 3 )
#define FLASH_ACR_LATENCY_MSK BSP_MSK32( 0, 3 )
#define FLASH_ACR_PRFTEN BSP_BIT32( 8 )
#define FLASH_ACR_ICEN BSP_BIT32( 9 )
#define FLASH_ACR_DCEN BSP_BIT32( 10 )
#define FLASH_ACR_ICRST BSP_BIT32( 11 )
#define FLASH_ACR_DCRST BSP_BIT32( 12 )

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H */