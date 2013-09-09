/**
 * @file
 *
 * @brief Enable Dispatching of Threads
 *
 * @ingroup ScoreThread
 */

/*
 *   _Thread_Enable_dispatch
 *
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threaddispatch.h>

#if defined (__THREAD_DO_NOT_INLINE_ENABLE_DISPATCH__ )
void _Thread_Enable_dispatch( void )
{
  if ( _Thread_Dispatch_decrement_disable_level() )
    return;
  _Thread_Dispatch();
}
#endif
