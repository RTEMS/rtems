/*
 *  Default configuration file
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <rtems.h>

int main( int argc, char **argv );

static void Init( rtems_task_argument arg )
{
  const char *boot_cmdline = *((const char **) arg);
  char       *cmdline = NULL;
  int         argc = 0;
  char      **argv = NULL;
  int         result;

  if ( boot_cmdline != NULL ) {
    size_t n = strlen( boot_cmdline ) + 1;

    cmdline = malloc( n );
    if ( cmdline != NULL ) {
      char* command;

      memcpy( cmdline, boot_cmdline, n);

      command = cmdline;

      /*
       * Break the line up into arguments with "" being ignored.
       */
      while ( true ) {
        command = strtok( command, " \t\r\n" );
        if ( command == NULL )
          break;

        ++argc;
        command = '\0';
      }

      /*
       * If there are arguments, allocate enough memory for the argv
       * array to be passed into main().
       *
       * NOTE: If argc is 0, then argv will be NULL.
       */
      argv = calloc( argc, sizeof( *argv ) );
      if ( argv != NULL ) {
        int a;

        command = cmdline;
        argv[ 0 ] = command;

        for ( a = 1; a < argc; ++a ) {
          command += strlen( command ) + 1;
          argv[ a ] = command;
        }
      } else {
        argc = 0;
      }
    }
  }

  result = main( argc, argv );

  free( argv );
  free( cmdline );

  exit( result );
}

/* configuration information */

/* This is enough to get a basic main() up. */
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_STACK_CHECKER_ENABLED

/* on smaller architectures lower the number or resources */
#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 8
#define CONFIGURE_MAXIMUM_DRIVERS 16
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 32

/* Include basic device drivers needed to call delays */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_MAXIMUM_PROCESSORS

#define CONFIGURE_DISABLE_BSP_SETTINGS

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
