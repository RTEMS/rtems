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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/userextimpl.h>
#include <rtems/score/percpu.h>

void _User_extensions_Add_set(
  User_extensions_Control *the_extension
)
{
  ISR_lock_Context lock_context;

  _User_extensions_Acquire( &lock_context );
  _Chain_Initialize_node( &the_extension->Node );
  _Chain_Append_unprotected(
    &_User_extensions_List.Active,
    &the_extension->Node
  );
  _User_extensions_Release( &lock_context );

  /*
   * If a switch handler is present, append it to the switch chain.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    the_extension->Switch.thread_switch =
      the_extension->Callouts.thread_switch;

    _Per_CPU_Acquire_all( &lock_context );
    _Chain_Initialize_node( &the_extension->Switch.Node );
    _Chain_Append_unprotected(
      &_User_extensions_Switches_list,
      &the_extension->Switch.Node
    );
    _Per_CPU_Release_all( &lock_context );
  }
}
