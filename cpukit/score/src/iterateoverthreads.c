/**
 *  @file
 *
 *  @brief Iterates Over All Threads
 *  @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

typedef struct {
  rtems_per_thread_routine routine;
} routine_arg;

static bool routine_adaptor( rtems_tcb *tcb, void *arg )
{
  routine_arg *ra;

  ra = arg;
  ( *ra->routine )( tcb );
  return false;
}

void rtems_iterate_over_all_threads( rtems_per_thread_routine routine )
{
  routine_arg arg = {
    .routine = routine
  };

  if ( routine != NULL ) {
    _Thread_Iterate( routine_adaptor, &arg );
  }
}
