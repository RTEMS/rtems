/*  bsp_cleanup()
 *
 *  This routine normally is part of start.s and usually returns
 *  control to a monitor.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/startup/bspclean.c:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

static void _noopfun(void) {}

void app_bsp_cleanup(void)
__attribute__(( weak, alias("_noopfun") ));

void bsp_cleanup( void )
{
  app_bsp_cleanup();

  /* All done.  Hang out. */
  BSP_ask_for_reset();
}
