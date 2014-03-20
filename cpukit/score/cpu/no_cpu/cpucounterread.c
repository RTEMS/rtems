/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>

CPU_Counter_ticks _CPU_Counter_read( void )
{
  static CPU_Counter_ticks counter;

  CPU_Counter_ticks snapshot;

  snapshot = counter;
  counter = snapshot + 1;

  return snapshot;
}
