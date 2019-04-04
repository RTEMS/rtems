/**
 * @file
 *
 * @brief User Extension Iteration Helpers
 *
 * @ingroup RTEMSScoreUserExt
 */

/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/score/userextimpl.h>

#include <pthread.h>

User_extensions_List _User_extensions_List = {
  CHAIN_INITIALIZER_EMPTY( _User_extensions_List.Active ),
  CHAIN_ITERATOR_REGISTRY_INITIALIZER( _User_extensions_List.Iterators )
#if defined(RTEMS_SMP)
  ,
  ISR_LOCK_INITIALIZER( "User Extensions List" )
#endif
};

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

    (*callout)( ctx->source, false, ctx->error );
  }
}

void _User_extensions_Thread_terminate_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
)
{
  User_extensions_thread_terminate_extension callout =
    callouts->thread_terminate;

  if ( callout != NULL ) {
    (*callout)( executing );
  }
}

void _User_extensions_Iterate(
  void                     *arg,
  User_extensions_Visitor   visitor,
  Chain_Iterator_direction  direction
)
{
  Thread_Control              *executing;
  const User_extensions_Table *initial_current;
  const User_extensions_Table *initial_begin;
  const User_extensions_Table *initial_end;
  const Chain_Node            *end;
  Chain_Node                  *node;
  User_extensions_Iterator     iter;
  ISR_lock_Context             lock_context;

  executing = _Thread_Get_executing();

  initial_begin = rtems_configuration_get_user_extension_table();
  initial_end =
    initial_begin + rtems_configuration_get_number_of_initial_extensions();

  if ( direction == CHAIN_ITERATOR_FORWARD ) {
    initial_current = initial_begin;

    while ( initial_current != initial_end ) {
      (*visitor)( executing, arg, initial_current );
      ++initial_current;
    }

    end = _Chain_Immutable_tail( &_User_extensions_List.Active );
  } else {
    end = _Chain_Immutable_head( &_User_extensions_List.Active );
  }

  _User_extensions_Acquire( &lock_context );

  _Chain_Iterator_initialize(
    &_User_extensions_List.Active,
    &_User_extensions_List.Iterators,
    &iter.Iterator,
    direction
  );

  if ( executing != NULL ) {
    iter.previous = executing->last_user_extensions_iterator;
    executing->last_user_extensions_iterator = &iter;
  }

  while ( ( node = _Chain_Iterator_next( &iter.Iterator ) ) != end ) {
    const User_extensions_Control *extension;

    _Chain_Iterator_set_position( &iter.Iterator, node );

    _User_extensions_Release( &lock_context );

    extension = (const User_extensions_Control *) node;
    ( *visitor )( executing, arg, &extension->Callouts );

    _User_extensions_Acquire( &lock_context );
  }

  if ( executing != NULL ) {
    executing->last_user_extensions_iterator = iter.previous;
  }

  _Chain_Iterator_destroy( &iter.Iterator );

  _User_extensions_Release( &lock_context );

  if ( direction == CHAIN_ITERATOR_BACKWARD ) {
    initial_current = initial_end;

    while ( initial_current != initial_begin ) {
      --initial_current;
      (*visitor)( executing, arg, initial_current );
    }
  }
}
