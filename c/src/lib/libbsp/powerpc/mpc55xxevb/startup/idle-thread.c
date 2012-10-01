/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief bsp_idle_thread() implementation.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>

#include <mpc55xx/regs.h>

void *bsp_idle_thread(uintptr_t arg)
{
  while (true) {
    #ifdef MPC55XX_HAS_WAIT_INSTRUCTION
      __asm__ volatile ("wait");
    #endif
  }

  return NULL;
}
