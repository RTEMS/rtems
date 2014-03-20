/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/bootcard.h>

void bsp_reset(void)
{
  volatile uint32_t *sys_lock = (volatile uint32_t *) 0x10000020;
  volatile uint32_t *sys_resetctl = (volatile uint32_t *) 0x10000040;

  while (true) {
    *sys_lock = 0xa05f;
    *sys_resetctl = 0xf4;
  }
}
