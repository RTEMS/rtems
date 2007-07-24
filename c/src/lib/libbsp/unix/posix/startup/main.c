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
  boot_card();

  return 0;
}
