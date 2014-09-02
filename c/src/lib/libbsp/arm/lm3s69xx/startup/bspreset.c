/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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


#include <rtems.h>
#include <rtems/score/armv7m.h>

#include <bsp/bootcard.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;

  (void) level;
  rtems_interrupt_disable(level);

  _ARMV7M_SCB->aircr = ARMV7M_SCB_AIRCR_VECTKEY
    | ARMV7M_SCB_AIRCR_SYSRESETREQ;

  while (true)
    /* Do nothing */;
}
