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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>

static CPU_Counter_ticks _SPARC_Counter_register_dummy;

CPU_Counter_ticks _SPARC_Counter_difference_default(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return 1;
}

SPARC_Counter _SPARC_Counter = {
  .counter_register = &_SPARC_Counter_register_dummy,
  .counter_difference = _SPARC_Counter_difference_default
};
