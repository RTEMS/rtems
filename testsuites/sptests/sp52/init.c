/*
 *  Based upon test code posted on the RTEMS User's Mailing List
 *  by Sergio Faustino <sergio.faustino@edisoft.pt>:
 *
 *    http://www.rtems.org/pipermail/rtems-users/2009-June/005540.html
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if defined(USE_TIMER_SERVER)
   #define TEST_NUMBER      "53"
   #define TSR_MODE         "Server"
   #define FIRE_WHEN        rtems_timer_server_fire_when
   #define FIRE_WHEN_STRING "rtems_timer_server_fire_when"
#else
   #define TEST_NUMBER      "52"
   #define TSR_MODE         "Interrupt"
   #define FIRE_WHEN        rtems_timer_fire_when
   #define FIRE_WHEN_STRING "rtems_timer_fire_when"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine TIMER_service_routine(
  rtems_id  ignored_id,
  void     *user_data
);

#define INITIAL_SECOND 10
volatile bool _timer_passage = FALSE;

/*timer Routine*/
rtems_timer_service_routine TIMER_service_routine(
  rtems_id  ignored_id,
  void     *user_data
)
{
  _timer_passage = TRUE;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          timer_id;
  rtems_name        timer_name;

  rtems_time_of_day global_time;
  rtems_time_of_day time_to_fire;

  puts( "\n\n*** TEST " TEST_NUMBER " ***" );

  /* build timer name*/
  timer_name = rtems_build_name('T', 'M', '1', ' ');

  /* create Timer */
  status = rtems_timer_create(timer_name, &timer_id);
  directive_failed( status, "rtems_timer_create" );

  #if defined(USE_TIMER_SERVER)
    /* initiate timer server */
    status = rtems_timer_initiate_server(
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES
    );
    directive_failed( status, "rtems_timer_initiate_server" );
  #endif

  /* Set system clock  */
  build_time(&global_time, 6, 8, 2009, 16, 5, INITIAL_SECOND, 0);
  status = rtems_clock_set(&global_time);
  directive_failed( status, "rtems_clock_set" );

  /* Set Timer to Fire */
  /* build fire times */
  time_to_fire = global_time;

  /* only diferent second */
  time_to_fire.second = INITIAL_SECOND + 5;

  status = FIRE_WHEN(
    timer_id,
    &time_to_fire,
    TIMER_service_routine,
    (void*) NULL
  );
  directive_failed( status, FIRE_WHEN_STRING );

  /* Set system clock FORWARD */
  global_time.second = INITIAL_SECOND + 10;
  status = rtems_clock_set(&global_time);

  if (!_timer_passage) {
    puts( TSR_MODE " Timer FAILED to fire after setting time forward");
    rtems_test_exit(0);
  }

  puts( TSR_MODE " Timer fired after setting time forward -- OK");

  puts( "*** END OF TEST " TEST_NUMBER " ***" );
  rtems_test_exit(0);
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              2
#define CONFIGURE_MAXIMUM_TIMERS             1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY (RTEMS_MINIMUM_PRIORITY + 1)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
