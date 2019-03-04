/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

#include <libcpu/arm-cp15.h>

void *bsp_idle_thread(uintptr_t arg)
{
  while (true) {
    arm_cp15_wait_for_interrupt();
  }

  return NULL;
}
