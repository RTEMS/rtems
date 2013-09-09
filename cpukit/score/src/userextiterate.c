/**
 * @file
 *
 * @brief User Extension Iteration Helpers
 *
 * @ingroup ScoreUserExt
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/score/userextimpl.h>

CHAIN_DEFINE_EMPTY( _User_extensions_List );

void _User_extensions_Thread_create_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_create_extension callout = callouts->thread_create;

  if ( callout != NULL ) {
    User_extensions_Thread_create_context *ctx = arg;

    ctx->ok = ctx->ok && (*callout)( executing, ctx->created );
  }
}

void _User_extensions_Thread_delete_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_delete_extension callout = callouts->thread_delete;

  if ( callout != NULL ) {
    (*callout)( executing, arg );
  }
}

void _User_extensions_Thread_start_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_start_extension callout = callouts->thread_start;

  if ( callout != NULL ) {
    (*callout)( executing, arg );
  }
}

void _User_extensions_Thread_restart_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_restart_extension callout = callouts->thread_restart;

  if ( callout != NULL ) {
    (*callout)( executing, arg );
  }
}

void _User_extensions_Thread_begin_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_begin_extension callout = callouts->thread_begin;

  if ( callout != NULL ) {
    (*callout)( executing );
  }
}

void _User_extensions_Thread_exitted_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_exitted_extension callout = callouts->thread_exitted;

  if ( callout != NULL ) {
    (*callout)( executing );
  }
}

void _User_extensions_Fatal_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_fatal_extension callout = callouts->fatal;

  if ( callout != NULL ) {
    const User_extensions_Fatal_context *ctx = arg;

    (*callout)( ctx->source, ctx->is_internal, ctx->error );
  }
}

void _User_extensions_Iterate(
  void                    *arg,
  User_extensions_Visitor  visitor
)
{
  Thread_Control *executing = _Thread_Get_executing();
  const User_extensions_Table *callouts_current =
    rtems_configuration_get_user_extension_table();
  const User_extensions_Table *callouts_end =
    callouts_current + rtems_configuration_get_number_of_initial_extensions();
  const Chain_Node *node;
  const Chain_Node *tail;

  while ( callouts_current != callouts_end ) {
    (*visitor)( executing, arg, callouts_current );

    ++callouts_current;
  }

  node = _Chain_Immutable_first( &_User_extensions_List );
  tail = _Chain_Immutable_tail( &_User_extensions_List );
  while ( node != tail ) {
    const User_extensions_Control *extension =
      (const User_extensions_Control *) node;

    (*visitor)( executing, arg, &extension->Callouts );

    node = _Chain_Immutable_next( node );
  }
}
