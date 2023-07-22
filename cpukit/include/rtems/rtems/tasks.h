/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This header file defines the main parts of the Tasks Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 1988, 2023 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/task/if/header */

#ifndef _RTEMS_RTEMS_TASKS_H
#define _RTEMS_RTEMS_TASKS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cpuset.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/context.h>
#include <rtems/score/cpu.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/watchdogticks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/task/if/group */

/**
 * @defgroup RTEMSAPIClassicTasks Task Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Task Manager provides a comprehensive set of directives to
 *   create, delete, and administer tasks.
 */

/* Generated from spec:/rtems/task/if/argument */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This integer type represents task argument values.
 *
 * @par Notes
 * The type is an architecture-specific unsigned integer type which is large
 * enough to represent pointer values and 32-bit unsigned integers.
 */
typedef CPU_Uint32ptr rtems_task_argument;

/* Generated from spec:/rtems/task/if/config */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This structure defines the configuration of a task constructed by
 *   rtems_task_construct().
 */
typedef struct {
  /**
   * @brief This member defines the name of the task.
   */
  rtems_name name;

  /**
   * @brief This member defines the initial priority of the task.
   */
  rtems_task_priority initial_priority;

  /**
   * @brief This member shall point to the task storage area begin.
   *
   * The task storage area will contain the task stack, the thread-local storage,
   * and the floating-point context on architectures with a separate
   * floating-point context.
   *
   * The task storage area begin address and size should be aligned by
   * #RTEMS_TASK_STORAGE_ALIGNMENT.  To avoid memory waste, use RTEMS_ALIGNED()
   * and #RTEMS_TASK_STORAGE_ALIGNMENT to enforce the recommended alignment of a
   * statically allocated task storage area.
   */
  void *storage_area;

  /**
   * @brief This member defines size of the task storage area in bytes.
   *
   * Use the RTEMS_TASK_STORAGE_SIZE() macro to determine the recommended task
   * storage area size.
   */
  size_t storage_size;

  /**
   * @brief This member defines the maximum thread-local storage size supported
   *   by the task storage area.
   *
   * Use RTEMS_ALIGN_UP() and #RTEMS_TASK_STORAGE_ALIGNMENT to adjust the size to
   * meet the minimum alignment requirement of a thread-local storage area used
   * to construct a task.
   *
   * If the value is less than the actual thread-local storage size, then the
   * task construction by rtems_task_construct() fails.
   *
   * If the is less than the task storage area size, then the task construction
   * by rtems_task_construct() fails.
   *
   * The actual thread-local storage size is determined when the application
   * executable is linked.  The ``rtems-exeinfo`` command line tool included in
   * the RTEMS Tools can be used to obtain the thread-local storage size and
   * alignment of an application executable.
   *
   * The application may configure the maximum thread-local storage size for all
   * threads explicitly through the @ref
   * CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE configuration option.
   */
  size_t maximum_thread_local_storage_size;

  /**
   * @brief This member defines the optional handler to free the task storage
   *   area.
   *
   * It is called on exactly two mutually exclusive occasions.  Firstly, when the
   * task construction aborts due to a failed task create extension, or secondly,
   * when the task is deleted.  It is called from task context under protection
   * of the object allocator lock.  It is allowed to call free() in this handler.
   * If handler is NULL, then no action will be performed.
   */
  void ( *storage_free )( void * );

  /**
   * @brief This member defines the initial modes of the task.
   */
  rtems_mode initial_modes;

  /**
   * @brief This member defines the attributes of the task.
   */
  rtems_attribute attributes;
} rtems_task_config;

/* Generated from spec:/rtems/task/if/configured-minimum-stack-size */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This constant can be used to indicate that the task should be created
 *   with the configured minimum stack size.
 *
 * Using this constant when specifying the task stack size indicates that this
 * task is to be created with a stack size of the minimum stack size that was
 * configured by the application. If not explicitly configured by the
 * application, the default configured minimum stack size is
 * #RTEMS_MINIMUM_STACK_SIZE which depends on the target architecture. Since
 * this uses the configured minimum stack size value, you may get a stack size
 * that is smaller or larger than the recommended minimum. This can be used to
 * provide large stacks for all tasks on complex applications or small stacks
 * on applications that are trying to conserve memory.
 */
#define RTEMS_CONFIGURED_MINIMUM_STACK_SIZE 0

/* Generated from spec:/rtems/task/if/current-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This constant is passed to rtems_task_set_priority() when the caller
 *   wants to obtain the current priority.
 */
#define RTEMS_CURRENT_PRIORITY 0

/* Generated from spec:/rtems/task/if/task */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This type defines the return type of task entry points.
 *
 * This type can be used to document task entry points in the source code.
 */
typedef void rtems_task;

/* Generated from spec:/rtems/task/if/entry */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This type defines the task entry point of an RTEMS task.
 */
typedef rtems_task ( *rtems_task_entry )( rtems_task_argument );

/* Generated from spec:/rtems/task/if/initialization-table */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This structure defines the properties of the Classic API user
 *   initialization task.
 */
typedef struct {
  /**
   * @brief This member defines the task name.
   */
  rtems_name name;

  /**
   * @brief This member defines the task stack size in bytes.
   */
  size_t stack_size;

  /**
   * @brief This member defines the initial task priority.
   */
  rtems_task_priority initial_priority;

  /**
   * @brief This member defines the attribute set of the task.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member defines the entry point of the task.
   */
  rtems_task_entry entry_point;

  /**
   * @brief This member defines the initial modes of the task.
   */
  rtems_mode mode_set;

  /**
   * @brief This member defines the entry point argument of the task.
   */
  rtems_task_argument argument;
} rtems_initialization_tasks_table;

/* Generated from spec:/rtems/task/if/maximum-priority-impl */

/**
 * @ingroup RTEMSImplClassicTask
 *
 * @brief Returns the maximum priority of the scheduler with index zero.
 */
