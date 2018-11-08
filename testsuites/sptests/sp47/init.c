/*
 *  Submitted as part of PR1357
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SP 47";

#define PRIXrtems_mode 	PRIX32

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

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code sc;
  rtems_id ti;

  TEST_BEGIN();
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

  rtems_task_exit();
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_MAXIMUM_TASKS            64
#define CONFIGURE_MAXIMUM_PERIODS		  10

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
