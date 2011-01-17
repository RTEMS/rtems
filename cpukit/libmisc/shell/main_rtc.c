/**
 * @file
 *
 * Real time clock shell command.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rtems.h>
#include <rtems/rtc.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#define RTEMS_RTC_SHELL_ERROR( fmt, ...) \
  do { \
    printf( "error: " fmt "\n", ##__VA_ARGS__); \
    return -1; \
  } while (0)

#define RTEMS_RTC_SHELL_ERROR_SC( sc, fmt, ...) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    printf( "error: " fmt ": %s\n", ##__VA_ARGS__, rtems_status_text( sc)); \
    return -1; \
  }

static const char rtems_rtc_shell_usage [] =
  "real time clock read and set\n"
  "\n"
  "rtc\n"
  "\tprints the current time of day\n"
  "\n"
  "rtc YYYY-MM-DD [HH:MM:SS [TICKS]]\n"
  "\tsets the time of day and real time clock";

static int rtems_rtc_shell_main( int argc, char **argv)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_time_of_day tod = {
    .year = 1988,
    .month = 1,
    .day = 1,
    .hour = 0,
    .minute = 0,
    .second = 0,
    .ticks = 0
  };

  if (argc == 1) {
    sc = rtems_clock_get_tod( &tod);
    RTEMS_RTC_SHELL_ERROR_SC( sc, "get time of day");

    printf(
      "%04" PRIu32 "-%02" PRIu32 "-%02" PRIu32
        " %02" PRIu32 ":%02" PRIu32 ":%02" PRIu32
        " %02" PRIu32 "\n",
      tod.year,
      tod.month,
      tod.day,
      tod.hour,
      tod.minute,
      tod.second,
      tod.ticks
    );
  } else if (argc > 1 && argc < 5) {
    int rv = 0;
    int fd = 0;
    ssize_t n = 0;
    uint32_t v [3];

    if (argc > 1) {
      rv = sscanf(
        argv [1],
        "%04" PRIu32 "-%02" PRIu32 "-%02" PRIu32,
        v,
        v + 1,
        v + 2
      );

      if (rv == 3) {
        tod.year = v [0];
        tod.month = v [1];
        tod.day = v [2];
      } else {
        RTEMS_RTC_SHELL_ERROR( "unexpected YYYY-MM-DD input: %s", argv [1]);
      }
    }

    if (argc > 2) {
      rv = sscanf(
        argv [2],
        "%04" PRIu32 ":%02" PRIu32 ":%02" PRIu32,
        v,
        v + 1,
        v + 2
      );

      if (rv == 3) {
        tod.hour = v [0];
        tod.minute = v [1];
        tod.second = v [2];
      } else {
        RTEMS_RTC_SHELL_ERROR( "unexpected HH:MM:SS input: %s", argv [2]);
      }
    }

    if (argc > 3) {
      rv = sscanf( argv [3], "%5" PRIu32, v);

      if (rv == 1) {
        tod.ticks = v [0];
      } else {
        RTEMS_RTC_SHELL_ERROR( "unexpected TICKS input: %s", argv [3]);
      }
    }

    sc = rtems_clock_set( &tod);
    RTEMS_RTC_SHELL_ERROR_SC( sc, "set time of day");

    fd = open( RTC_DEVICE_NAME, O_WRONLY);
    if (fd < 0) {
      perror( "error: open " RTC_DEVICE_NAME);
      return -1;
    }

    n = write( fd, &tod, sizeof( tod));
    if (n != (ssize_t) sizeof( tod)) {
      perror( "error: write to " RTC_DEVICE_NAME);
      close( fd);
      return -1;
    }

    rv = close( fd);
    if (rv != 0) {
      perror( "error: close " RTC_DEVICE_NAME);
      return -1;
    }
  } else {
    puts( rtems_rtc_shell_usage);
    return -1;
  }

  return 0;
}

struct rtems_shell_cmd_tt rtems_shell_RTC_Command = {
  .name = "rtc",
  .usage = rtems_rtc_shell_usage,
  .topic = "misc",
  .command = rtems_rtc_shell_main,
  .alias = NULL,
  .next = NULL
};