rtems_task_priority _RTEMS_Maximum_priority( void );

/* Generated from spec:/rtems/task/if/maximum-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This runtime constant represents the lowest (least important) task
 *   priority of the scheduler with index zero.
 */
#define RTEMS_MAXIMUM_PRIORITY _RTEMS_Maximum_priority()

/* Generated from spec:/rtems/task/if/minimum-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant provides the highest (most important) task
 *   priority settable by the API.
 */
#define RTEMS_MINIMUM_PRIORITY 1

/* Generated from spec:/rtems/task/if/minimum-stack-size */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant provides the minimum task stack size
 *   recommended for the target architecture.
 *
 * It is the minimum stack size recommended for use on this processor.  This
 * value is selected by the RTEMS maintainers conservatively to minimize the
 * risk of blown stacks for most user applications.  Using this constant when
 * specifying the task stack size, indicates that the stack size will be at
 * least RTEMS_MINIMUM_STACK_SIZE bytes in size.  If the user configured
 * minimum stack size (see @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE) is larger
 * than the recommended minimum, then it will be used.
 */
#define RTEMS_MINIMUM_STACK_SIZE STACK_MINIMUM_SIZE

/* Generated from spec:/rtems/task/if/no-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant may be used for the
 *   rtems_task_set_priority() directive to get the current task priority.
 */
#define RTEMS_NO_PRIORITY RTEMS_CURRENT_PRIORITY

/* Generated from spec:/rtems/task/if/self-define */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant may be used to identify the calling task
 *   in task related directives.
 */
#define RTEMS_SELF OBJECTS_ID_OF_SELF

/* Generated from spec:/rtems/task/if/storage-alignment */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant defines the recommended alignment of a
 *   task storage area in bytes.
 *
 * @par Notes
 * Use it with RTEMS_ALIGNED() to define the alignment of a statically
 * allocated task storage area.
 */
#define RTEMS_TASK_STORAGE_ALIGNMENT CPU_STACK_ALIGNMENT

/* Generated from spec:/rtems/task/if/storage-size */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets the recommended task storage area size for the size and task
 *   attributes.
 *
 * @param _size is the size dedicated to the task stack and thread-local
 *   storage in bytes.
 *
 * @param _attributes is the attribute set of the task using the storage area.
 *
 * @return Returns the recommended task storage area size calculated from the
 *   input parameters.
 */
#if CPU_ALL_TASKS_ARE_FP == TRUE
  #define RTEMS_TASK_STORAGE_SIZE( _size, _attributes ) \
    ( ( _size ) + CONTEXT_FP_SIZE )
#else
  #define RTEMS_TASK_STORAGE_SIZE( _size, _attributes ) \
    ( ( _size ) + \
      ( ( ( _attributes ) & RTEMS_FLOATING_POINT ) != 0 ? \
        CONTEXT_FP_SIZE : 0 ) )
#endif

/* Generated from spec:/rtems/task/if/tcb */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This structure represents the TCB.
 */
typedef struct _Thread_Control rtems_tcb;

/* Generated from spec:/rtems/task/if/visitor */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Visitor routines invoked by rtems_task_iterate() shall have this
 *   type.
 */
typedef bool( *rtems_task_visitor )( rtems_tcb *, void * );

/* Generated from spec:/rtems/task/if/yield-processor */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief This compile time constant may be passed to the
 *   rtems_task_wake_after() directive as the interval when a task wishes to
 *   yield the processor.
 */
#define RTEMS_YIELD_PROCESSOR WATCHDOG_NO_TIMEOUT

