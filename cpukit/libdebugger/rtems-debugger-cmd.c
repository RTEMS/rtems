/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#define __need_getopt_newlib
#include <getopt.h>

#include <rtems.h>
#include <rtems/printer.h>
#include <rtems/shell.h>

#include <rtems/rtems-debugger.h>

/*
 * Debugger command for the RTEMS shell.
 */

static int rtems_shell_main_debugger(int argc, char *argv[])
{
  if (argc == 1) {
    printf("RTEMS debugger is %srunning\n", rtems_debugger_running() ? "" : "not ");
    return 0;
  }

  if (strcasecmp(argv[1], "start") == 0) {
    rtems_printer       printer;
    const char*         remote = "tcp";
    const char*         device = "1122";
    int                 timeout = RTEMS_DEBUGGER_TIMEOUT;
    rtems_task_priority priority = 1;
    bool                verbose = false;
    struct getopt_data  data;
    char*               end;
    int                 r;

    if (rtems_debugger_running()) {
      printf("error: debugger already running.\n");
      return 1;
    }

    memset(&data, 0, sizeof(data));

    rtems_print_printer_fprintf(&printer, stdout);

    argv += 1;
    argc -= 1;

    while (true) {
      int c;

      c = getopt_r(argc, argv, "vR:d:t:P:l:", &data);
      if (c == -1)
        break;

      switch (c) {
      case 'v':
        verbose = true;
        break;
      case 'R':
        remote = data.optarg;
        break;
      case 'd':
        device = data.optarg;
        break;
      case 't':
        timeout = strtoul(data.optarg, &end, 10);
        if (timeout == 0 || *end != '\0') {
          printf("error: invalid timeout: %s\n", data.optarg);
          return 1;
        }
        break;
      case 'P':
        priority = strtoul(data.optarg, &end, 10);
        if (priority == 0 || *end != '\0') {
          printf("error: invalid priority: %s\n", data.optarg);
          return 1;
        }
        break;
      case 'l':
        if (strcasecmp(data.optarg, "stdout") == 0)
          rtems_print_printer_fprintf(&printer, stdout);
        else if (strcasecmp(data.optarg, "stderr") == 0)
          rtems_print_printer_fprintf(&printer, stderr);
        else if (strcasecmp(data.optarg, "kernel") == 0)
          rtems_print_printer_printk(&printer);
        else {
          printf("error: unknown printer (stdout, stderr, kernel): %s\n", data.optarg);
          return 1;
        }
        break;
      default:
      case '?':
        if (data.optarg != NULL)
          printf("error: unknown option: %s\n", data.optarg);
        else
          printf("error: invalid start command\n");
        return 1;
      }
    }

    printf("RTEMS Debugger start: remote=%s device=%s priority=%" PRIu32 "\n",
           remote, device, priority);

    r = rtems_debugger_start(remote, device, timeout, priority, &printer);
    if (r < 0) {
      printf("debugger start failed\n");
      return 1;
    }

    rtems_debugger_set_verbose(verbose);
  }
  else if (strcasecmp(argv[1], "stop") == 0) {
    int r;

    if (!rtems_debugger_running()) {
      printf("error: debugger not running.\n");
      return 1;
    }

    r = rtems_debugger_stop();
    if (r < 0) {
      printf("debugger stop failed\n");
      return 1;
    }
  }
  else if (strcasecmp(argv[1], "remote-debug") == 0) {
    int r;

    if (!rtems_debugger_running()) {
      printf("error: debugger not running.\n");
      return 1;
    }

    if (argc == 3 && strcasecmp(argv[2], "on") == 0) {
      r = rtems_debugger_remote_debug(true);
      if (r < 0) {
        printf("debugger remote-debug on failed\n");
        return 1;
      }
    }
    else if (argc == 3 && strcasecmp(argv[2], "off") == 0) {
      r = rtems_debugger_remote_debug(false);
      if (r < 0) {
        printf("debugger remote-debug off failed\n");
        return 1;
      }
    }
    else {
      printf("debugger remote-debug: not on or off\n");
      return 1;
    }
  }
  else if (strcasecmp(argv[1], "verbose") == 0) {
    if (!rtems_debugger_running()) {
      printf("error: debugger not running.\n");
      return 1;
    }

    if (argc == 3 && strcasecmp(argv[2], "on") == 0) {
      rtems_debugger_set_verbose(true);
    }
    else if (argc == 3 && strcasecmp(argv[2], "off") == 0) {
      rtems_debugger_set_verbose(false);
    }
    else {
      printf("debugger verbose: not on or off\n");
      return 1;
    }
  }
  else if (strcasecmp(argv[1], "help") == 0) {
    printf("debugger [start/stop/help] ...\n" \
           "  start -v -R remote -d device -t secs -P priority -l [stdout,stderr,kernel]\n" \
           "  stop\n" \
           "  remote-debug <on/off>\n" \
           "  verbose <on/off>\n" \
           "  help\n");
  }
  else {
    printf("error: unknown command: %s\n", argv[1]);
    return 1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_DEBUGGER_Command = {
  "debugger",                               /* name */
  "debugger [start/stop] [options ...]",    /* usage */
  "misc",                                   /* topic */
  rtems_shell_main_debugger,                /* command */
  NULL,                                     /* alias */
  NULL,                                     /* next */
  0755,
  0,
  0
};
