/*  Application_task
 *
 *  This routine is as an example of an application task which
 *  prints a message including its RTEMS task id.  This task
 *  then invokes exit to return to the monitor.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "tmacros.h"

#include <stdio.h>
#include <stdlib.h>

rtems_task Application_task(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_status_code status;
  unsigned int      a = (unsigned int) argument;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "ident" );

  printf(
    "Application task was invoked with argument (%d) "
    "and has id of 0x%" PRIxrtems_id "\n", a, tid
  );

  TEST_END();
  exit( 0 );
}
