/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define CONFIGURE_INIT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include <tmacros.h>

const char rtems_test_name[] = "CAPTURE ENGINE";

volatile int can_proceed = 1;

static void notification(int fd, int seconds_remaining, void *arg)
{
  printf(
    "Press any key to start capture engine (%is remaining)\n",
    seconds_remaining
  );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code   status;
  rtems_task_priority old_priority;
  rtems_mode          old_mode;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);
  TEST_BEGIN();

  status = rtems_shell_wait_for_input(
    STDIN_FILENO,
    20,
    notification,
    NULL
  );
  if (status == RTEMS_SUCCESSFUL) {
    /* lower the task priority to allow created tasks to execute */

    rtems_task_set_priority(RTEMS_SELF, 20, &old_priority);
    rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

    while (!can_proceed)
    {
      printf ("Sleeping\n");
      usleep (1000000);
    }

    rtems_monitor_init (0);
    rtems_capture_cli_init (0);

    setup_tasks_to_watch ();

    rtems_task_exit();
  } else {
    TEST_END();
    rtems_test_exit( 0 );
  }
}
