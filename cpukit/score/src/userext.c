/**
 * @file
 *
 * @ingroup ScoreUserExt
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

CHAIN_DEFINE_EMPTY( _User_extensions_Switches_list );

typedef struct {
  User_extensions_Switch_control *switch_control;
} User_extensions_Switch_context;

static void _User_extensions_Switch_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_switch_extension callout = callouts->thread_switch;

  if ( callout != NULL ) {
    User_extensions_Switch_context *ctx = arg;
    User_extensions_Switch_control *ctrl = ctx->switch_control;

    _Chain_Append_unprotected( &_User_extensions_Switches_list, &ctrl->Node );
    ctrl->thread_switch = callout;

    ctx->switch_control = ctrl + 1;
  }
}

void _User_extensions_Handler_initialization(void)
{
  uint32_t number_of_initial_extensions =
    rtems_configuration_get_number_of_initial_extensions();

  if ( number_of_initial_extensions > 0 ) {
    User_extensions_Switch_control *initial_extension_switch_controls =
      _Workspace_Allocate_or_fatal_error(
        number_of_initial_extensions
          * sizeof( *initial_extension_switch_controls )
      );
    User_extensions_Switch_context ctx = { initial_extension_switch_controls };

    _User_extensions_Iterate( &ctx, _User_extensions_Switch_visitor );
  }
}
