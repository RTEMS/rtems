/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/userextimpl.h>
#include <rtems/score/percpu.h>

void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
)
{
  ISR_lock_Context lock_context;

  _User_extensions_Acquire( &lock_context );
  _Chain_Iterator_registry_update(
    &_User_extensions_List.Iterators,
    &the_extension->Node
  );
  _Chain_Extract_unprotected( &the_extension->Node );
  _User_extensions_Release( &lock_context );

  /*
   * If a switch handler is present, remove it.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    ISR_lock_Context lock_context;

    _Per_CPU_Acquire_all( &lock_context );
    _Chain_Extract_unprotected( &the_extension->Switch.Node );
    _Per_CPU_Release_all( &lock_context );
  }
}
