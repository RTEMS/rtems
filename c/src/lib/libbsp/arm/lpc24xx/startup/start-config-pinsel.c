/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start pin selection configuration.
 */

/*
 * Copyright (c) 2011-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/start-config.h>

BSP_START_DATA_SECTION const lpc24xx_pin_range
  lpc24xx_start_config_pinsel [] = {
#if defined(LPC24XX_EMC_MT48LC4M16A2) \
  && defined(LPC24XX_EMC_M29W160E)
  LPC24XX_PIN_EMC_A_0_20,
  LPC24XX_PIN_EMC_D_0_15,
  LPC24XX_PIN_EMC_RAS,
  LPC24XX_PIN_EMC_CAS,
  LPC24XX_PIN_EMC_WE,
  LPC24XX_PIN_EMC_DYCS_0,
  LPC24XX_PIN_EMC_CLK_0,
  LPC24XX_PIN_EMC_CKE_0,
  LPC24XX_PIN_EMC_DQM_0,
  LPC24XX_PIN_EMC_DQM_1,
  LPC24XX_PIN_EMC_OE,
  LPC24XX_PIN_EMC_CS_1,
#endif
#if (defined(LPC24XX_EMC_IS42S32800D7) || defined(LPC24XX_EMC_W9825G2JB75I)) \
  && (defined(LPC24XX_EMC_M29W320E70) || defined(LPC24XX_EMC_SST39VF3201))
  LPC24XX_PIN_EMC_A_0_22,
  LPC24XX_PIN_EMC_D_0_31,
  LPC24XX_PIN_EMC_RAS,
  LPC24XX_PIN_EMC_CAS,
  LPC24XX_PIN_EMC_WE,
  LPC24XX_PIN_EMC_DYCS_0,
  LPC24XX_PIN_EMC_CLK_0,
  LPC24XX_PIN_EMC_CKE_0,
  LPC24XX_PIN_EMC_DQM_0,
  LPC24XX_PIN_EMC_DQM_1,
  LPC24XX_PIN_EMC_DQM_2,
  LPC24XX_PIN_EMC_DQM_3,
  LPC24XX_PIN_EMC_OE,
  LPC24XX_PIN_EMC_CS_0,
#endif
#if defined(LPC24XX_EMC_IS42S32800B)
  LPC24XX_PIN_EMC_A_0_14,
  LPC24XX_PIN_EMC_D_0_31,
  LPC24XX_PIN_EMC_RAS,
  LPC24XX_PIN_EMC_CAS,
  LPC24XX_PIN_EMC_WE,
  LPC24XX_PIN_EMC_DYCS_0,
  LPC24XX_PIN_EMC_CLK_0,
  LPC24XX_PIN_EMC_CKE_0,
  LPC24XX_PIN_EMC_DQM_0,
  LPC24XX_PIN_EMC_DQM_1,
  LPC24XX_PIN_EMC_DQM_2,
  LPC24XX_PIN_EMC_DQM_3,
#endif
  LPC24XX_PIN_TERMINAL
};
