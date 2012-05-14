/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_USEREXT_H
#define _RTEMS_SCORE_USEREXT_H

#include <rtems/score/interr.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void User_extensions_routine RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 * @defgroup ScoreUserExt User Extension Handler
 *
 * @ingroup Score
 *
 * @brief The User Extension Handler provides invocation of application
 * dependent routines at critical points in the life of each thread and the
 * system as a whole.
 *
 * @{
 */

/**
 * @brief Task create extension.
 *
 * It corresponds to _Thread_Initialize() (used by the rtems_task_create()
 * directive).  The first parameter points to the currently executing thread
 * which created the new thread.  The second parameter points to the created
 * thread.
 *
 * It is invoked after the new thread has been completely initialized, but
 * before it is placed on a ready chain.
 *
 * Thread dispatching may be disabled.  This depends on the context of the
 * _Thread_Initialize() call.  Thread dispatch is disabled during the creation
 * of the idle thread and the initialization threads.  It can be considered as
 * an invalid API usage, if the application calls _Thread_Initialize() with
 * disabled thread dispatching.  Disabled thread dispatching is different from
 * disabled preemption.
 *
 * It can be assumed that the executing thread locked the allocator mutex.
 * The only exception is the creation of the idle thread.  In this case the
 * allocator mutex is not locked.  Since the allocator mutex is non-recursive,
 * it is prohibited to call the normal memory allocation routines.  It is
 * possible to use internal rountines like _Workspace_Allocate() or
 * _Heap_Allocate() for heaps which are protected by the allocator mutex.
 *
 * @retval true The thread create extension was successful.
 * @retval false A thread create user extension will frequently attempt to
 * allocate resources.  If this allocation fails, then the extension should
 * return @a false and the entire thread create operation will fail.
 */
typedef bool ( *User_extensions_thread_create_extension )(
  Thread_Control *,
  Thread_Control *
);

/**
 * @brief Task delete extension.
 *
 * It corresponds to _Thread_Close() (used by the rtems_task_delete()
 * directive).  The first parameter points to the currently executing thread
 * which deleted the thread.  The second parameter points to the deleted
 * thread.
 *
 * It is invoked before all resources of the thread are deleted.
 *
 * Thread dispatching is enabled.  The executing thread locked the allocator
 * mutex.
 */
typedef void( *User_extensions_thread_delete_extension )(
  Thread_Control *,
  Thread_Control *
);

/**
 * @brief Task start extension.
 *
 * It corresponds to _Thread_Start() (used by the rtems_task_start()
 * directive).  The first parameter points to the currently executing thread
 * which started the thread.  The second parameter points to the started
 * thread.
 *
 * It is invoked after the environment of the thread has been loaded and the
 * thread has been made ready.
 *
 * Thread dispatching is disabled.  The executing thread is not the holder of
 * the allocator mutex.
 */
typedef void( *User_extensions_thread_start_extension )(
  Thread_Control *,
  Thread_Control *
);

/**
 * @brief Task restart extension.
 *
 * It corresponds to _Thread_Restart() (used by the rtems_task_restart()
 * directive).  The first parameter points to the currently executing thread
 * which restarted the thread.  The second parameter points to the restarted
 * thread.
 *
 * It is invoked after the environment of the thread has been loaded and the
 * thread has been made ready.
 *
 * Thread dispatching is disabled.  The executing thread is not the holder of
 * the allocator mutex.
 */
typedef void( *User_extensions_thread_restart_extension )(
  Thread_Control *,
  Thread_Control *
);

/**
 * @brief Task switch extension.
 *
 * It corresponds to _Thread_Dispatch().  The first parameter points to the
 * currently executing thread.  The second parameter points to the heir thread.
 *
 * It is invoked before the context switch from the executing to the heir
 * thread.
 *
 * Thread dispatching is disabled.  The state of the allocator mutex is
 * arbitrary.
 *
 * The context switches initiated through _Thread_Start_multitasking() and
 * _Thread_Stop_multitasking() are not covered by this extension.  The
 * executing thread may run with a minimal setup, for example with a freed task
 * stack.
 */
typedef void( *User_extensions_thread_switch_extension )(
  Thread_Control *,
  Thread_Control *
);

/**
 * @brief Task begin extension.
 *
 * It corresponds to _Thread_Handler().  The first parameter points to the
 * currently executing thread which begins now execution.
 *
 * Thread dispatching is disabled.  The executing thread is not the holder of
 * the allocator mutex.
 */
typedef void( *User_extensions_thread_begin_extension )(
  Thread_Control *
);

/**
 * @brief Task exitted extension.
 *
 * It corresponds to _Thread_Handler().  The first parameter points to the
 * currently executing thread which exitted before.
 *
 * Thread dispatching is disabled.  The state of the allocator mutex is
 * arbitrary.
 */
typedef void( *User_extensions_thread_exitted_extension )(
  Thread_Control *
);

/**
 * @brief Fatal error extension.
 *
 * It corresponds to _Internal_error_Occurred() (used by the
 * rtems_fatal_error_occurred() directive).  The first parameter contains the
 * error source.  The second parameter indicates if it was an internal error.
 * The third parameter contains the error code.
 *
 * This extension should not call any RTEMS directives.
 */
typedef void( *User_extensions_fatal_extension )(
  Internal_errors_Source,
  bool,
  Internal_errors_t
);

/**
 * @brief User extension table.
 */
typedef struct {
  User_extensions_thread_create_extension  thread_create;
  User_extensions_thread_start_extension   thread_start;
  User_extensions_thread_restart_extension thread_restart;
  User_extensions_thread_delete_extension  thread_delete;
  User_extensions_thread_switch_extension  thread_switch;
  User_extensions_thread_begin_extension   thread_begin;
  User_extensions_thread_exitted_extension thread_exitted;
  User_extensions_fatal_extension          fatal;
}   User_extensions_Table;

/**
 * @brief Manages the switch callouts.
 *
 * They are managed separately from other extensions for performance reasons.
 */
typedef struct {
  Chain_Node                              Node;
  User_extensions_thread_switch_extension thread_switch;
}   User_extensions_Switch_control;

/**
 * @brief Manages each user extension set.
 *
 * The switch control is part of the extensions control even if not used due to
 * the extension not having a switch handler.
 */
typedef struct {
  Chain_Node                     Node;
  User_extensions_Switch_control Switch;
  User_extensions_Table          Callouts;
}   User_extensions_Control;

/**
 * @brief List of active extensions.
 */
SCORE_EXTERN Chain_Control _User_extensions_List;

/**
 * @brief List of active task switch extensions.
 */
SCORE_EXTERN Chain_Control _User_extensions_Switches_list;

/**
 * @name Extension Maintainance
 *
 * @{
 */

void _User_extensions_Handler_initialization( void );

void _User_extensions_Add_set(
  User_extensions_Control *extension
);

RTEMS_INLINE_ROUTINE void _User_extensions_Add_API_set(
  User_extensions_Control *extension
)
{
  _User_extensions_Add_set( extension );
}

RTEMS_INLINE_ROUTINE void _User_extensions_Add_set_with_table(
  User_extensions_Control     *extension,
  const User_extensions_Table *extension_table
)
{
  extension->Callouts = *extension_table;

  _User_extensions_Add_set( extension );
}

void _User_extensions_Remove_set(
  User_extensions_Control *extension
);

/** @} */

/**
 * @name Extension Callout Dispatcher
 *
 * @{
 */

bool _User_extensions_Thread_create(
  Thread_Control *created
);

void _User_extensions_Thread_delete(
  Thread_Control *deleted
);

void _User_extensions_Thread_start(
  Thread_Control *started
);

void _User_extensions_Thread_restart(
  Thread_Control *restarted
);

void _User_extensions_Thread_begin(
  Thread_Control *executing
);

void _User_extensions_Thread_switch(
  Thread_Control *executing,
  Thread_Control *heir
);

void _User_extensions_Thread_exitted(
  Thread_Control *executing
);

void _User_extensions_Fatal(
  Internal_errors_Source source,
  bool                   is_internal,
  Internal_errors_t      error
);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
