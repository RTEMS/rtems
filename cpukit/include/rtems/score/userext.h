/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief User Extension Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_USEREXT_H
#define _RTEMS_SCORE_USEREXT_H

#include <rtems/score/interr.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _Thread_Control;

typedef void User_extensions_routine RTEMS_DEPRECATED;

/**
 * @defgroup RTEMSScoreUserExt User Extension Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief The User Extension Handler provides invocation of application
 * dependent routines at critical points in the life of each thread and the
 * system as a whole.
 */
/**@{**/

/**
 * @brief Task create extension.
 *
 * It corresponds to _Thread_Initialize() (used by the rtems_task_create()
 * directive and pthread_create()).
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
 * allocator mutex is not locked.  Since the allocator mutex allows nesting the
 * normal memory allocation routines can be used.
 *
 * @param[in] executing The executing thread.
 * @param[in] created The created thread.
 *
 * @retval true Successful operation.
 * @retval false A thread create user extension will frequently attempt to
 * allocate resources.  If this allocation fails, then the extension should
 * return @a false and the entire thread create operation will fail.
 */
typedef bool ( *User_extensions_thread_create_extension )(
  struct _Thread_Control *executing,
  struct _Thread_Control *created
);

/**
 * @brief Task delete extension.
 *
 * It corresponds to _Thread_Close() (used by the rtems_task_delete()
 * directive, pthread_exit() and pthread_cancel()).
 *
 * It is invoked before all resources of the thread are deleted.  The executing
 * and deleted arguments are never equal.
 *
 * Thread dispatching is enabled.  The executing thread locked the allocator
 * mutex.
 *
 * @param[in] executing The executing thread.
 * @param[in] deleted The deleted thread.
 */
typedef void( *User_extensions_thread_delete_extension )(
  struct _Thread_Control *executing,
  struct _Thread_Control *deleted
);

/**
 * @brief Task start extension.
 *
 * It corresponds to _Thread_Start() (used by the rtems_task_start()
 * directive).
 *
 * It is invoked after the environment of the thread has been loaded and the
 * thread has been made ready.
 *
 * Thread dispatching is disabled.  The executing thread is not the holder of
 * the allocator mutex.
 *
 * @param[in] executing The executing thread.
 * @param[in] started The started thread.
 */
typedef void( *User_extensions_thread_start_extension )(
  struct _Thread_Control *executing,
  struct _Thread_Control *started
);

/**
 * @brief Task restart extension.
 *
 * It corresponds to _Thread_Restart() (used by the rtems_task_restart()
 * directive).
 *
 * It is invoked in the context of the restarted thread right before the
 * execution context is reloaded.  The executing and restarted arguments are
 * always equal.  The thread stack reflects the previous execution context.
 *
 * Thread dispatching is enabled.  The thread is not the holder of the
 * allocator mutex.  The thread life is protected.  Thread restart and delete
 * requests issued by restart extensions lead to recursion.
 *
 * @param[in] executing The executing thread.
 * @param[in] restarted The executing thread.  Yes, the executing thread.
 */
typedef void( *User_extensions_thread_restart_extension )(
  struct _Thread_Control *executing,
  struct _Thread_Control *restarted
);

/**
 * @brief Task switch extension.
 *
 * It corresponds to _Thread_Dispatch().
 *
 * It is invoked before the context switch from the executing to the heir
 * thread.
 *
 * Thread dispatching is disabled.  The state of the allocator mutex is
 * arbitrary.  Interrupts are disabled and the per-CPU lock is acquired on SMP
 * configurations.
 *
 * The context switches initiated through _Thread_Start_multitasking() are not
 * covered by this extension.
 *
 * @param[in] executing The executing thread.
 * @param[in] heir The heir thread.
 */
typedef void( *User_extensions_thread_switch_extension )(
  struct _Thread_Control *executing,
  struct _Thread_Control *heir
);

/**
 * @brief Task begin extension.
 *
 * It corresponds to _Thread_Handler().
 *
 * Thread dispatching is disabled.  The executing thread is not the holder of
 * the allocator mutex.
 *
 * @param[in] executing The executing thread.
 */
typedef void( *User_extensions_thread_begin_extension )(
  struct _Thread_Control *executing
);

/**
 * @brief Task exitted extension.
 *
 * It corresponds to _Thread_Handler() after a return of the entry function.
 *
 * Thread dispatching is disabled.  The state of the allocator mutex is
 * arbitrary.
 *
 * @param[in] executing The executing thread.
 */
typedef void( *User_extensions_thread_exitted_extension )(
  struct _Thread_Control *executing
);

/**
 * @brief Fatal error extension.
 *
 * It corresponds to _Terminate() (used by the rtems_fatal() directive).
 *
 * This extension should not call any RTEMS directives.
 *
 * @param[in] source The fatal source indicating the subsystem the fatal
 * condition originated in.
 * @param[in] always_set_to_false This parameter is always set to false and
 * provided only for backward compatibility reasons.
 * @param[in] code The fatal error code.  This value must be interpreted with
 * respect to the source.
 */
typedef void( *User_extensions_fatal_extension )(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
);

/**
 * @brief Task termination extension.
 *
 * This extension is invoked by _Thread_Life_action_handler() in case a
 * termination request is recognized.
 *
 * It is invoked in the context of the terminated thread right before the
 * thread dispatch to the heir thread.  The POSIX cleanup and key destructors
 * execute in this context.
 *
 * Thread dispatching is enabled.  The thread is not the holder of the
 * allocator mutex.  The thread life is protected.  Thread restart and delete
 * requests issued by terminate extensions lead to recursion.
 *
 * @param[in] terminated The terminated thread.
 */
typedef void( *User_extensions_thread_terminate_extension )(
  struct _Thread_Control *terminated
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
  User_extensions_thread_terminate_extension thread_terminate;
}   User_extensions_Table;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
