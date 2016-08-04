#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CONFIGURE_INIT
#include "system.h"
#include <rtems/test.h>
#include <rtems/status-checks.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int experiment_flag = 0;
int testnumber = 15;
rtems_id Task_id[ 2 ];
rtems_name Task_name[ 2 ];
uint32_t tick_per_second;

rtems_task Init(
	rtems_task_argument argument
)
{
  rtems_status_code status;

	tick_per_second = rtems_clock_get_ticks_per_second();
	printf("\nTicks per second in your system: %" PRIu32 "\n", tick_per_second);

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );


  //Two tasks creating
  status = rtems_task_create(
    Task_name[ 1 ], 2, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
  );
  if ( status != RTEMS_SUCCESSFUL) {
    printf( "rtems_task_create 1 failed with status of %d.\n", status );
    exit( 1 );
  } 

  status = rtems_task_create(
    Task_name[ 2 ], 5, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 2 ]
  );
  if ( status != RTEMS_SUCCESSFUL) {
    printf( "rtems_task_create 2 failed with status of %d.\n", status );
    exit( 1 );
  }

  //After creating the periods for tasks, start to run them sequencially.

  experiment_flag = 1;
	status = rtems_task_start( Task_id[ 1 ], Task_1, 1);
  if ( status != RTEMS_SUCCESSFUL) {
    printf( "rtems_task_start 1 failed with status of %d.\n", status );
    exit( 1 );
  }
  status = rtems_task_start( Task_id[ 2 ], Task_2, 1);
  if ( status != RTEMS_SUCCESSFUL) {
    printf( "rtems_task_start 2 failed with status of %d.\n", status );
    exit( 1 );
  }

	status = rtems_task_delete( RTEMS_SELF );
}
