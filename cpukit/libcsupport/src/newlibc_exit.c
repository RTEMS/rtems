/*
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <rtems/libio.h>

#include <stdio.h>
#include <unistd.h>

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

  (*rtems_libio_exit_helper)();
  rtems_shutdown_executive(status);
  for (;;) ; /* to avoid warnings */
}

#endif
