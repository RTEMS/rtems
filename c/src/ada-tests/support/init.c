/* 
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <bsp.h>

#ifdef PROVIDES_GETPID
#include <unistd.h>
pid_t getpid()
{
#ifndef PID
  return 1;
#else
  return PID;
#endif
}
#endif

void *POSIX_Init(
  void *argument
)
{
  extern int gnat_main ( int argc, char **argv, char **envp );

  (void) gnat_main ( 0, 0, 0 );

  exit( 0 );
}

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS              20
#define CONFIGURE_MAXIMUM_POSIX_KEYS                 20
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              30
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  20

#define CONFIGURE_INIT

#include <confdefs.h>

