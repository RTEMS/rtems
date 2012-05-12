/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h> /* exit */

extern int paranoia(int, char **);

char *args[2] = { "paranoia", 0 };

rtems_task Init(
  rtems_task_argument ignored
)
{
  /*
   *  Install whatever optional floating point assistance package
   *  is required by this CPU.
   */

#if BSP_SMALL_MEMORY
  printf("NO Paranoia Test. MEMORY TOO SMALL");
#else

#if (defined (m68040))
  M68KFPSPInstallExceptionHandlers ();
#endif

  printf( "\n\n*** PARANOIA TEST ***\n" );
  paranoia(1, args);
  printf( "*** END OF PARANOIA TEST ***\n" );
#endif /* BSP_SMALL_MEMORY */
  exit( 0 );
}
