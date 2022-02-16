/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief POSIX Cleanup Support
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#include <pthread.h>

#include <rtems/sysinit.h>
#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/userextimpl.h>

void _pthread_cleanup_push(
  struct _pthread_cleanup_context   *context,
  void                            ( *routine )( void * ),
  void                              *arg
)
{
  ISR_Level       level;
  Thread_Control *executing;

  context->_routine = routine;
  context->_arg = arg;

  /* This value is unused, just provide a deterministic value */
  context->_canceltype = -1;

  _ISR_Local_disable( level );

  executing = _Thread_Executing;
  context->_previous = executing->last_cleanup_context;
  executing->last_cleanup_context = context;

  _ISR_Local_enable( level );
}

void _pthread_cleanup_pop(
  struct _pthread_cleanup_context *context,
  int                              execute
)
{
  ISR_Level       level;
  Thread_Control *executing;

  if ( execute != 0 ) {
    ( *context->_routine )( context->_arg );
  }

  _ISR_Local_disable( level );

  executing = _Thread_Executing;
  executing->last_cleanup_context = context->_previous;

  _ISR_Local_enable( level );
}

static void _POSIX_Cleanup_terminate_extension( Thread_Control *the_thread )
{
  struct _pthread_cleanup_context *context;

  context = the_thread->last_cleanup_context;
  the_thread->last_cleanup_context = NULL;

  while ( context != NULL ) {
    ( *context->_routine )( context->_arg );

    context = context->_previous;
  }
}

static void _POSIX_Cleanup_restart_extension(
  Thread_Control *executing,
  Thread_Control *the_thread
)
{
  (void) executing;
  _POSIX_Cleanup_terminate_extension( the_thread );
}

static User_extensions_Control _POSIX_Cleanup_extensions = {
  .Callouts = {
    .thread_restart = _POSIX_Cleanup_restart_extension,
    .thread_terminate = _POSIX_Cleanup_terminate_extension
  }
};

static void _POSIX_Cleanup_initialization( void )
{
  _User_extensions_Add_API_set( &_POSIX_Cleanup_extensions );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Cleanup_initialization,
  RTEMS_SYSINIT_POSIX_CLEANUP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
