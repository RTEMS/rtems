/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Application Loader.
 *
 * Shell command wrappers for the RTEMS Application loader.
 */

/*
 *  COPYRIGHT (c) 2013 Chris Johns <chrisj@rtems.org>
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/rtl/rap.h>
#include <rtems/rtl/rap-shell.h>

static void
shell_rap_command_help (void)
{
  printf ("usage: rap [cmd] [arg]\n" \
          "Commands and options:\n" \
          "ls:   List the loaded applications (also list)\n" \
          "ld:   Load an application (also load)\n" \
          "un:   Unload an application (also unload)\n");
}

static void
shell_rap_get_error (const char* what)
{
  char message[64];
  int  error;
  error = rtems_rap_get_error (message, sizeof (message));
  printf ("error: %s: (%d) %s\n", what, error, message);
}

static bool
shell_rap_list_handler (void* handle)
{
  printf (" %-10p %-10p %-s\n",
          handle, rtems_rap_dl_handle (handle), rtems_rap_name (handle));
  return true;
}

static int
shell_rap_list (int argc, char* argv[])
{
  printf (" App        DL Handle  Name\n");
  return rtems_rap_iterate (shell_rap_list_handler) ? 0 : 1;
}

static int
shell_rap_load (int argc, char* argv[])
{
  int r = 0;
  if (argc == 0)
  {
    printf ("error: no application name\n");
    return 0;
  }
  if (rtems_rap_load (argv[0], 0, argc - 1, (const char**) (argv + 1)))
    printf ("%s loaded\n", argv[0]);
  else
  {
    r = 1;
    shell_rap_get_error ("loading");
  }
  return r;
}

int
shell_rap (int argc, char* argv[])
{
  if (argc == 1)
  {
    shell_rap_command_help ();
    return 0;
  }

  if ((strcmp (argv[1], "ls") == 0) ||
      (strcmp (argv[1], "list") == 0))
  {
    return shell_rap_list (argc - 2, argv + 2);
  }
  else if ((strcmp (argv[1], "ld") == 0) ||
           (strcmp (argv[1], "load") == 0))
  {
    return shell_rap_load (argc - 2, argv + 2);
  }

  printf ("error: invalid command: %s\n", argv[1]);
  return 0;
}

