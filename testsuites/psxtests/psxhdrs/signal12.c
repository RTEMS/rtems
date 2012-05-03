/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

int test( void );

int test( void )
{
  void (*signal_function_pointer)(int);
  int  signal_number;

  signal_number = SIGALRM;

  signal_function_pointer = signal( signal_number, SIG_IGN );

  return (signal_function_pointer == SIG_ERR) ? -1 : 0;
}
