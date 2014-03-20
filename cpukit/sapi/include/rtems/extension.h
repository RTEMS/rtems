/**
 * @file
 * 
 * @brief User Extensions API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_EXTENSION_H
#define _RTEMS_EXTENSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/userext.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

typedef struct {
  Objects_Control          Object;
  User_extensions_Control  Extension;
}   Extension_Control;

typedef User_extensions_routine
  rtems_extension RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 * @defgroup ClassicUserExtensions User Extensions
 *
 * @ingroup ClassicRTEMS
 *
 * @brief The User Extensions Manager allows the application developer to
 * augment the executive by allowing them to supply extension routines which
 * are invoked at critical system events.
 *
 * @section ClassicUserExtensionsSets Extension Sets
 *
 * An @ref User_extensions_Table "extension set" is defined as a set of
 * routines which are invoked at each of the critical system events at which
 * user extension routines are invoked.  Together a set of these routines
 * typically perform a specific functionality such as performance monitoring or
 * debugger support.
 *
 * RTEMS allows the user to have multiple extension sets active at the same
 * time. First, a single static extension set may be defined as the
 * application's User Extension Table which is included as part of the
 * Configuration Table. This extension set is active for the entire life of the
 * system and may not be deleted. This extension set is especially important
 * because it is the only way the application can provided a fatal error
 * extension which is invoked if RTEMS fails during the
 * rtems_initialize_data_structures() directive. The static extension set is
 * optional and may be configured as @c NULL if no static extension set is
 * required.
 *
 * Second, the user can install dynamic extensions using the
 * rtems_extension_create() directive. These extensions are RTEMS objects in
 * that they have a name, an ID, and can be dynamically created and deleted. In
 * contrast to the static extension set, these extensions can only be created
 * and installed after the rtems_initialize_data_structures() directive
 * successfully completes execution. Dynamic extensions are useful for
 * encapsulating the functionality of an extension set. For example, the
 * application could use extensions to manage a special coprocessor, do
 * performance monitoring, and to do stack bounds checking. Each of these
 * extension sets could be written and installed independently of the others.
 *
 * All user extensions are optional and RTEMS places no naming restrictions on
 * the user. The user extension entry points are copied into an internal RTEMS
 * structure. This means the user does not need to keep the table after
 * creating it, and changing the handler entry points dynamically in a table
 * once created has no effect. Creating a table local to a function can save
 * space in space limited applications.
 *
 * Extension switches do not effect the context switch overhead if no switch
 * handler is installed.
 *
 * @section ClassicUserExtensionsTCB Task Control Block Area
 *
 * RTEMS provides for a pointer to a user-defined data area for each extension
 * set to be linked to each task's control block (TCB). This area is only
 * available for the dynamic extensions. This set of pointers is an extension
 * of the TCB and can be used to store additional data required by the user's
 * extension functions. It is also possible for a user extension to utilize the
 * notepad locations associated with each task although this may conflict with
 * application usage of those particular notepads.
 *
 * The TCB extension is an array of pointers in the TCB. The index into the
 * table can be obtained from the extension identifier returned when the
 * extension is created:
 *
 * @code
 * rtems_tcb *task = some_task;
 * size_t index = rtems_object_id_get_index(extension_id);
 * void *extension_data = task->extensions [index];
 * @endcode
 *
 * The number of pointers in the area is the same as the number of user
 * extension sets configured. This allows an application to augment the TCB
 * with user-defined information. For example, an application could implement
 * task profiling by storing timing statistics in the TCB's extended memory
 * area. When a task context switch is being executed, the task switch
 * extension could read a real-time clock to calculate how long the task being
 * swapped out has run as well as timestamp the starting time for the task
 * being swapped in.
 *
 * If used, the extended memory area for the TCB should be allocated and the
 * TCB extension pointer should be set at the time the task is created or
 * started by either the task create or task start extension. The application
 * is responsible for managing this extended memory area for the TCBs. The
 * memory may be reinitialized by the task restart extension and should be
 * deallocated by the task delete extension when the task is deleted. Since the
 * TCB extension buffers would most likely be of a fixed size, the RTEMS
 * partition manager could be used to manage the application's extended memory
 * area. The application could create a partition of fixed size TCB extension
 * buffers and use the partition manager's allocation and deallocation
 * directives to obtain and release the extension buffers.
 *
 * @section ClassicUserExtensionsOrder Order of Invokation
 *
 * When one of the critical system events occur, the user extensions are
 * invoked in either @a forward or @a reverse order. Forward order indicates
 * that the static extension set is invoked followed by the dynamic extension
 * sets in the order in which they were created. Reverse order means that the
 * dynamic extension sets are invoked in the opposite of the order in which
 * they were created followed by the static extension set. By invoking the
 * extension sets in this order, extensions can be built upon one another. At
 * the following system events, the extensions are invoked in forward order:
 *
 * - Task creation
 * - Task start
 * - Task restart
 * - Task context switch
 * - Post task context switch
 * - Task begins to execute
 *
 * At the following system events, the extensions are invoked in reverse order:
 *
 * - Task exit
 * - Task deletion
 * - Fatal error detection
 *
 * At these system events, the extensions are invoked in reverse order to
 * insure that if an extension set is built upon another, the more complicated
 * extension is invoked before the extension set it is built upon. For example,
 * by invoking the static extension set last it is known that the "system"
 * fatal error extension will be the last fatal error extension executed.
 * Another example is use of the task delete extension by the Standard C
 * Library. Extension sets which are installed after the Standard C Library
 * will operate correctly even if they utilize the C Library because the C
 * Library's task delete extension is invoked after that of the other
 * extensions.
 */
