/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief User Extension Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

void _User_extensions_Handler_initialization(void)
{
  const User_extensions_Table    *initial_table;
  User_extensions_Switch_control *initial_switch_controls;
  size_t                          n;
  size_t                          i;

  initial_table = _User_extensions_Initial_extensions;
  initial_switch_controls = _User_extensions_Initial_switch_controls;
  n = _User_extensions_Initial_count;

  for ( i = 0 ; i < n ; ++i ) {
    User_extensions_thread_switch_extension callout;

    callout = initial_table[ i ].thread_switch;

    if ( callout != NULL ) {
      User_extensions_Switch_control *c;

      c = &initial_switch_controls[ i ];
      c->thread_switch = callout;
      _Chain_Initialize_node( &c->Node );
      _Chain_Append_unprotected( &_User_extensions_Switches_list, &c->Node );
    }
  }
}
