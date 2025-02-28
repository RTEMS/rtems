/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief blkstats Shell Command Implementation
 */

/*
 * Copyright (C) 2012 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/blkdev.h>
#include <rtems/printer.h>
#include <rtems/shellconfig.h>

#include <string.h>

static bool is_reset_option(const char *opt)
{
  return strcmp(opt, "-r") == 0 || strcmp(opt, "--reset") == 0;
}

static int rtems_shell_main_blkstats(int argc, char **argv)
{
  bool ok = false;
  bool reset = false;
  const char *device;
  rtems_printer printer;

  if (argc == 2) {
    ok = true;
    device = argv [1];
  } else if (argc == 3 && is_reset_option(argv [1])) {
    ok = true;
    reset = true;
    device = argv [2];
  }

  rtems_print_printer_printf(&printer);

  if (ok) {
    rtems_blkstats(&printer, device, reset);
  } else {
    rtems_printf(&printer, "usage: %s\n", rtems_shell_BLKSTATS_Command.usage);
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_BLKSTATS_Command = {
  .name = "blkstats",
  .usage = "blkstats [-r|--reset] PATH_TO_DEVICE",
  .topic = "files",
  .command = rtems_shell_main_blkstats
};
