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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>

void _User_extensions_Handler_initialization(void)
{
  User_extensions_Switch_control *initial_extension_switch_controls;
  const User_extensions_Table    *initial_table;
  uint32_t                        n;
  uint32_t                        i;

  n = rtems_configuration_get_number_of_initial_extensions();

  initial_extension_switch_controls = _Workspace_Allocate_or_fatal_error(
    n * sizeof( *initial_extension_switch_controls )
  );

  initial_table = rtems_configuration_get_user_extension_table();

  for ( i = 0 ; i < n ; ++i ) {
    User_extensions_thread_switch_extension callout;

    callout = initial_table[ i ].thread_switch;

    if ( callout != NULL ) {
      User_extensions_Switch_control *c;

      c = &initial_extension_switch_controls[ i ];
      c->thread_switch = callout;
      _Chain_Initialize_node( &c->Node );
      _Chain_Append_unprotected( &_User_extensions_Switches_list, &c->Node );
    }
  }
}
