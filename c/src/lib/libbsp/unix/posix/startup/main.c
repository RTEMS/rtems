/*
 *  Description:
 *  This file exists solely to (try to) ensure RTEMS is initialized.
 *  It is not to be used in systems which want C++ support.
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * RTEMS program name
 * Probably not used by anyone, but it is nice to have it.
 * Actually the UNIX version of CPU_INVOKE_DEBUGGER will probably
 * need to use it
 */

char *rtems_progname;

int main(
  int argc,
  char **argv
)
{
  rtems_argc = argc;
  rtems_argv = argv;

  bsp_start();

  if ((argc > 0) && argv && argv[0])
      rtems_progname = argv[0];
  else
      rtems_progname = "RTEMS";

  /*
   *  Start multitasking
   */

  rtems_initialize_executive_late( bsp_isr_level );

  /*
   * Returns when multitasking is stopped
   * This allows our destructors to get run normally
   */

  fflush( stdout );
  fflush( stderr );

  bsp_cleanup();

  return 0;
}
