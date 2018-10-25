/**
 * @file
 *
 * @brief Private Support Information for POSIX Threads
 * @ingroup POSIX_PTHREADS Private Threads
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <assert.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/score/stack.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#if defined(RTEMS_POSIX_API)
#include <rtems/posix/psignalimpl.h>
#endif
#include <rtems/posix/config.h>
#include <rtems/posix/keyimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

Thread_Information _POSIX_Threads_Information;

#if defined(RTEMS_POSIX_API)
void _POSIX_Threads_Sporadic_timer( Watchdog_Control *watchdog )
{
  POSIX_API_Control    *api;
  Thread_Control       *the_thread;
  Thread_queue_Context  queue_context;

  api = RTEMS_CONTAINER_OF( watchdog, POSIX_API_Control, Sporadic.Timer );
  the_thread = api->Sporadic.thread;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  if ( _Priority_Node_is_active( &api->Sporadic.Low_priority ) ) {
    _Thread_Priority_add(
      the_thread,
      &the_thread->Real_priority,
      &queue_context
    );
    _Thread_Priority_remove(
      the_thread,
      &api->Sporadic.Low_priority,
      &queue_context
    );
    _Priority_Node_set_inactive( &api->Sporadic.Low_priority );
  }

  _Watchdog_Per_CPU_remove_ticks( &api->Sporadic.Timer );
  _POSIX_Threads_Sporadic_timer_insert( the_thread, api );

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
}

void _POSIX_Threads_Sporadic_budget_callout( Thread_Control *the_thread )
{
  POSIX_API_Control    *api;
  Thread_queue_Context  queue_context;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  /*
   *  This will prevent the thread from consuming its entire "budget"
   *  while at low priority.
   */
  the_thread->cpu_time_budget = UINT32_MAX;

  if ( !_Priority_Node_is_active( &api->Sporadic.Low_priority ) ) {
    _Thread_Priority_add(
      the_thread,
      &api->Sporadic.Low_priority,
      &queue_context
    );
    _Thread_Priority_remove(
      the_thread,
      &the_thread->Real_priority,
      &queue_context
    );
  }

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
}

/*
 *  _POSIX_Threads_Create_extension
 *
 *  This method is invoked for each thread created.
 */

static bool _POSIX_Threads_Create_extension(
  Thread_Control *executing RTEMS_UNUSED,
  Thread_Control *created
)
{
  POSIX_API_Control *api;

  api = created->API_Extensions[ THREAD_API_POSIX ];

  api->Sporadic.thread = created;
  _Watchdog_Preinitialize( &api->Sporadic.Timer, _Per_CPU_Get_by_index( 0 ) );
  _Watchdog_Initialize( &api->Sporadic.Timer, _POSIX_Threads_Sporadic_timer );
  _Priority_Node_set_inactive( &api->Sporadic.Low_priority );

  return true;
}

static void _POSIX_Threads_Terminate_extension( Thread_Control *executing )
{
  POSIX_API_Control *api;
  ISR_lock_Context   lock_context;

  api = executing->API_Extensions[ THREAD_API_POSIX ];

  _Thread_State_acquire( executing, &lock_context );
  _Watchdog_Per_CPU_remove_ticks( &api->Sporadic.Timer );
  _Thread_State_release( executing, &lock_context );
}
#endif

/*
 *  _POSIX_Threads_Exitted_extension
 *
 *  This method is invoked each time a thread exits.
 */
static void _POSIX_Threads_Exitted_extension(
  Thread_Control *executing
)
{
  /*
   *  If the executing thread was not created with the POSIX API, then this
   *  API do not get to define its exit behavior.
   */
  if ( _Objects_Get_API( executing->Object.id ) == OBJECTS_POSIX_API )
    pthread_exit( executing->Wait.return_argument );
}

User_extensions_Control _POSIX_Threads_User_extensions = {
  .Callouts = {
#if defined(RTEMS_POSIX_API)
    .thread_create    = _POSIX_Threads_Create_extension,
    .thread_terminate = _POSIX_Threads_Terminate_extension,
#endif
    .thread_exitted   = _POSIX_Threads_Exitted_extension
  }
};

/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  This routine initializes all threads manager related data structures.
 */
static void _POSIX_Threads_Manager_initialization(void)
{
  _Thread_Initialize_information(
    &_POSIX_Threads_Information, /* object information table */
    OBJECTS_POSIX_API,           /* object API */
    OBJECTS_POSIX_THREADS,       /* object class */
    _Configuration_POSIX_Maximum_threads
  );

  /*
   *  Add all the extensions for this API
   */
  _User_extensions_Add_API_set( &_POSIX_Threads_User_extensions );

  /*
   *  If we supported MP, then here we would ...
   *       Register the MP Process Packet routine.
   */
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Threads_Manager_initialization,
  RTEMS_SYSINIT_POSIX_THREADS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
