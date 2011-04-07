/*
 *  Task Create Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __need_getopt_newlib
#include <newlib/getopt.h>

#include <stdio.h>

#include <rtems.h>
#include "shell.h"
#include <rtems/stringto.h>
#include <schedsim_shell.h>
#include <rtems/error.h>

int rtems_shell_main_semaphore_create(
  int   argc,
  char *argv[]
)
{
  char                 name[5];
  rtems_id             id;
  rtems_status_code    status;
  long                 tmp;
  rtems_task_priority  ceiling;
  rtems_attribute      attr;
  struct getopt_data   getopt_reent;
  char                 option;
  int                  value;

  CHECK_RTEMS_IS_UP();

  ceiling = 0;
  attr    = RTEMS_DEFAULT_ATTRIBUTES;
  value   = 0;

  memset(&getopt_reent, 0, sizeof(getopt_data));
  while ( (option = getopt_r( argc, argv, "bcsfpiC:V:", &getopt_reent)) != -1 ) {
    switch (option) {
       case 'b': attr |= RTEMS_BINARY_SEMAPHORE;   break;
       case 'c': attr |= RTEMS_COUNTING_SEMAPHORE; break;
       case 's': attr |= RTEMS_SIMPLE_BINARY_SEMAPHORE; break;

       case 'f': attr |= RTEMS_FIFO; break;
       case 'p': attr |= RTEMS_PRIORITY; break;

       case 'i': attr |= RTEMS_INHERIT_PRIORITY; break;
       case 'C':
         attr |= RTEMS_PRIORITY_CEILING;
         if ( rtems_string_to_long(getopt_reent.optarg, &tmp, NULL, 0) ) {
           printf( "Ceiling argument (%s) is not a number\n", argv[1] );
           return -1;
         }
         ceiling = tmp;
         break;

       case 'V':
         if ( rtems_string_to_long(getopt_reent.optarg, &tmp, NULL, 0) ) {
           printf( "Ceiling argument (%s) is not a number\n", argv[1] );
           return -1;
         }
         value = tmp;
         break;

       default:
         fprintf( stderr, "%s: Usage [-bcsfpiC:V:] name\n", argv[0] );
         return -1;
     }
  }

  if ( getopt_reent.optind >= argc ) {
    fprintf( stderr, "No name specified\n" );
    return -1;
  }

  /*
   *  Now create the semaphore
   */
  memset( name, '\0', sizeof(name) );
  strncpy( name, argv[getopt_reent.optind], 4 );

  status = rtems_semaphore_create(
    rtems_build_name( name[0], name[1], name[2], name[3] ),
    value,
    attr,
    ceiling,
    &id
  );
  if ( status ) {
    fprintf(
      stderr,
      "Semaphore create(%s) returned %s\n",
      argv[1],
      rtems_status_text( status )
    ); 
    return -1;
  }

  printf( "Semaphore (%s) created: id=0x%08x\n", argv[1], id );
  
  return 0;
}

rtems_shell_cmd_t rtems_shell_SEMAPHORE_CREATE_Command = {
  "semaphore_create",                 /* name */
  "semaphore_create name priority",   /* usage */
  "rtems",                            /* topic */
  rtems_shell_main_semaphore_create,  /* command */
  NULL,                               /* alias */
  NULL                                /* next */
};
