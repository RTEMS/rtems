/*
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

#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "SMP 5";

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);


static void success(void)
{
  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  locked_printf( "Shut down from CPU %" PRIu32 "\n", rtems_scheduler_get_processor() );
  success();
}

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t           i;
  char               ch;
  uint32_t           cpu_num;
  rtems_id           id;
  rtems_status_code  status;

  locked_print_initialize();
  TEST_BEGIN();

  if ( rtems_scheduler_get_processor_maximum() == 1 ) {
    success();
  }

  for ( i=0; i<rtems_scheduler_get_processor_maximum() ; i++ ) {
    ch = '1' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    cpu_num = rtems_scheduler_get_processor();
    locked_printf(" CPU %" PRIu32 " start task TA%c\n", cpu_num, ch);

    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );
  }

  while (1)
    ;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS   2

#define CONFIGURE_MAXIMUM_TASKS            \
    (1 + CONFIGURE_MAXIMUM_PROCESSORS)
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
