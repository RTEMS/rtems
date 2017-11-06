/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <tmacros.h>

extern int paranoia(int, char **);

const char rtems_test_name[] = "PARANOIA";

char *args[2] = { "paranoia", 0 };

rtems_task Init(
  rtems_task_argument ignored
)
{
  /*
   *  Install whatever optional floating point assistance package
   *  is required by this CPU.
   */

#if (defined (m68040))
  M68KFPSPInstallExceptionHandlers ();
#endif

  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  TEST_BEGIN();
  paranoia(1, args);
  TEST_END();
  rtems_test_exit( 0 );
}
