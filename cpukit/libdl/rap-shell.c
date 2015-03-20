/*
 *  COPYRIGHT (c) 2013 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Application Loader.
 *
 * Shell command wrappers for the RTEMS Application loader.
 */

#if HAVE_CONFIG_H
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

