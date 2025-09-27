/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * Copyright (c) 2009 Yang Xi <hiyangxi@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <pxa255.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

#if ON_SKYEYE == 1
  SKYEYE_MAGIC_ADDRESS = 0xff;
#endif
  RTEMS_UNREACHABLE();
}
