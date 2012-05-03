/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#ifdef RTEMS_NETWORKING
  #include <rtems/rtems_bsdnet.h>
#endif

/*
 * This routine calls main from a confdefs.h default Init task
 * set up. The bootcard will provide the task argument as
 * command line string (ASCIIZ).
 */

int main (int argc, char* argv[]);

void Init (rtems_task_argument arg)
{
  const char* boot_cmdline = *((const char**) arg);
  char*       cmdline = 0;
  char*       command;
  int         argc = 0;
  char**      argv = NULL;
  int         result = -124;

  if (boot_cmdline)
  {
    cmdline = malloc (strlen (boot_cmdline) + 1);

    if (cmdline)
    {
      strcpy (cmdline, boot_cmdline);

      command = cmdline;

      /*
       * Break the line up into arguments with "" being ignored.
       */
      while (true)
      {
        command = strtok (command, " \t\r\n");
        if (command == NULL)
          break;
        argc++;
        command = '\0';
      }

      argv = calloc (argc, sizeof (char*));

      if (argv)
      {
        int a;

        command = cmdline;
        argv[0] = command;

        for (a = 1; a < argc; a++)
        {
          command += strlen (command) + 1;
          argv[a] = command;
        }
      }
      else
        argc = 0;
    }
  }

#ifdef RTEMS_NETWORKING
  rtems_bsdnet_initialize_network ();
#endif

  result = main (argc, argv);

  free (argv);
  free (cmdline);

  exit (result);
}

/*
 * By making this a weak alias and a user can provide there own.
 */

void Init (rtems_task_argument arg) __attribute__ ((weak));
