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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_PWR_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_PWR_H

#include <bsp/utility.h>

struct stm32f4_pwr_s {

  uint32_t cr;    // Control register
#define STM32F4_PWR_CR_VOS        BSP_BIT32(14) // Regulator scaling output selection
#define STM32F4_PWR_CR_FPDS       BSP_BIT32(9)  // Flash power-down in stop mode
#define STM32F4_PWR_CR_DBP        BSP_BIT32(8)  // Disable backup domain write protection
#define STM32F4_PWR_CR_PLS  BSP_FLD32(val, 5, 7)  // PVD level selection
#define STM32F4_PWR_CR_PLS_GET(reg) BSP_FLD32GET(reg, 5, 7)
#define STM32F4_PWR_CR_PLS_SET(reg, val)  BSP_FLD32SET(reg, val, 5, 7)
#define STM32F4_PWR_CR_PVDE       BSP_BIT32(4)  // Power voltage detector enable
#define STM32F4_PWR_CR_CSBF       BSP_BIT32(3)  // Clear standby flag
#define STM32F4_PWR_CR_CWUF       BSP_BIT32(2)  // Clear wakeup flag
#define STM32F4_PWR_CR_PDDS       BSP_BIT32(1)  // Power-down deepsleep
#define STM32F4_PWR_CR_LPDS       BSP_BIT32(0)  // Low-power deepsleep

  uint32_t csr;   // Control / status register
#define STM32F4_PWR_CSR_VOSRDY    BSP_BIT32(14) // Regulator voltage scaling output selection ready bit
#define STM32F4_PWR_CSR_BRE       BSP_BIT32(9)  // Backup domain regulator enable
#define STM32F4_PWR_CSR_EWUP      BSP_BIT32(8)  // Enable WKUP pin
#define STM32F4_PWR_CSR_BRR       BSP_BIT32(3)  // Backup regulator ready
#define STM32F4_PWR_CSR_PVDO      BSP_BIT32(2)  // PVD output
#define STM32F4_PWR_CSR_SBF       BSP_BIT32(1)  // Standby flag
#define STM32F4_PWR_CSR_WUF       BSP_BIT32(0)  // Wakeup flag

} __attribute__ ((packed));
typedef struct stm32f4_pwr_s stm32f4_pwr;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_PWR_H */
