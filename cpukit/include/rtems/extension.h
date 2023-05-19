/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicUserExt
 *
 * @brief This header file defines the User Extensions Manager API.
 */

/*
 * Copyright (C) 2009, 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/userext/if/header */

#ifndef _RTEMS_EXTENSION_H
#define _RTEMS_EXTENSION_H

#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/interr.h>
#include <rtems/score/userextdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/userext/if/group */

/**
 * @defgroup RTEMSAPIClassicUserExt User Extensions Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The User Extensions Manager allows the application developer to
 *   augment the executive by allowing them to supply extension routines which
 *   are invoked at critical system events.
 */

/* Generated from spec:/rtems/userext/if/delete */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Deletes the extension set.
 *
 * @param id is the extension set identifier.
 *
 * This directive deletes the extension set specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no extension set associated with the
 *   identifier specified by ``id``.
 *
 * @par Notes
 * The ESCB for the deleted extension set is reclaimed by RTEMS.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 * @endparblock
 */
rtems_status_code rtems_extension_delete( rtems_id id );

/* Generated from spec:/rtems/userext/if/fatal */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Fatal extensions are invoked when the system should terminate.
 *
 * @param source is the system termination source.  The source indicates the
 *   component which caused the system termination request, see
 *   ::rtems_fatal_source.  The system termination code may provide additional
 *   information related to the system termination request.
 *
 * @param always_set_to_false is a value equal to false.
 *
 * @param code is the system termination code.  This value must be interpreted
 *   with respect to the source.
 *
 * @par Notes
 * @parblock
 * The fatal extensions are invoked in extension forward order.
 *
 * The fatal extension should be extremely careful with respect to the RTEMS
 * directives it calls.  Depending on the system termination source, the system
 * may be in an undefined and corrupt state.
 *
 * It is recommended to register fatal extensions through initial extension
 * sets, see @ref CONFIGURE_INITIAL_EXTENSIONS.
 * @endparblock
 */
typedef User_extensions_fatal_extension rtems_fatal_extension;

/* Generated from spec:/rtems/userext/if/fatal-code */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief This integer type represents system termination codes.
 *
 * This integer type is large enough to store a 32-bit integer or a pointer.
 *
 * @par Notes
 * The interpretation of a system termination code depends on the system
 * termination source, see ::rtems_fatal_source.
 */
typedef Internal_errors_t rtems_fatal_code;

/* Generated from spec:/rtems/userext/if/fatal-source */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief This enumeration represents system termination sources.
 *
 * @par Notes
 * The system termination code may provide additional information depending on
 * the system termination source, see ::rtems_fatal_code.
 */
typedef Internal_errors_Source rtems_fatal_source;

/* Generated from spec:/rtems/userext/if/ident */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Identifies an extension set by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the object identifier of an object with the specified
 *   name will be stored in this object.
 *
 * This directive obtains an extension set identifier associated with the
 * extension set name specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the extension set name is not unique, then the extension set identifier
 * will match the first extension set with that name in the search order.
 * However, this extension set identifier is not guaranteed to correspond to
 * the desired extension set.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 *
 * The extension set identifier is used with other extension related directives
 * to access the extension set.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_extension_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/userext/if/table */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief The extensions table contains a set of extensions which may be
 *   registered in the system through the @ref CONFIGURE_INITIAL_EXTENSIONS
 *   application configuration option or the rtems_extension_create()
 *   directive.
 */
typedef User_extensions_Table rtems_extensions_table;

