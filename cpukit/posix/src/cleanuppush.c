/**
 * @file
 *
 * @brief POSIX Cleanup Support
 * @ingroup POSIXAPI
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
