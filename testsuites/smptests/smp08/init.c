/*
 *  COPYRIGHT (c) 1989-2011.
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

void PrintTaskInfo(
  const char         *task_name,
  rtems_time_of_day  *_tb 
)
{
  int               cpu_num;

  cpu_num = bsp_smp_processor_id();

  /* Print the cpu number and task name */
  locked_printf(
    "  CPU %d running task %s - rtems_clock_get_tod "
    "%02" PRId32 ":%02" PRId32 ":%02" PRId32 "   %02" PRId32 
        "/%02" PRId32 "/%04" PRId32 "\n",
    cpu_num,
    task_name,
    _tb->hour, _tb->minute, _tb->second, 
    _tb->month, _tb->day, _tb->year
  ); 
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  int               i;
  char              ch[4];
  rtems_id          id;
 
  locked_print_initialize();
  locked_printf( "\n\n*** SMP08 TEST ***\n" );

  time.year   = 1988;
  time.month  = 12;
  time.day    = 31;
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );

  /* Create/verify synchronisation semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create" );

  /* Show that the init task is running on this cpu */
  PrintTaskInfo( "Init", &time );

  for ( i=1; i <= rtems_smp_get_number_of_processors() *3; i++ ) {

    sprintf(ch, "%02" PRId32, i );
    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch[0], ch[1] ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );
  }

  status = rtems_task_delete( RTEMS_SELF );
}
