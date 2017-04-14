/*
 *  Time Shell Command Implmentation
 *
 *  Author: Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/inttypes.h>
#include "internal.h"

static int rtems_shell_main_time(
  int   argc,
  char *argv[]
)
{
  int                errorlevel = 0;
  struct timespec    start;
  struct timespec    end;
  struct timespec    period;
  rtems_status_code  sc;

  argc--;

  sc = rtems_clock_get_uptime(&start);
  if (sc != RTEMS_SUCCESSFUL)
    fprintf(stderr, "error: cannot read time\n");

  if (argc) {
    errorlevel = rtems_shell_execute_cmd(argv[1], argc, &argv[1]);
  }

  sc = rtems_clock_get_uptime(&end);
  if (sc != RTEMS_SUCCESSFUL)
    fprintf(stderr, "error: cannot read time\n");

  period.tv_sec = end.tv_sec - start.tv_sec;
  period.tv_nsec = end.tv_nsec - start.tv_nsec;
  if (period.tv_nsec < 0)
  {
    --period.tv_sec;
    period.tv_nsec += 1000000000UL;
  }

  fprintf(stderr, "time: %" PRIdtime_t ":%02" PRIdtime_t ":%02" PRIdtime_t ".%03li\n",
         period.tv_sec / 3600,
         period.tv_sec / 60, period.tv_sec % 60,
         period.tv_nsec / 1000000);

  return errorlevel;
}

rtems_shell_cmd_t rtems_shell_TIME_Command = {
  .name = "time",
  .usage = "time command [arguments...]",
  .topic = "misc",
  .command = rtems_shell_main_time,
  .mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
};
