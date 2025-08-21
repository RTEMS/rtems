/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <ep7312.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

#if ON_SKYEYE == 1
  #define SKYEYE_MAGIC_ADDRESS (*(volatile unsigned int *)(0xb0000000))

  SKYEYE_MAGIC_ADDRESS = 0xff;
#else
  __asm__ volatile ("b _start");
#endif
  RTEMS_UNREACHABLE();
}
