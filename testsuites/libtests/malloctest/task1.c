/*  task1.c
 *
 *  This set of three tasks do some simple task switching for about
 *  15 seconds and then call a routine to "blow the stack".
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "system.h"
#include <rtems/libcsupport.h>    /* for malloc_dump, malloc_walk */
#include <string.h>         /* for memset */
#include <stdlib.h>

#define NUM_PASSES 100

rtems_task Task_1_through_5(
  rtems_task_argument argument
)
{
  int i;
  unsigned int passes = 0;
  rtems_id          tid;
  rtems_time_of_day time;
  rtems_status_code status;
  unsigned char *mem_ptr;
  int mem_amt;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  while (TRUE)
  {
    if ( passes++ > NUM_PASSES ) {
	puts("*** END OF MALLOC TEST ***");
        rtems_test_exit(0);
    }

    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
    directive_failed( status, "rtems_clock_get" );

    put_name( Task_name[ task_number( tid ) ], FALSE );
    print_time( " - rtems_clock_get - ", &time, "\n" );

    mem_amt = ((int)((float)rand()*1000.0/(float)RAND_MAX));
    while (!(mem_ptr = malloc ( mem_amt))) {
	printf("out of memory... trying again.\n");
	mem_amt = ((int)((float)rand()*1000.0/(float)RAND_MAX));
    }
    printf("mallocing %d bytes\n",mem_amt);
    memset( mem_ptr, mem_amt, mem_amt );
    malloc_dump();
    malloc_walk(1,FALSE);
    status = rtems_task_wake_after( task_number( tid ) * 1 * TICKS_PER_SECOND/4 );
    for (i=0; i < mem_amt; i++)
    {
       if ( mem_ptr[i] != (mem_amt & 0xff))
       {
          printf("failed %d, %d, 0x%x, 0x%x\n",i,mem_amt,mem_ptr[i],mem_amt&0xff);
          rtems_test_exit(1);
       }
    }
    directive_failed( status, "rtems_task_wake_after" );
    free( mem_ptr );
  }
}
