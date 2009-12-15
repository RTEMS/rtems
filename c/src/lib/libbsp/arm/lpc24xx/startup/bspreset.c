/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Reset code.
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

#include <rtems.h>

#include <bsp/bootcard.h>
#include <bsp/lpc24xx.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  /* Trigger watchdog reset */
  WDCLKSEL = 0;
  WDTC = 0xff;
  WDMOD = 0x3;
  WDFEED = 0xaa;
  WDFEED = 0x55;

  while (true) {
    /* Do nothing */
  }
}
