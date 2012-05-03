/*
 *  Scheduler Simple Handler / Initialize
 *  Scheduler Simple Handler / Allocate (Empty Routine)
 *  Scheduler Simple Handler / Update (Empty Routine)
 *  Scheduler Simple Handler / Free (Empty Routine)
 *
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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/chain.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulersimple.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

/**
 * This routine does nothing, and is used as a stub for Schedule allocate
 *
 * Note: returns a non-zero value, or else thread initialize thinks the
 * allocation failed.
 *
 * The overhead of a function call will still be imposed.
 */
void * _Scheduler_simple_Allocate(
  Thread_Control *the_thread
)
{
  return (void*)-1; /* maybe pick an appropriate poison value */
}


/**
 * This routine does nothing, and is used as a stub for Schedule update
 *
 * The overhead of a function call will still be imposed.
 */
void _Scheduler_simple_Update(
  Thread_Control *the_thread
)
{
}

/**
 * This routine does nothing, and is used as a stub for Schedule free
 *
 * The overhead of a function call will still be imposed.
 */
void _Scheduler_simple_Free(
  Thread_Control *the_thread
)
{
}

/**
 * This routine initializes the simple scheduler.
 */
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