/* Generated from spec:/rtems/task/if/create */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Creates a task.
 *
 * @param name is the object name of the task.
 *
 * @param initial_priority is the initial task priority.
 *
 * @param stack_size is the task stack size in bytes.
 *
 * @param initial_modes is the initial mode set of the task.
 *
 * @param attribute_set is the attribute set of the task.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the created task will be stored in
 *   this object.
 *
 * This directive creates a task which resides on the local node.  The task has
 * the user-defined object name specified in ``name``.  The assigned object
 * identifier is returned in ``id``.  This identifier is used to access the
 * task with other task related directives.
 *
 * The **initial priority** of the task is specified in ``initial_priority``.
 * The home scheduler of the created task is the home scheduler of the calling
 * task at some time point during the task creation. The initial task priority
 * specified in ``initial_priority`` shall be valid for this scheduler.
 *
 * The **stack size** of the task is specified in ``stack_size``.  If the
 * requested stack size is less than the configured minimum stack size, then
 * RTEMS will use the configured minimum as the stack size for this task.  The
 * configured minimum stack size is defined by the @ref
 * CONFIGURE_MINIMUM_TASK_STACK_SIZE application configuration option.  In
 * addition to being able to specify the task stack size as a integer, there
 * are two constants which may be specified:
 *
 * * The #RTEMS_MINIMUM_STACK_SIZE constant can be specified to use the
 *   **recommended minimum stack size** for the target processor.  This value
 *   is selected by the RTEMS maintainers conservatively to minimize the risk
 *   of blown stacks for most user applications.  Using this constant when
 *   specifying the task stack size, indicates that the stack size will be at
 *   least #RTEMS_MINIMUM_STACK_SIZE bytes in size.  If the user configured
 *   minimum stack size is larger than the recommended minimum, then it will be
 *   used.
 *
 * * The #RTEMS_CONFIGURED_MINIMUM_STACK_SIZE constant can be specified to use
 *   the minimum stack size that was configured by the application.  If not
 *   explicitly configured by the application, the default configured minimum
 *   stack size is the target processor dependent value
 *   #RTEMS_MINIMUM_STACK_SIZE.  Since this uses the configured minimum stack
 *   size value, you may get a stack size that is smaller or larger than the
 *   recommended minimum.  This can be used to provide large stacks for all
 *   tasks on complex applications or small stacks on applications that are
 *   trying to conserve memory.
 *
 * The **initial mode set** specified in ``initial_modes`` is built through a
 * *bitwise or* of the mode constants described below.  Not all combinations of
 * modes are allowed.  Some modes are mutually exclusive.  If mutually
 * exclusive modes are combined, the behaviour is undefined.  Default task
 * modes can be selected by using the #RTEMS_DEFAULT_MODES constant.  The task
 * mode set defines
 *
 * * the preemption mode of the task: #RTEMS_PREEMPT (default) or
 *   #RTEMS_NO_PREEMPT,
 *
 * * the timeslicing mode of the task: #RTEMS_TIMESLICE or #RTEMS_NO_TIMESLICE
 *   (default),
 *
 * * the ASR processing mode of the task: #RTEMS_ASR (default) or
 *   #RTEMS_NO_ASR,
 *
 * * the interrupt level of the task: RTEMS_INTERRUPT_LEVEL() with a default of
 *   ``RTEMS_INTERRUPT_LEVEL( 0 )`` which is associated with enabled
 *   interrupts.
 *
 * The **initial preemption mode** of the task is enabled or disabled.
 *
 * * An **enabled preemption** is the default and can be emphasized through the
 *   use of the #RTEMS_PREEMPT mode constant.
 *
 * * A **disabled preemption** is set by the #RTEMS_NO_PREEMPT mode constant.
 *
 * The **initial timeslicing mode** of the task is enabled or disabled.
 *
 * * A **disabled timeslicing** is the default and can be emphasized through
 *   the use of the #RTEMS_NO_TIMESLICE mode constant.
 *
 * * An **enabled timeslicing** is set by the #RTEMS_TIMESLICE mode constant.
 *
 * The **initial ASR processing mode** of the task is enabled or disabled.
 *
 * * An **enabled ASR processing** is the default and can be emphasized through
 *   the use of the #RTEMS_ASR mode constant.
 *
 * * A **disabled ASR processing** is set by the #RTEMS_NO_ASR mode constant.
 *
 * The **initial interrupt level mode** of the task is defined by
 * RTEMS_INTERRUPT_LEVEL().
 *
 * * Task execution with **interrupts enabled** the default and can be
 *   emphasized through the use of the RTEMS_INTERRUPT_LEVEL() mode macro with
 *   a value of zero (0) for the parameter.  An interrupt level of zero is
 *   associated with enabled interrupts on all target processors.
 *
 * * Task execution at a **non-zero interrupt level** can be specified by the
 *   RTEMS_INTERRUPT_LEVEL() mode macro with a non-zero value for the
 *   parameter.  The interrupt level portion of the task mode supports a
 *   maximum of 256 interrupt levels.  These levels are mapped onto the
 *   interrupt levels actually supported by the target processor in a processor
 *   dependent fashion.
 *
 * The **attribute set** specified in ``attribute_set`` is built through a
 * *bitwise or* of the attribute constants described below.  Not all
 * combinations of attributes are allowed.  Some attributes are mutually
 * exclusive.  If mutually exclusive attributes are combined, the behaviour is
 * undefined.  Attributes not mentioned below are not evaluated by this
 * directive and have no effect.  Default attributes can be selected by using
 * the #RTEMS_DEFAULT_ATTRIBUTES constant.  The attribute set defines
 *
 * * the scope of the task: #RTEMS_LOCAL (default) or #RTEMS_GLOBAL and
 *
 * * the floating-point unit use of the task: #RTEMS_FLOATING_POINT or
 *   #RTEMS_NO_FLOATING_POINT (default).
 *
 * The task has a local or global **scope** in a multiprocessing network (this
 * attribute does not refer to SMP systems).  The scope is selected by the
 * mutually exclusive #RTEMS_LOCAL and #RTEMS_GLOBAL attributes.
 *
 * * A **local scope** is the default and can be emphasized through the use of
 *   the #RTEMS_LOCAL attribute.  A local task can be only used by the node
 *   which created it.
 *
 * * A **global scope** is established if the #RTEMS_GLOBAL attribute is set.
 *   Setting the global attribute in a single node system has no effect.the
 *
 * The **use of the floating-point unit** is selected by the mutually exclusive
 * #RTEMS_FLOATING_POINT and #RTEMS_NO_FLOATING_POINT attributes.  On some
 * target processors, the use of the floating-point unit can be enabled or
 * disabled for each task.  Other target processors may have no hardware
 * floating-point unit or enable the use of the floating-point unit for all
 * tasks.  Consult the *RTEMS CPU Architecture Supplement* for the details.
 *
 * * A **disabled floating-point unit** is the default and can be emphasized
 *   through use of the #RTEMS_NO_FLOATING_POINT attribute.  For performance
 *   reasons, it is recommended that tasks not using the floating-point unit
 *   should specify this attribute.
 *
 * * An **enabled floating-point unit** is selected by the
 *   #RTEMS_FLOATING_POINT attribute.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The ``initial_priority`` was invalid.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   task.  The number of tasks available to the application is configured
 *   through the @ref CONFIGURE_MAXIMUM_TASKS application configuration option.
 *
 * @retval ::RTEMS_TOO_MANY In multiprocessing configurations, there was no
 *   inactive global object available to create a global task.  The number of
 *   global objects available to the application is configured through the @ref
 *   CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application configuration option.
 *
 * @retval ::RTEMS_UNSATISFIED There was not enough memory to allocate the task
 *   storage area.  The task storage area contains the task stack, the
 *   thread-local storage, and the floating point context.
 *
 * @retval ::RTEMS_UNSATISFIED One of the task create extensions failed to
 *   create the task.
 *
 * @retval ::RTEMS_UNSATISFIED In SMP configurations, the non-preemption mode
 *   was not supported.
 *
 * @retval ::RTEMS_UNSATISFIED In SMP configurations, the interrupt level mode
 *   was not supported.
 *
 * @par Notes
 * @parblock
 * The task processor affinity is initialized to the set of online processors.
 *
 * When created, a task is placed in the dormant state and can only be made
 * ready to execute using the directive rtems_task_start().
 *
 * Application developers should consider the stack usage of the device drivers
 * when calculating the stack size required for tasks which utilize the driver.
 * The task stack size shall account for an target processor dependent
 * interrupt stack frame which may be placed on the stack of the interrupted
 * task while servicing an interrupt.  The stack checker may be used to monitor
 * the stack usage, see @ref CONFIGURE_STACK_CHECKER_ENABLED.
 *
 * For control and maintenance of the task, RTEMS allocates a TCB from the
 * local TCB free pool and initializes it.
 *
 * The TCB for a global task is allocated on the local node.  Task should not
 * be made global unless remote tasks must interact with the task.  This is to
 * avoid the system overhead incurred by the creation of a global task.  When a
 * global task is created, the task's name and identifier must be transmitted
 * to every node in the system for insertion in the local copy of the global
 * object table.
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
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The number of tasks available to the application is configured through the
 *   @ref CONFIGURE_MAXIMUM_TASKS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 *
 * * The number of global objects available to the application is configured
 *   through the @ref CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application
 *   configuration option.
 * @endparblock
 */