/* Generated from spec:/rtems/userext/if/create */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Creates an extension set.
 *
 * @param name is the object name of the extension set.
 *
 * @param extension_table is the table with the extensions to be used by the
 *   extension set.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the created extension set will be
 *   stored in this object.
 *
 * This directive creates an extension set which resides on the local node.
 * The extension set has the user-defined object name specified in ``name``.
 * The assigned object identifier is returned in ``id``.  This identifier is
 * used to access the extension set with other extension set related
 * directives.
 *
 * The extension set is initialized using the extension table specified in
 * ``extension_table``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``extension_table`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create an
 *   extension set.  The number of extension sets available to the application
 *   is configured through the @ref CONFIGURE_MAXIMUM_USER_EXTENSIONS
 *   application configuration option.
 *
 * @par Notes
 * @parblock
 * The user-provided extension table is not used after the return of the
 * directive.
 *
 * Each extension of the extension table is optional and may be NULL.  All
 * extensions except the task switch extension of the extension table are
 * atomically and immediately installed.  A task switch extension is separately
 * installed after the other extensions.  The extensions of the extension table
 * are invoked upon the next system event supporting an extension.
 *
 * An alternative to dynamically created extension sets are initial extensions,
 * see @ref CONFIGURE_INITIAL_EXTENSIONS.  Initial extensions are recommended
 * for extension sets which provide a fatal error extension.
 *
 * For control and maintenance of the extension set, RTEMS allocates a ESCB
 * from the local ESCB free pool and initializes it.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The number of extension sets available to the application is configured
 *   through the @ref CONFIGURE_MAXIMUM_USER_EXTENSIONS application
 *   configuration option.
 * @endparblock
 */
rtems_status_code rtems_extension_create(
  rtems_name                    name,
  const rtems_extensions_table *extension_table,
  rtems_id                     *id
);

/* Generated from spec:/rtems/userext/if/task-begin */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task begin extensions are invoked when a task begins execution.
 *
 * @param executing is the TCB of the executing thread.
 *
 * @par Notes
 * @parblock
 * The task begin extensions are invoked in extension forward order.
 *
 * Task begin extensions are invoked with thread dispatching enabled.  This
 * allows the use of dynamic memory allocation, creation of POSIX keys, and use
 * of C++ thread-local storage.  Blocking synchronization primitives are
 * allowed also.
 *
 * The task begin extensions are invoked before the global construction.
 *
 * The task begin extensions may be called as a result of a task restart
 * through rtems_task_restart().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is enabled.
 *
 * * The executing thread is not the owner of the object allocator mutex.
 * @endparblock
 */
typedef User_extensions_thread_begin_extension rtems_task_begin_extension;

/* Generated from spec:/rtems/userext/if/task-create */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task create extensions are invoked when a task is created.
 *
 * @param executing is the TCB of the executing thread.  When the idle thread
 *   is created, the executing thread is equal to NULL.
 *
 * @param created is the TCB of the created thread.
 *
 * @return Returns true, if the task create extension was successful, otherwise
 *   false.
 *
 * @par Notes
 * @parblock
 * The task create extensions are invoked in extension forward order.
 *
 * The task create extensions are invoked after a new task has been completely
 * initialized, but before it is started.
 *
 * While normal tasks are created, the executing thread is the owner of the
 * object allocator mutex.  The object allocator mutex allows nesting, so the
 * normal memory allocation routines can be used allocate memory for the
 * created thread.
 *
 * If the task create extension returns false, then the task create operation
 * stops immediately and the entire task create operation will fail.  In this
 * case, all task delete extensions are invoked, see
 * ::rtems_task_delete_extension.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * While the system is initialized, thread dispatching is disabled.
 *
 * * While the system is in the multitasking state, thread dispatching is
 *   enabled.
 *
 * * While an idle thread or another internal system thread is created, the
 *   object allocator mutex has no owner.
 *
 * * While a task is created by rtems_task_create(), the executing thread is
 *   the owner of the object allocator mutex.
 *
 * * While a task is constructed by rtems_task_construct(), the executing
 *   thread is the owner of the object allocator mutex.
 *
 * * While a task is created by pthread_create(), the executing thread is the
 *   owner of the object allocator mutex.
 * @endparblock
 */
typedef User_extensions_thread_create_extension rtems_task_create_extension;

/* Generated from spec:/rtems/userext/if/task-delete */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task delete extensions are invoked when a task is deleted.
 *
 * @param executing is the TCB of the executing thread.  If the idle thread is
 *   created and one of the initial task create extension fails, then the
 *   executing thread is equal to NULL.
 *
 * @param created is the TCB of the deleted thread.  The executing and deleted
 *   arguments are never equal.
 *
 * @par Notes
 * @parblock
 * The task delete extensions are invoked in extension reverse order.
 *
 * The task delete extensions are invoked by task create directives before an
 * attempt to allocate a TCB is made.
 *
 * If a task create extension failed, then a task delete extension may be
 * invoked without a previous invocation of the corresponding task create
 * extension of the extension set.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * While the system is initialized, thread dispatching is disabled.
 *
 * * While the system is in the multitasking state, thread dispatching is
 *   enabled.
 *
 * * While an idle thread or another internal system thread is created, the
 *   object allocator mutex has no owner.
 *
 * * While a task is created by rtems_task_create(), the executing thread is
 *   the owner of the object allocator mutex.
 *
 * * While a task is constructed by rtems_task_construct(), the executing
 *   thread is the owner of the object allocator mutex.
 *
 * * While a task is created by pthread_create(), the executing thread is the
 *   owner of the object allocator mutex.
 * @endparblock
 */
typedef User_extensions_thread_delete_extension rtems_task_delete_extension;

/* Generated from spec:/rtems/userext/if/task-exitted */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task exitted extensions are invoked when a task entry returns.
 *
 * @param executing is the TCB of the executing thread.
 *
 * @par Notes
 * The task exitted extensions are invoked in extension forward order.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is enabled.
 * @endparblock
 */
typedef User_extensions_thread_exitted_extension rtems_task_exitted_extension;

/* Generated from spec:/rtems/userext/if/task-restart */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task restart extensions are invoked when a task restarts.
 *
 * @param executing is the TCB of the executing thread.
 *
 * @param restarted is the TCB of the executing thread.  Yes, the executing
 *   thread.
 *
 * @par Notes
 * @parblock
 * The task restart extensions are invoked in extension forward order.
 *
 * The task restart extensions are invoked in the context of the restarted
 * thread right before the execution context is reloaded.  The thread stack
 * reflects the previous execution context.
 *
 * Thread restart and delete requests issued by restart extensions lead to
 * recursion.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is enabled.
 *
 * * Thread life is protected.
 *
 * * The executing thread is not the owner of the object allocator mutex.
 * @endparblock
 */
typedef User_extensions_thread_restart_extension rtems_task_restart_extension;

/* Generated from spec:/rtems/userext/if/task-start */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task start extensions are invoked when a task was made ready for the
 *   first time.
 *
 * @param executing is the TCB of the executing thread.
 *
 * @param started is the TCB of the started thread.
 *
 * @par Notes
 * @parblock
 * The task start extensions are invoked in extension forward order.
 *
 * In SMP configurations, the thread may already run on another processor
 * before the task start extensions are actually invoked.  Task switch and task
 * begin extensions may run before or in parallel with the thread start
 * extension in SMP configurations, see ::rtems_task_switch_extension and
 * ::rtems_task_begin_extension.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is disabled.
 * @endparblock
 */
typedef User_extensions_thread_start_extension rtems_task_start_extension;

/* Generated from spec:/rtems/userext/if/task-switch */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task switch extensions are invoked when a thread switch from an
 *   executing thread to a heir thread takes place.
 *
 * @param executing is the TCB of the executing thread.  In SMP configurations,
 *   this is the previously executing thread also known as the ancestor thread.
 *
 * @param heir is the TCB of the heir thread.  In SMP configurations, this is
 *   the executing thread.
 *
 * @par Notes
 * @parblock
 * The task switch extensions are invoked in extension forward order.
 *
 * The invocation conditions of the task switch extensions depend on whether
 * RTEMS was built with SMP support enabled or disabled.  A user must pay
 * attention to the differences to correctly implement a task switch extension.
 *
 * Where the system was built with SMP support disabled, the task switch
 * extensions are invoked before the context switch from the currently
 * executing thread to the heir thread.  The executing is a pointer to the TCB
 * of the currently executing thread. The heir is a pointer to the TCB of the
 * heir thread.  The context switch initiated through the multitasking start is
 * not covered by the task switch extensions.
 *
 * Where the system was built with SMP support enabled, the task switch
 * extensions are invoked after the context switch to the heir thread.  The
 * executing is a pointer to the TCB of the previously executing thread.
 * Despite the name, this is not the currently executing thread. The heir is a
 * pointer to the TCB of the newly executing thread. This is the currently
 * executing thread. The context switches initiated through the multitasking
 * start are covered by the task switch extensions. The reason for the
 * differences to uniprocessor configurations is that the context switch may
 * update the heir thread of the processor.  The task switch extensions are
 * invoked with maskable interrupts disabled and with ownership of a
 * processor-specific SMP lock.  Task switch extensions may run in parallel on
 * multiple processors.  It is recommended to use thread-local or
 * processor-specific data structures for task switch extensions.  A global SMP
 * lock should be avoided for performance reasons, see
 * rtems_interrupt_lock_initialize().
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is disabled.
 *
 * * Where the system was built with SMP support enabled, maskable interrupts
 *   are disabled for the executing thread.
 * @endparblock
 */
typedef User_extensions_thread_switch_extension rtems_task_switch_extension;

/* Generated from spec:/rtems/userext/if/task-terminate */

/**
 * @ingroup RTEMSAPIClassicUserExt
 *
 * @brief Task terminate extensions are invoked when a task terminates.
 *
 * @param executing is the TCB of the executing thread.  This is the
 *   terminating thread.
 *
 * @par Notes
 * @parblock
 * The task terminate extensions are invoked in extension reverse order.
 *
 * The task terminate extensions are invoked in the context of the terminating
 * thread right before the thread dispatch to the heir thread should take
 * place. The thread stack reflects the previous execution context.  The POSIX
 * cleanup and key destructors execute in this context.
 *
 * Thread restart and delete requests issued by terminate extensions lead to
 * recursion.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to functions of this type:
 *
 * * Thread dispatching is enabled.
 *
 * * Thread life is protected.
 *
 * * The executing thread is not the owner of the object allocator mutex.
 * @endparblock
 */
typedef User_extensions_thread_terminate_extension rtems_task_terminate_extension;

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_EXTENSION_H */
