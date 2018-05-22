/*
 * Copyright (c) 2016, 2018 embedded brains GmbH.  All rights reserved.
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

#include <bsp/clock-armv7m.h>

uint32_t _CPU_Counter_frequency(void)
{
  return _ARMV7M_Clock_frequency();
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
  return _ARMV7M_Clock_counter(&_ARMV7M_TC);
}
