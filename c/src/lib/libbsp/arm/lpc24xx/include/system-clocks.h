/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H
#define LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void lpc24xx_micro_seconds_delay( unsigned us);

unsigned lpc24xx_cclk( void);

void lpc24xx_set_pll( unsigned clksrc, unsigned nsel, unsigned msel, unsigned cclksel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H */