rtems_status_code rtems_task_create(
  rtems_name          name,
  rtems_task_priority initial_priority,
  size_t              stack_size,
  rtems_mode          initial_modes,
  rtems_attribute     attribute_set,
  rtems_id           *id
);

/* Generated from spec:/rtems/task/if/construct */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Constructs a task from the specified task configuration.
 *
 * @param config is the pointer to an rtems_task_config object.  It configures
 *   the task.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the identifier of the constructed task will be stored
 *   in this object.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``config`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The task name was invalid.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The initial task priority was invalid.
 *
 * @retval ::RTEMS_INVALID_SIZE The thread-local storage size is greater than
 *   the maximum thread-local storage size specified in the task configuration.
 *   The thread-local storage size is determined by the thread-local variables
 *   used by the application and @ref
 *   CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE.
 *
 * @retval ::RTEMS_INVALID_SIZE The task storage area was too small to provide
 *   a task stack of the configured minimum size, see @ref
 *   CONFIGURE_MINIMUM_TASK_STACK_SIZE. The task storage area contains the task
 *   stack, the thread-local storage, and the floating-point context on
 *   architectures with a separate floating-point context.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive task object available to
 *   construct a task.
 *
 * @retval ::RTEMS_TOO_MANY In multiprocessing configurations, there was no
 *   inactive global object available to construct a global task.
 *
 * @retval ::RTEMS_UNSATISFIED One of the task create extensions failed during
 *   the task construction.
 *
 * @retval ::RTEMS_UNSATISFIED In SMP configurations, the non-preemption mode
 *   was not supported.
 *
 * @retval ::RTEMS_UNSATISFIED In SMP configurations, the interrupt level mode
 *   was not supported.
 *
 * @par Notes
 * @parblock
 * In contrast to tasks created by rtems_task_create(), the tasks constructed
 * by this directive use a user-provided task storage area.  The task storage
 * area contains the task stack, the thread-local storage, and the
 * floating-point context on architectures with a separate floating-point
 * context.
 *
 * This directive is intended for applications which do not want to use the
 * RTEMS Workspace and instead statically allocate all operating system
 * resources.  It is not recommended to use rtems_task_create() and
 * rtems_task_construct() together in an application.  It is also not
 * recommended to use rtems_task_construct() for drivers or general purpose
 * libraries.  The reason for these recommendations is that the task
 * configuration needs settings which can be only given with a through
 * knowledge of the application resources.
 *
 * An application based solely on static allocation can avoid any runtime
 * memory allocators.  This can simplify the application architecture as well
 * as any analysis that may be required.
 *
 * The stack space estimate done by ``<rtems/confdefs.h>`` assumes that all
 * tasks are created by rtems_task_create().  The estimate can be adjusted to
 * take user-provided task storage areas into account through the @ref
 * CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE application configuration
 * option.
 *
 * The @ref CONFIGURE_MAXIMUM_TASKS should include tasks constructed by
 * rtems_task_construct().
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
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The number of tasks available to the application is configured through the
 *   @ref CONFIGURE_MAXIMUM_TASKS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 *
 * * The number of global objects available to the application is configured
 *   through the @ref CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS application
 *   configuration option.
 * @endparblock
 */
rtems_status_code rtems_task_construct(
  const rtems_task_config *config,
  rtems_id                *id
);

/* Generated from spec:/rtems/task/if/ident */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Identifies a task by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param node is the node or node set to search for a matching object.
 *
 * @param[out] id is the pointer to an ::rtems_id object.  When the directive
 *   call is successful, the object identifier of an object with the specified
 *   name will be stored in this object.
 *
 * This directive obtains a task identifier associated with the task name
 * specified in ``name``.
 *
 * A task may obtain its own identifier by specifying #RTEMS_WHO_AM_I for the
 * name.
 *
 * The node to search is specified in ``node``.  It shall be
 *
 * * a valid node number,
 *
 * * the constant #RTEMS_SEARCH_ALL_NODES to search in all nodes,
 *
 * * the constant #RTEMS_SEARCH_LOCAL_NODE to search in the local node only, or
 *
 * * the constant #RTEMS_SEARCH_OTHER_NODES to search in all nodes except the
 *   local node.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the specified nodes.
 *
 * @retval ::RTEMS_INVALID_NODE In multiprocessing configurations, the
 *   specified node was invalid.
 *
 * @par Notes
 * @parblock
 * If the task name is not unique, then the task identifier will match the
 * first task with that name in the search order.  However, this task
 * identifier is not guaranteed to correspond to the desired task.
 *
 * The objects are searched from lowest to the highest index.  If ``node`` is
 * #RTEMS_SEARCH_ALL_NODES, all nodes are searched with the local node being
 * searched first.  All other nodes are searched from lowest to the highest
 * node number.
 *
 * If node is a valid node number which does not represent the local node, then
 * only the tasks exported by the designated node are searched.
 *
 * This directive does not generate activity on remote nodes.  It accesses only
 * the local copy of the global object table.
 *
 * The task identifier is used with other task related directives to access the
 * task.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_ident(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
);

/* Generated from spec:/rtems/task/if/self */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets the task identifier of the calling task.
 *
 * This directive returns the task identifier of the calling task.
 *
 * @return Returns the task identifier of the calling task.
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
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_id rtems_task_self( void );

/* Generated from spec:/rtems/task/if/start */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Starts the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * @param entry_point is the task entry point.
 *
 * @param argument is the task entry point argument.
 *
 * This directive readies the task, specified by ``id``, for execution based on
 * the priority and execution mode specified when the task was created.  The
 * task entry point of the task is given in ``entry_point``.  The task's entry
 * point argument is contained in ``argument``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``entry_point`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INCORRECT_STATE The task was not in the dormant state.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Notes
 * @parblock
 * The type of the entry point argument is an unsigned integer type.  However,
 * the integer type has the property that any valid pointer to ``void`` can be
 * converted to this type and then converted back to a pointer to ``void``.
 * The result will compare equal to the original pointer.  The type can
 * represent at least 32 bits.  Some applications use the entry point argument
 * as an index into a parameter table to get task-specific parameters.
 *
 * Any actions performed on a dormant task such as suspension or change of
 * priority are nullified when the task is initiated via the rtems_task_start()
 * directive.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 * @endparblock
 */
