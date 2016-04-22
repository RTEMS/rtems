/**
 * @file
 *
 * This routine is as an example of an application task which
 * prints a message including its RTEMS task id.  This task
 * then invokes exit to return to the monitor.
 */

/*
 *  COPYRIGHT (c) 1989-1999, 2016.
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
#include <stdio.h>
#include <stdlib.h>

#include "tmacros.h"

rtems_task Application_task(
  rtems_task_argument node
)
{
  rtems_id          tid;
  rtems_status_code status;

  rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  (void) status;
  printf( "This task was invoked with the node argument (%" PRIdrtems_task_argument ")\n", node );
  printf( "This task has the id of 0x%" PRIxrtems_id "\n",  tid );
  TEST_END();
  exit( 0 );
}
