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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H

#include <bsp/utility.h>

struct stm32f4_flash_s {

  uint32_t acr;   // Access and control register
#define STM32F4_FLASH_ACR_DCRST   BSP_BIT32(12) // Data cache reset
#define STM32F4_FLASH_ACR_ICRST   BSP_BIT32(11) // Instruction cache reset
#define STM32F4_FLASH_ACR_DCEN    BSP_BIT32(10) // Data cache enable
#define STM32F4_FLASH_ACR_ICEN    BSP_BIT32(9)  // Instruction cache enable
#define STM32F4_FLASH_ACR_PRFTEN  BSP_BIT32(8)  // Prefetch enable
#define STM32F4_FLASH_ACR_LATENCY(val)  BSP_FLD32(val, 0, 2)  // Flash access latency
#define STM32F4_FLASH_ACR_LATENCY_GET(reg)  BSP_FLD32GET(reg, 0, 2)
#define STM32F4_FLASH_ACR_LATENCY_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)

  uint32_t keyr;  // Key register
#define STM32F4_FLASH_KEYR_KEY1 0x45670123
#define STM32F4_FLASH_KEYR_KEY2 0xCDEF89AB

  uint32_t optkeyr; // Option key register
#define STM32F4_FLASH_OPTKEYR_OPTKEY1 0x08192A3B
#define STM32F4_FLASH_OPTKEYR_OPTKEY2 0x4C5D6E7F

  uint32_t sr;    // Status register
#define STM32F4_FLASH_SR_BSY    BSP_BIT32(16) // Busy
#define STM32F4_FLASH_SR_PGSERR BSP_BIT32(7)  // Programming sequence error
#define STM32F4_FLASH_SR_PGPERR BSP_BIT32(6)  // Programming parallelism error
#define STM32F4_FLASH_SR_PGAERR BSP_BIT32(5)  // Programming alignment error
#define STM32F4_FLASH_SR_WRPERR BSP_BIT32(4)  // Write protection error
#define STM32F4_FLASH_SR_OPERR  BSP_BIT32(1)  // Operation error
#define STM32F4_FLASH_SR_EOP    BSP_BIT32(0)  // End of operation

  uint32_t cr;    // Control register
#define STM32F4_FLASH_CR_LOCK   BSP_BIT32(31) // Lock
#define STM32F4_FLASH_CR_ERRIE  BSP_BIT32(25) // Error interrupt enable
#define STM32F4_FLASH_CR_EOPIE  BSP_BIT32(24) // End of operation interrupt enable
#define STM32F4_FLASH_CR_STRT   BSP_BIT32(16) // Start
#define STM32F4_FLASH_CR_PSIZE(val) BSP_FLD32(val, 8, 9)  // Program size
#define STM32F4_FLASH_CR_PSIZE_GET(reg) BSP_FLD32GET(reg, 8, 9)
#define STM32F4_FLASH_CR_PSIZE_SET(reg, val)  BSP_FLD32SET(reg, val, 8, 9)
#define STM32F4_FLASH_CR_SNB  BSP_FLD32(val, 3, 6)  // Sector number
#define STM32F4_FLASH_CR_SNB_GET(reg) BSP_FLD32GET(reg, 3, 6)
#define STM32F4_FLASH_CR_SNB_SET(reg, val)  BSP_FLD32SET(reg, val, 3, 6)
#define STM32F4_FLASH_CR_MER    BSP_BIT32(2)  // Mass erase
#define STM32F4_FLASH_CR_SER    BSP_BIT32(1)  // Sector erase
#define STM32F4_FLASH_CR_PG     BSP_BIT32(0)  // Programming

  uint32_t optcr;   // Option control register
#define STM32F4_FLASH_OPTCR_NWRP(val) BSP_FLD32(val, 16, 27)  // Not write protect
#define STM32F4_FLASH_OPTCR_NWRP_GET(reg) BSP_FLD32GET(reg, 16, 27)
#define STM32F4_FLASH_OPTCR_NWRP_SET(reg, val)  BSP_FLD32SET(reg, val, 16, 27)
#define STM32F4_FLASH_OPTCR_RDP(val)  BSP_FLD32(val, 8, 15) // Read protect
#define STM32F4_FLASH_OPTCR_RDP_GET(reg)  BSP_FLD32GET(reg, 8, 15)
#define STM32F4_FLASH_OPTCR_RDP_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define STM32F4_FLASH_OPTCR_USER(val) BSP_FLD32(val, 5, 7)  // User option bytes
#define STM32F4_FLASH_OPTCR_USER_GET(reg) BSP_FLD32GET(reg, 5, 7)
#define STM32F4_FLASH_OPTCR_USER_SET(reg, val)  BSP_FLD32SET(reg, val, 5, 7)
#define STM32F4_FLASH_OPTCR_BOR_LEVEL(val)  BSP_FLD32(val, 2, 3)  // BOR reset level
#define STM32F4_FLASH_OPTCR_BOR_LEVEL_GET(reg)  BSP_FLD32GET(reg, 2, 3)
#define STM32F4_FLASH_OPTCR_BOR_LEVEL_SET(reg, val) BSP_FLD32SET(reg, val, 2, 3)
#define STM32F4_FLASH_CR_OPTSTRT  BSP_BIT32(1)  // Option start
#define STM32F4_FLASH_CR_OPTLOCK  BSP_BIT32(0)  // Option lock

} __attribute__ ((packed));
typedef struct stm32f4_flash_s stm32f4_flash;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_FLASH_H */
