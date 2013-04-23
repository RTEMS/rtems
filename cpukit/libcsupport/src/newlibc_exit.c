/*
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <rtems/libcsupport.h>

#include <stdio.h>
#include <unistd.h>

static void libc_wrapup(void)
{
  /*
   *  In case RTEMS is already down, don't do this.  It could be
   *  dangerous.
   */

  if (!_System_state_Is_up(_System_state_Get()))
     return;

  /*
   * Try to drain output buffers.
   *
   * Should this be changed to do *all* file streams?
   *    _fwalk (_REENT, fclose);
   */

  fclose (stdin);
  fclose (stdout);
  fclose (stderr);
}

/* FIXME: These defines are a blatant hack */

  #if defined(__USE_INIT_FINI__)
    #if defined(__m32r__)
      #define FINI_SYMBOL __fini
    #elif defined(__ARM_EABI__)
      #define FINI_SYMBOL __libc_fini_array
    #else
      #define FINI_SYMBOL _fini
    #endif

    extern void FINI_SYMBOL( void );
  #endif

void _exit(int status)
{
  /*
   *  If the toolset uses init/fini sections, then we need to
   *  run the global destructors now.
   */
  #if defined(FINI_SYMBOL)
    FINI_SYMBOL();
  #endif

  libc_wrapup();
  rtems_shutdown_executive(status);
  for (;;) ; /* to avoid warnings */
}

#endif
