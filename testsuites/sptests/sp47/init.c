/*
 *  Submitted as part of PR1357
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#define PRIXModes_Control 	PRIX32
#define PRIXrtems_mode		PRIXModes_Control

rtems_task test_asr(rtems_task_argument unused);
rtems_task Init(rtems_task_argument ignored);

rtems_task test_asr(rtems_task_argument unused)
{
  rtems_mode mode;

  rtems_task_mode(0, RTEMS_CURRENT_MODE, &mode);

  if ( (mode & RTEMS_NO_ASR) == 0 ) {
    puts( "ERROR - disable ASR not honored" );
    printf(
      "mode = 0x%08" PRIXrtems_mode " asr = %s\n", mode,
      (mode & RTEMS_NO_ASR) ? "OFF" : "ON"
    );
  } else
    puts( "Creating task with ASR disable mode honored" );

  puts( "*** END OF TEST 47 ***" );
  rtems_test_exit( 0 );
}

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code sc;
  rtems_id ti;

  puts( "\n\n*** TEST 47 ***" );
  sc = rtems_task_create(
    rtems_build_name('t', 's', 't', '0'),
    100,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_ASR,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ti
  );

  if (sc != RTEMS_SUCCESSFUL) {
    printf("rtems_task_create failed: %i\n", sc);
    exit(0);
  }

  rtems_task_start(ti, test_asr, 0);

  rtems_task_delete(RTEMS_SELF);
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_MAXIMUM_TASKS            64
#define CONFIGURE_MAXIMUM_PERIODS		  10

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
