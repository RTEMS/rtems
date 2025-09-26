/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief This source file contains the definition of ::_User_extensions_List
 *   and the implementation of _User_extensions_Fatal_visitor(),
 *   _User_extensions_Iterate(), _User_extensions_Thread_begin_visitor(),
 *   _User_extensions_Thread_create_visitor(),
 *   _User_extensions_Thread_delete_visitor(),
 *   _User_extensions_Thread_exitted_visitor(),
 *   _User_extensions_Thread_restart_visitor(),
 *   _User_extensions_Thread_start_visitor(), and
 *   _User_extensions_Thread_terminate_visitor().
 */

/*
 * Copyright (C) 2012, 2019 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/userextimpl.h>

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

  (void) arg;

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
  (void) arg;

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
  (void) executing;

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
  (void) arg;

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

  initial_begin = _User_extensions_Initial_extensions;
  initial_end = initial_begin + _User_extensions_Initial_count;

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

  /*
   * Ignore this warning:
   *
   * - storing the address of local variable 'iter' ... [-Wdangling-pointer=]
   *
   * We store the local variable &iter.Iterator here on the
   * &User_extensions_List.Iterators, and then remove it from the list below
   * at the call to _Chain_Iterator_destroy(). So it is not left dangling.
   */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdangling-pointer="
  _Chain_Iterator_initialize(
    &_User_extensions_List.Active,
    &_User_extensions_List.Iterators,
    &iter.Iterator,
    direction
  );
  #pragma GCC diagnostic pop

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