rtems_status_code rtems_task_start(
  rtems_id            id,
  rtems_task_entry    entry_point,
  rtems_task_argument argument
);

/* Generated from spec:/rtems/task/if/restart */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Restarts the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * @param argument is the task entry point argument.
 *
 * This directive resets the task specified by ``id`` to begin execution at its
 * original entry point.  The task's priority and execution mode are set to the
 * original creation values.  If the task is currently blocked, RTEMS
 * automatically makes the task ready.  A task can be restarted from any state,
 * except the dormant state.  The task's entry point argument is contained in
 * ``argument``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INCORRECT_STATE The task never started.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Notes
 * @parblock
 * The type of the entry point argument is an unsigned integer type.  However,
 * the integer type has the property that any valid pointer to ``void`` can be
 * converted to this type and then converted back to a pointer to ``void``.
 * The result will compare equal to the original pointer.  The type can
 * represent at least 32 bits.  Some applications use the entry point argument
 * as an index into a parameter table to get task-specific parameters.
 *
 * A new entry point argument may be used to distinguish between the initial
 * rtems_task_start() of the task and any ensuing calls to rtems_task_restart()
 * of the task.  This can be beneficial in deleting a task.  Instead of
 * deleting a task using the rtems_task_delete() directive, a task can delete
 * another task by restarting that task, and allowing that task to release
 * resources back to RTEMS and then delete itself.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may change the priority of a task.  This may cause the
 *   calling task to be preempted.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 * @endparblock
 */
rtems_status_code rtems_task_restart(
  rtems_id            id,
  rtems_task_argument argument
);

