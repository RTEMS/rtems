/**
 * @file
 *
 * @brief Scheduler Simple Functions
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/wkspace.h>

void * _Scheduler_simple_Allocate(
  Thread_Control *the_thread
)
{
  return (void*)-1; /* maybe pick an appropriate poison value */
}

void _Scheduler_simple_Update(
  Thread_Control *the_thread
)
{
}

void _Scheduler_simple_Free(
  Thread_Control *the_thread
)
{
}

void _Scheduler_simple_Initialize ( void )
{
  void *f;

  /*
   * Initialize Ready Queue
   */

  /* allocate ready queue structures */
  f = _Workspace_Allocate_or_fatal_error( sizeof(Chain_Control) );
  _Scheduler.information = f;

  /* initialize ready queue structure */
  _Chain_Initialize_empty( (Chain_Control *)f );
}
