/*
 *  signal(2) - Install signal handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>
#include <errno.h>

#ifndef HAVE_SIGHANDLER_T
  typedef void (*sighandler_t)(int);
#endif

sighandler_t signal(
  int           signum,
  sighandler_t  handler
)
{
  struct sigaction s;
  struct sigaction old;

  s.sa_handler = handler ;
  sigemptyset(&s.sa_mask);

  /*
   *  Depending on which system we want to behave like, one of
   *  the following versions should be chosen.
   */

/* #define signal_like_linux */

#if defined(signal_like_linux)
  s.sa_flags   = SA_RESTART | SA_INTERRUPT | SA_NOMASK;
  s.sa_restorer= NULL ;
#elif defined(signal_like_SVR4)
  s.sa_flags   = SA_RESTART;
#else
  s.sa_flags   = 0;
#endif

  sigaction( signum, &s, &old );
  return (sighandler_t) old.sa_handler;
}