/* Generated from spec:/rtems/task/if/delete */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Deletes the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * This directive deletes the task, either the calling task or another task, as
 * specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_CALLED_FROM_ISR The directive was called from within
 *   interrupt context.
 *
 * @retval ::RTEMS_INCORRECT_STATE The task termination procedure was started,
 *   however, waiting for the terminating task would have resulted in a
 *   deadlock.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Notes
 * @parblock
 * The task deletion is done in several steps.  Firstly, the task is marked as
 * terminating.  While the task life of the terminating task is protected, it
 * executes normally until it disables the task life protection or it deletes
 * itself.  A terminating task will eventually stop its normal execution and
 * start its termination procedure.  The procedure executes in the context of
 * the terminating task.  The task termination procedure involves the
 * destruction of POSIX key values and running the task termination user
 * extensions.  Once complete the execution of the task is stopped and
 * task-specific resources are reclaimed by the system, such as the stack
 * memory, any allocated delay or timeout timers, the TCB, and, if the task is
 * #RTEMS_FLOATING_POINT, its floating point context area.  RTEMS explicitly
 * does not reclaim the following resources: region segments, partition
 * buffers, semaphores, timers, or rate monotonic periods.
 *
 * A task is responsible for releasing its resources back to RTEMS before
 * deletion.  To insure proper deallocation of resources, a task should not be
 * deleted unless it is unable to execute or does not hold any RTEMS resources.
 * If a task holds RTEMS resources, the task should be allowed to deallocate
 * its resources before deletion.  A task can be directed to release its
 * resources and delete itself by restarting it with a special argument or by
 * sending it a message, an event, or a signal.
 *
 * Deletion of the calling task (#RTEMS_SELF) will force RTEMS to select
 * another task to execute.
 *
 * When a task deletes another task, the calling task waits until the task
 * termination procedure of the task being deleted has completed.  The
 * terminating task inherits the eligible priorities of the calling task.
 *
 * When a global task is deleted, the task identifier must be transmitted to
 * every node in the system for deletion from the local copy of the global
 * object table.
 *
 * The task must reside on the local node, even if the task was created with
 * the #RTEMS_GLOBAL attribute.
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
 * * When the directive operates on a global object, the directive sends a
 *   message to remote nodes.  This may preempt the calling task.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_task_delete( rtems_id id );

/* Generated from spec:/rtems/task/if/exit */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Deletes the calling task.
 *
 * This directive deletes the calling task.
 *
 * @par Notes
 * @parblock
 * The directive is an optimized variant of the following code sequences, see
 * also rtems_task_delete():
 *
 * @code
 * #include <pthread.h>
 * #include <rtems.h>
 *
 * void classic_delete_self( void )
 * {
 *   (void) rtems_task_delete( RTEMS_SELF );
 * }
 *
 * void posix_delete_self( void )
 * {
 *   (void) pthread_detach( pthread_self() );
 *   (void) pthread_exit( NULL);
 * }
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not return to the caller.
 *
 * * While thread dispatching is disabled, if the directive performs a thread
 *   dispatch, then the fatal error with the fatal source INTERNAL_ERROR_CORE
 *   and the fatal code INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL will
 *   occur.
 * @endparblock
 */
RTEMS_NO_RETURN void rtems_task_exit( void );

/* Generated from spec:/rtems/task/if/suspend */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Suspends the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * This directive suspends the task specified by ``id`` from further execution
 * by placing it in the suspended state.  This state is additive to any other
 * blocked state that the task may already be in.  The task will not execute
 * again until another task issues the rtems_task_resume() directive for this
 * task and any blocked state has been removed.  The rtems_task_restart()
 * directive will also remove the suspended state.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_ALREADY_SUSPENDED The task was already suspended.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Notes
 * The requesting task can suspend itself for example by specifying #RTEMS_SELF
 * as ``id``.  In this case, the task will be suspended and a successful return
 * code will be returned when the task is resumed.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_task_suspend( rtems_id id );

/* Generated from spec:/rtems/task/if/resume */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Resumes the task.
 *
 * @param id is the task identifier.
 *
 * This directive removes the task specified by ``id`` from the suspended
 * state.  If the task is in the ready state after the suspension is removed,
 * then it will be scheduled to run.  If the task is still in a blocked state
 * after the suspension is removed, then it will remain in that blocked state.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INCORRECT_STATE The task was not suspended.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_task_resume( rtems_id id );

/* Generated from spec:/rtems/task/if/is-suspended */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Checks if the task is suspended.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * This directive returns a status code indicating whether or not the task
 * specified by ``id`` is currently suspended.
 *
 * @retval ::RTEMS_SUCCESSFUL The task was **not** suspended.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_ALREADY_SUSPENDED The task was suspended.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_is_suspended( rtems_id id );

/* Generated from spec:/rtems/task/if/set-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Sets the real priority or gets the current priority of the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * @param new_priority is the new real priority or #RTEMS_CURRENT_PRIORITY to
 *   get the current priority.
 *
 * @param[out] old_priority is the pointer to an ::rtems_task_priority object.
 *   When the directive call is successful, the current or previous priority of
 *   the task with respect to its home scheduler will be stored in this object.
 *
 * This directive manipulates the priority of the task specified by ``id``.
 * When ``new_priority`` is not equal to #RTEMS_CURRENT_PRIORITY, the specified
 * task's previous priority is returned in ``old_priority``.  When
 * ``new_priority`` is #RTEMS_CURRENT_PRIORITY, the specified task's current
 * priority is returned in ``old_priority``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``old_priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The task priority specified in
 *   ``new_priority`` was invalid with respect to the home scheduler of the
 *   task.
 *
 * @par Notes
 * @parblock
 * Valid priorities range from one to a maximum value which depends on the
 * configured scheduler.  The lower the priority value the higher is the
 * importance of the task.
 *
 * If the task is currently holding any binary semaphores which use a locking
 * protocol, then the task's priority cannot be lowered immediately.  If the
 * task's priority were lowered immediately, then this could violate properties
 * of the locking protocol and may result in priority inversion.  The requested
 * lowering of the task's priority will occur when the task has released all
 * binary semaphores which make the task more important.  The task's priority
 * can be increased regardless of the task's use of binary semaphores with
 * locking protocols.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may change the priority of a task.  This may cause the
 *   calling task to be preempted.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/* Generated from spec:/rtems/task/if/get-priority */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets the current priority of the task with respect to the scheduler.
 *
 * @param task_id is the task identifier.  The constant #RTEMS_SELF may be used
 *   to specify the calling task.
 *
 * @param scheduler_id is the scheduler identifier.
 *
 * @param[out] priority is the pointer to an ::rtems_task_priority object.
 *   When the directive call is successful, the current priority of the task
 *   with respect to the specified scheduler will be stored in this object.
 *
 * This directive returns the current priority in ``priority`` of the task
 * specified by ``task_id`` with respect to the scheduler specified by
 * ``scheduler_id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``task_id``.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_NOT_DEFINED The task had no priority with respect to the
 *   scheduler.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Notes
 * The current priority reflects temporary priority adjustments due to locking
 * protocols, the rate-monotonic period objects on some schedulers such as EDF,
 * and the POSIX sporadic server.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_get_priority(
  rtems_id             task_id,
  rtems_id             scheduler_id,
  rtems_task_priority *priority
);

/* Generated from spec:/rtems/task/if/mode */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets and optionally sets the mode of the calling task.
 *
 * @param mode_set is the mode set to apply to the calling task.  When ``mask``
 *   is set to #RTEMS_CURRENT_MODE, the value of this parameter is ignored.
 *   Only modes requested by ``mask`` are applied to the calling task.
 *
 * @param mask is the mode mask which specifies which modes in ``mode_set`` are
 *   applied to the calling task.  When the value is #RTEMS_CURRENT_MODE, the
 *   mode of the calling task is not changed.
 *
 * @param previous_mode_set is the pointer to an ::rtems_mode object.  When the
 *   directive call is successful, the mode of the task before any mode changes
 *   done by the directive call will be stored in this object.
 *
 * This directive queries and optionally manipulates the execution mode of the
 * calling task.  A task's execution mode enables and disables preemption,
 * timeslicing, asynchronous signal processing, as well as specifying the
 * interrupt level.  To modify an execution mode, the mode class(es) to be
 * changed must be specified in the ``mask`` parameter and the desired mode(s)
 * must be specified in the ``mode_set`` parameter.
 *
 * A task can obtain its current execution mode, without modifying it, by
 * calling this directive with a ``mask`` value of #RTEMS_CURRENT_MODE.
 *
 * The **mode set** specified in ``mode_set`` is built through a *bitwise or*
 * of the mode constants described below.  Not all combinations of modes are
 * allowed.  Some modes are mutually exclusive.  If mutually exclusive modes
 * are combined, the behaviour is undefined.  Default task modes can be
 * selected by using the #RTEMS_DEFAULT_MODES constant.  The task mode set
 * defines
 *
 * * the preemption mode of the task: #RTEMS_PREEMPT (default) or
 *   #RTEMS_NO_PREEMPT,
 *
 * * the timeslicing mode of the task: #RTEMS_TIMESLICE or #RTEMS_NO_TIMESLICE
 *   (default),
 *
 * * the ASR processing mode of the task: #RTEMS_ASR (default) or
 *   #RTEMS_NO_ASR,
 *
 * * the interrupt level of the task: RTEMS_INTERRUPT_LEVEL() with a default of
 *   ``RTEMS_INTERRUPT_LEVEL( 0 )`` which is associated with enabled
 *   interrupts.
 *
 * The **mode mask** specified in ``mask`` is built through a *bitwise or* of
 * the mode mask constants described below.
 *
 * When the #RTEMS_PREEMPT_MASK is set in ``mask``, the **preemption mode** of
 * the calling task is
 *
 * * enabled by using the #RTEMS_PREEMPT mode constant in ``mode_set`` and
 *
 * * disabled by using the #RTEMS_NO_PREEMPT mode constant in ``mode_set``.
 *
 * When the #RTEMS_TIMESLICE_MASK is set in ``mask``, the **timeslicing mode**
 * of the calling task is
 *
 * * enabled by using the #RTEMS_TIMESLICE mode constant in ``mode_set`` and
 *
 * * disabled by using the #RTEMS_NO_TIMESLICE mode constant in ``mode_set``.
 *
 * Enabling timeslicing has no effect if preemption is disabled.  For a task to
 * be timesliced, that task must have both preemption and timeslicing enabled.
 *
 * When the #RTEMS_ASR_MASK is set in ``mask``, the **ASR processing mode** of
 * the calling task is
 *
 * * enabled by using the #RTEMS_ASR mode constant in ``mode_set`` and
 *
 * * disabled by using the #RTEMS_NO_ASR mode constant in ``mode_set``.
 *
 * When the #RTEMS_INTERRUPT_MASK is set in ``mask``, **interrupts** of the
 * calling task are
 *
 * * enabled by using the RTEMS_INTERRUPT_LEVEL() mode macro with a value of
 *   zero (0) in ``mode_set`` and
 *
 * * disabled up to the specified level by using the RTEMS_INTERRUPT_LEVEL()
 *   mode macro with a positive value in ``mode_set``.
 *
 * An interrupt level of zero is associated with enabled interrupts on all
 * target processors.  The interrupt level portion of the task mode supports a
 * maximum of 256 interrupt levels.  These levels are mapped onto the interrupt
 * levels actually supported by the target processor in a processor dependent
 * fashion.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_NOT_IMPLEMENTED The #RTEMS_NO_PREEMPT was set in
 *   ``mode_set`` and setting the preemption mode was requested by
 *   #RTEMS_PREEMPT_MASK in ``mask`` and the system configuration had no
 *   implementation for this mode.
 *
 * @retval ::RTEMS_NOT_IMPLEMENTED The RTEMS_INTERRUPT_LEVEL() was set to a
 *   positive level in ``mode_set`` and setting the interrupt level was
 *   requested by #RTEMS_INTERRUPT_MASK in ``mask`` and the system
 *   configuration had no implementation for this mode.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * When the directive enables preemption for the calling task, another task
 *   may preempt the calling task.
 *
 * * While thread dispatching is disabled, if the directive performs a thread
 *   dispatch, then the fatal error with the fatal source INTERNAL_ERROR_CORE
 *   and the fatal code INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL will
 *   occur.
 * @endparblock
 */
rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
);

