/**
 * @file
 *
 * @author Sebastian Huber <sebastian.huber@embedded-brains.de>
 *
 * @ingroup lpc24xx
 *
 * @brief Idle task.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/lpc24xx.h>

void *bsp_idle_thread(uintptr_t ignored)
{
  while (true) {
    /*
     * Set power mode to idle.  Causes the processor clock to be stopped, while
     * on-chip peripherals remain active.  Any enabled interrupt from a
     * peripheral or an external interrupt source will cause the processor to
     * resume execution.
     */
    PCON = 0x1;
  }
}
