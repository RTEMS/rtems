/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start pin selection configuration.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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
 *
 * $Id$
 */

#include <bsp/start-config.h>

const BSP_START_DATA_SECTION uint32_t lpc24xx_start_config_pinsel_5_9 [] = {
#if defined(LPC24XX_EMC_MICRON) \
  || defined(LPC24XX_EMC_K4S561632E) \
  || defined(LPC24XX_EMC_NUMONYX)
  0x05010115,
  0x55555555,
  0x0,
  #ifdef LPC24XX_EMC_K4S561632E
    0x15555555,
    0x0a040000
  #else
    0x55555555,
    0x40050155
  #endif
#endif
};

const BSP_START_DATA_SECTION size_t
  lpc24xx_start_config_pinsel_5_9_size =
    sizeof(lpc24xx_start_config_pinsel_5_9);
