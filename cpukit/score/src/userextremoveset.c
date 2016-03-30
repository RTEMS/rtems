/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/userextimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/percpu.h>

void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
)
{
  _Assert( _Objects_Allocator_is_owner() );

  _Chain_Extract_unprotected( &the_extension->Node );

  /*
   * If a switch handler is present, remove it.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    ISR_Level level;

    _Per_CPU_Acquire_all( level );
    _Chain_Extract_unprotected( &the_extension->Switch.Node );
    _Per_CPU_Release_all( level );
  }
}