/* Generated from spec:/rtems/task/if/wake-after */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Wakes up after a count of clock ticks have occurred or yields the
 *   processor.
 *
 * @param ticks is the count of clock ticks to delay the task or
 *   #RTEMS_YIELD_PROCESSOR to yield the processor.
 *
 * This directive blocks the calling task for the specified ``ticks`` count of
 * clock ticks if the value is not equal to #RTEMS_YIELD_PROCESSOR. When the
 * requested count of ticks have occurred, the task is made ready.  The clock
 * tick directives automatically update the delay period.  The calling task may
 * give up the processor and remain in the ready state by specifying a value of
 * #RTEMS_YIELD_PROCESSOR in ``ticks``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @par Notes
 * Setting the system date and time with the rtems_clock_set() directive and
 * similar directives which set CLOCK_REALTIME have no effect on a
 * rtems_task_wake_after() blocked task.  The delay until first clock tick will
 * never be a whole clock tick interval since this directive will never execute
 * exactly on a clock tick.  Applications requiring use of a clock
 * (CLOCK_REALTIME or CLOCK_MONOTONIC) instead of clock ticks should make use
 * of clock_nanosleep().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive requires a Clock Driver.
 *
 * * While thread dispatching is disabled, if the directive performs a thread
 *   dispatch, then the fatal error with the fatal source INTERNAL_ERROR_CORE
 *   and the fatal code INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL will
 *   occur.
 * @endparblock
 */
