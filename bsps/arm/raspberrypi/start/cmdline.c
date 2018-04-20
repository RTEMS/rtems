/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief mailbox support.
 */
/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <bsp.h>
#include <bsp/vc.h>

#include <string.h>

#define MAX_CMDLINE_LENGTH 1024
static int rpi_cmdline_ready = -1;
static char rpi_cmdline_cached[MAX_CMDLINE_LENGTH] = "force .data placement";
static bcm2835_get_cmdline_entries rpi_cmdline_entries;

const char *rpi_cmdline_get_raw(void)
{
  memset(&rpi_cmdline_entries, 0, sizeof(rpi_cmdline_entries));
  if (bcm2835_mailbox_get_cmdline(&rpi_cmdline_entries) < 0)
     return NULL;
  return rpi_cmdline_entries.cmdline;
}

const char *rpi_cmdline_get_cached(void)
{
  if (rpi_cmdline_ready <= 0) {
    const char *line = rpi_cmdline_get_raw();
    if (line != NULL)
      strncpy(rpi_cmdline_cached, line, MAX_CMDLINE_LENGTH - 1);
    rpi_cmdline_cached[MAX_CMDLINE_LENGTH - 1] = 0;
    rpi_cmdline_ready = 1;
  }
  return rpi_cmdline_cached;
}

const char *rpi_cmdline_get_arg(const char* arg)
{
  const char *opt_data;
  opt_data = strstr(rpi_cmdline_get_cached(), arg);
  if (opt_data)
    opt_data += strlen(arg);
  return opt_data;
}