/**@{**/

typedef User_extensions_thread_create_extension   rtems_task_create_extension;
typedef User_extensions_thread_delete_extension   rtems_task_delete_extension;
typedef User_extensions_thread_start_extension    rtems_task_start_extension;
typedef User_extensions_thread_restart_extension  rtems_task_restart_extension;
typedef User_extensions_thread_switch_extension   rtems_task_switch_extension;
typedef User_extensions_thread_begin_extension    rtems_task_begin_extension;
typedef User_extensions_thread_exitted_extension  rtems_task_exitted_extension;
typedef User_extensions_fatal_extension           rtems_fatal_extension;

typedef User_extensions_Table                     rtems_extensions_table;

typedef Internal_errors_Source rtems_fatal_source;

typedef Internal_errors_t rtems_fatal_code;

/**
 * @brief Creates an extension set object.
 *
 * This directive creates a extension set object from the extension table
 * @a extension_table.  The assigned extension set identifier is returned in
 * @a id.  The identifier is used to access this extension set in other
 * extension set related directives.  The name @a name will be assigned to the
 * extension set object.
 *
 * Newly created extension sets are immediately installed and are invoked upon
 * the next system event supporting an extension.
 *
 * This directive will not cause the calling task to be preempted.
 *
 * @retval RTEMS_SUCCESSFUL Extension set created successfully.
 * @retval RTEMS_INVALID_ADDRESS Identifier pointer is @c NULL.
 * @retval RTEMS_INVALID_NAME Invalid extension set name.
 * @retval RTEMS_TOO_MANY Too many extension sets created.
 */
rtems_status_code rtems_extension_create(
  rtems_name                    name,
  const rtems_extensions_table *extension_table,
  rtems_id                     *id
);

/**
 * @brief Identifies an extension set object by a name.
 *
 * This directive obtains an extension set identifier in @a id associated with
 * the extension set name @a name. If the extension set name is not unique,
 * then the extension set identifier will match one of the extension sets with
 * that name.  However, this extension set identifier is not guaranteed to
 * correspond to the desired extension set. The extension set identifier is
 * used to access this extension set in other extension set related directives.
 *
 * This directive will not cause the calling task to be preempted.
 *
 * @retval RTEMS_SUCCESSFUL Extension set identified successfully.
 * @retval RTEMS_INVALID_ADDRESS Identifier pointer is @c NULL.
 * @retval RTEMS_INVALID_NAME Extension set name not found or invalid name.
 */
rtems_status_code rtems_extension_ident(
  rtems_name  name,
  rtems_id   *id
);

/**
 * @brief Deletes an extension set object specified by the identifier @a id.
 *
 * Any subsequent references to the extension's name and identifier are
 * invalid.
 *
 * This directive will not cause the calling task to be preempted.
 *
 * @retval RTEMS_SUCCESSFUL Extension set deleted successfully.
 * @retval RTEMS_INVALID_ID Invalid extension set identifier.
 */
rtems_status_code rtems_extension_delete(
  rtems_id id
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