rtems_status_code rtems_task_wake_after( rtems_interval ticks );

/* Generated from spec:/rtems/task/if/wake-when */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Wakes up when specified.
 *
 * @param time_buffer is the date and time to wake up.
 *
 * This directive blocks a task until the date and time specified in
 * ``time_buffer``.  At the requested date and time, the calling task will be
 * unblocked and made ready to execute.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_NOT_DEFINED The system date and time was not set.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``time_buffer`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_CLOCK The time of day was invalid.
 *
 * @par Notes
 * The ticks portion of ``time_buffer`` structure is ignored.  The timing
 * granularity of this directive is a second.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive requires a Clock Driver.
 *
 * * While thread dispatching is disabled, if the directive performs a thread
 *   dispatch, then the fatal error with the fatal source INTERNAL_ERROR_CORE
 *   and the fatal code INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL will
 *   occur.
 * @endparblock
 */
rtems_status_code rtems_task_wake_when( const rtems_time_of_day *time_buffer );

/* Generated from spec:/rtems/task/if/get-scheduler */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets the home scheduler of the task.
 *
 * @param task_id is the task identifier.  The constant #RTEMS_SELF may be used
 *   to specify the calling task.
 *
 * @param[out] scheduler_id is the pointer to an ::rtems_id object.  When the
 *   directive call is successful, the identifier of the home scheduler of the
 *   task will be stored in this object.
 *
 * This directive returns the identifier of the home scheduler of the task
 * specified by ``task_id`` in ``scheduler_id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``scheduler_id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``task_id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_get_scheduler(
  rtems_id  task_id,
  rtems_id *scheduler_id
);

/* Generated from spec:/rtems/task/if/set-scheduler */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Sets the home scheduler for the task.
 *
 * @param task_id is the task identifier.  The constant #RTEMS_SELF may be used
 *   to specify the calling task.
 *
 * @param scheduler_id is the scheduler identifier of the new home scheduler
 *   for the task specified by ``task_id``.
 *
 * @param priority is the new real priority for the task with respect to the
 *   scheduler specified by ``scheduler_id``.
 *
 * This directive sets the home scheduler to the scheduler specified by
 * ``scheduler_id`` for the task specified by ``task_id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no scheduler associated with the
 *   identifier specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The task priority specified by ``priority``
 *   was invalid with respect to the scheduler specified by ``scheduler_id``.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``task_id``.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The task specified by ``task_id`` was
 *   enqueued on a wait queue.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The task specified by ``task_id`` had a
 *   current priority which consisted of more than the real priority.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The task specified by ``task_id`` had a
 *   helping scheduler.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The task specified by ``task_id`` was
 *   pinned.
 *
 * @retval ::RTEMS_UNSATISFIED The scheduler specified by ``scheduler_id``
 *   owned no processor.
 *
 * @retval ::RTEMS_UNSATISFIED The scheduler specified by ``scheduler_id`` did
 *   not support the affinity set of the task specified by ``task_id``.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may change the priority of a task.  This may cause the
 *   calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_set_scheduler(
  rtems_id            task_id,
  rtems_id            scheduler_id,
  rtems_task_priority priority
);

/* Generated from spec:/rtems/task/if/get-affinity */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Gets the processor affinity of the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * @param cpusetsize is the size of the processor set referenced by ``cpuset``
 *   in bytes.
 *
 * @param[out] cpuset is the pointer to a cpu_set_t object.  When the directive
 *   call is successful, the processor affinity set of the task will be stored
 *   in this object.  A set bit in the processor set means that the
 *   corresponding processor is in the processor affinity set of the task,
 *   otherwise the bit is cleared.
 *
 * This directive returns the processor affinity of the task in ``cpuset`` of
 * the task specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``cpuset`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_SIZE The size specified by ``cpusetsize`` of the
 *   processor set was too small for the processor affinity set of the task.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_get_affinity(
  rtems_id   id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
);

/* Generated from spec:/rtems/task/if/set-affinity */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Sets the processor affinity of the task.
 *
 * @param id is the task identifier.  The constant #RTEMS_SELF may be used to
 *   specify the calling task.
 *
 * @param cpusetsize is the size of the processor set referenced by ``cpuset``
 *   in bytes.
 *
 * @param cpuset is the pointer to a cpu_set_t object.  The processor set
 *   defines the new processor affinity set of the task.  A set bit in the
 *   processor set means that the corresponding processor shall be in the
 *   processor affinity set of the task, otherwise the bit shall be cleared.
 *
 * This directive sets the processor affinity of the task specified by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``cpuset`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_NUMBER The referenced processor set was not a valid
 *   new processor affinity set for the task.
 *
 * @retval ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT The task resided on a remote node.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may change the processor affinity of a task.  This may cause
 *   the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_task_set_affinity(
  rtems_id         id,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
);

/* Generated from spec:/rtems/task/if/iterate */

/**
 * @ingroup RTEMSAPIClassicTasks
 *
 * @brief Iterates over all tasks and invokes the visitor routine for each
 *   task.
 *
 * @param visitor is the visitor routine invoked for each task.
 *
 * @param arg is the argument passed to each visitor routine invocation during
 *   the iteration.
 *
 * This directive iterates over all tasks in the system.  This operation covers
 * all tasks of all APIs.  The user should be careful in accessing the contents
 * of the TCB.  The visitor argument ``arg`` is passed to all invocations of
 * ``visitor`` in addition to the TCB. The iteration stops immediately in case
 * the visitor routine returns true.
 *
 * @par Notes
 * The visitor routine is invoked while owning the objects allocator lock.  It
 * is allowed to perform blocking operations in the visitor routine, however,
 * care must be taken so that no deadlocks via the object allocator lock can
 * occur.
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
 * @endparblock
 */
void rtems_task_iterate( rtems_task_visitor visitor, void *arg );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_TASKS_H */
