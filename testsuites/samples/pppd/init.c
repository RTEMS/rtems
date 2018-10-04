/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIGURE_INIT
#include "system.h"

#include <rtems/rtems_bsdnet.h>
#include <rtems/rtemspppd.h>
#include <rtems/shell.h>
#include "netconfig.h"
#include <tmacros.h>

const char rtems_test_name[] = "PPPD";

static void notification(int fd, int seconds_remaining, void *arg)
{
  printf(
    "Press any key to start pppd (%is remaining)\n",
    seconds_remaining
  );
}

rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code status;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);

  TEST_BEGIN();

  status = rtems_shell_wait_for_input(
    STDIN_FILENO,
    10,
    notification,
    NULL
  );
  if (status != RTEMS_SUCCESSFUL) {
    TEST_END();
    exit( 0 );
  }

  /* initialize network */
  rtems_bsdnet_initialize_network();
  rtems_pppd_initialize();
  pppdapp_initialize();
  rtems_task_exit();
}
