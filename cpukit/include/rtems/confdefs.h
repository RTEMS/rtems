/**
 * @file
 *
 * @brief Configuration Table Template that will be Instantiated
 * by an Application
 *
 * This include file contains the configuration table template that will
 * be instantiated by an application based on the setting of a number
 * of macros.  The macros are documented in the Configuring a System
 * chapter of the Classic API User's Guide
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h

/*
 * Include the executive's configuration
 */
#include <rtems.h>
#include <rtems/sysinit.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/context.h>
#include <rtems/score/wkspace.h>
#include <rtems/confdefs/obsolete.h>
#include <rtems/confdefs/bdbuf.h>
#include <rtems/confdefs/clock.h>
#include <rtems/confdefs/console.h>
#include <rtems/confdefs/extensions.h>
#include <rtems/confdefs/inittask.h>
#include <rtems/confdefs/initthread.h>
#include <rtems/confdefs/iodrivers.h>
#include <rtems/confdefs/libio.h>
#include <rtems/confdefs/libpci.h>
#include <rtems/confdefs/malloc.h>
#include <rtems/confdefs/mpci.h>
#include <rtems/confdefs/newlib.h>
#include <rtems/confdefs/objectsclassic.h>
#include <rtems/confdefs/objectsposix.h>
#include <rtems/confdefs/percpu.h>
#include <rtems/confdefs/scheduler.h>
#include <rtems/confdefs/threads.h>
#include <rtems/confdefs/wkspacesupport.h>

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal defines must be prefixed with _CONFIGURE to distinguish them from
 * user-provided options which use a CONFIGURE prefix.
 */

/**
 * @defgroup Configuration RTEMS Configuration
 *
 * This module contains all RTEMS Configuration parameters.
 *
 * The model is to estimate the memory required for each configured item
 * and sum those estimates.  The estimate can be too high or too low for
 * a variety of reasons:
 *
 * Reasons estimate is too high:
 *   + FP contexts (not all tasks are FP)
 *
 * Reasons estimate is too low:
 *   + stacks greater than minimum size
 *   + messages
 *   + application must account for device driver resources
 *   + application must account for add-on library resource requirements
 *
 * NOTE:  Eventually this may be able to take into account some of
 *        the above.  This procedure has evolved from just enough to
 *        support the RTEMS Test Suites into something that can be
 *        used remarkably reliably by most applications.
 */

/**
 * @defgroup ConfigurationHelpers Configuration Helpers
 *
 * @ingroup Configuration
 *
 * This module contains items which are used internally to ease
 * the configuration calculations.
 */
/**@{*/

/**
 * This is a helper macro used in stack space calculations in this file.  It
 * may be provided by the application in case a special task stack allocator
 * is used.  The default is allocation from the RTEMS Workspace.
 */
#ifdef CONFIGURE_TASK_STACK_FROM_ALLOCATOR
  #define _Configure_From_stackspace(_stack_size) \
    CONFIGURE_TASK_STACK_FROM_ALLOCATOR(_stack_size + CONTEXT_FP_SIZE)
#else
  #define _Configure_From_stackspace(_stack_size) \
    _Configure_From_workspace(_stack_size + CONTEXT_FP_SIZE)
#endif
/**@}*/

/**
 * This is so we can account for tasks with stacks greater than minimum
 * size.  This is in bytes.
 */
#ifndef CONFIGURE_EXTRA_TASK_STACKS
  #define CONFIGURE_EXTRA_TASK_STACKS 0
#endif

/*
 * We must be able to split the free block used for the second last allocation
 * into two parts so that we have a free block for the last allocation.  See
 * _Heap_Block_split().
 */
#define _CONFIGURE_HEAP_HANDLER_OVERHEAD \
  _Configure_Align_up( HEAP_BLOCK_HEADER_SIZE, CPU_HEAP_ALIGNMENT )

/*
 *  Calculate the RAM size based on the maximum number of objects configured.
 */
#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

/**
 * The following macro is used to calculate the memory allocated by RTEMS
 * for the message buffers associated with a particular message queue.
 * There is a fixed amount of overhead per message.
 */
#define CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(_messages, _size) \
    _Configure_From_workspace( \
      (_messages) * (_Configure_Align_up(_size, sizeof(uintptr_t)) \
        + sizeof(CORE_message_queue_Buffer_control)))

/*
 * This macro is set to the amount of memory required for pending message
 * buffers in bytes.  It should be constructed by adding together a
 * set of values determined by CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE.
 */
#ifndef CONFIGURE_MESSAGE_BUFFER_MEMORY
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY 0
#endif

/**
 * This macro is available just in case the confdefs.h file underallocates
 * memory for a particular application.  This lets the user add some extra
 * memory in case something broken and underestimates.
 *
 * It is also possible for cases where confdefs.h overallocates memory,
 * you could substract memory from the allocated.  The estimate is just
 * that, an estimate, and assumes worst case alignment and padding on
 * each allocated element.  So in some cases it could be too conservative.
 *
 * NOTE: Historically this was used for message buffers.
 */
#ifndef CONFIGURE_MEMORY_OVERHEAD
  #define CONFIGURE_MEMORY_OVERHEAD 0
#endif

/**
 * This calculates the memory required for the executive workspace.
 *
 * This is an internal parameter.
 */
#define CONFIGURE_EXECUTIVE_RAM_SIZE \
( \
   _CONFIGURE_MEMORY_FOR_POSIX_OBJECTS + \
   CONFIGURE_MESSAGE_BUFFER_MEMORY + \
   (CONFIGURE_MEMORY_OVERHEAD * 1024) + \
   _CONFIGURE_HEAP_HANDLER_OVERHEAD \
)

/*
 * This macro provides a summation of the various initialization task
 * and thread stack requirements.
 */
#define _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS \
    (_CONFIGURE_INIT_TASK_STACK_EXTRA + \
    _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA)

/*
 * This macro is calculated to specify the memory required for
 * the stacks of all tasks.
 */
#define _CONFIGURE_TASKS_STACK \
  (rtems_resource_maximum_per_allocation( _CONFIGURE_TASKS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_TASK_STACK_SIZE ) )

/*
 * This macro is calculated to specify the memory required for
 * the stacks of all POSIX threads.
 */
#define _CONFIGURE_POSIX_THREADS_STACK \
  (rtems_resource_maximum_per_allocation( CONFIGURE_MAXIMUM_POSIX_THREADS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

#else /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#define _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS 0
#define _CONFIGURE_TASKS_STACK 0
#define _CONFIGURE_POSIX_THREADS_STACK 0

#if CONFIGURE_EXTRA_TASK_STACKS != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_TASK_STACKS"
#endif

#endif /* CONFIGURE_EXECUTIVE_RAM_SIZE */

/*
 * This macro is calculated to specify the memory required for
 * all tasks and threads of all varieties.
 */
#define _CONFIGURE_STACK_SPACE_SIZE \
  ( \
    _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS + \
    _CONFIGURE_TASKS_STACK + \
    _CONFIGURE_POSIX_THREADS_STACK + \
    _CONFIGURE_LIBBLOCK_TASKS_STACK_EXTRA + \
    CONFIGURE_EXTRA_TASK_STACKS + \
    _CONFIGURE_HEAP_HANDLER_OVERHEAD \
  )

#ifdef CONFIGURE_INIT
  /**
   * This variable specifies the minimum stack size for tasks in an RTEMS
   * application.
   *
   * NOTE: This is left as a simple uint32_t so it can be externed as
   *       needed without requring being high enough logical to
   *       include the full configuration table.
   */
  uint32_t rtems_minimum_stack_size =
    CONFIGURE_MINIMUM_TASK_STACK_SIZE;

  const uintptr_t _Stack_Space_size = _CONFIGURE_STACK_SPACE_SIZE;

  #if defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
    && defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
    #ifdef CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
      const bool _Stack_Allocator_avoids_workspace = true;
    #else
      const bool _Stack_Allocator_avoids_workspace = false;
    #endif

    #ifdef CONFIGURE_TASK_STACK_ALLOCATOR_INIT
      const Stack_Allocator_initialize _Stack_Allocator_initialize =
        CONFIGURE_TASK_STACK_ALLOCATOR_INIT;
    #else
      const Stack_Allocator_initialize _Stack_Allocator_initialize = NULL;
    #endif

    const Stack_Allocator_allocate _Stack_Allocator_allocate =
      CONFIGURE_TASK_STACK_ALLOCATOR;

    const Stack_Allocator_free _Stack_Allocator_free =
      CONFIGURE_TASK_STACK_DEALLOCATOR;
  #elif defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
    || defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
    #error "CONFIGURE_TASK_STACK_ALLOCATOR and CONFIGURE_TASK_STACK_DEALLOCATOR must be both defined or both undefined"
  #endif

  const uintptr_t _Workspace_Size = CONFIGURE_EXECUTIVE_RAM_SIZE;

  #ifdef CONFIGURE_UNIFIED_WORK_AREAS
    const bool _Workspace_Is_unified = true;
  #endif

  #ifdef CONFIGURE_DIRTY_MEMORY
    RTEMS_SYSINIT_ITEM(
      _Memory_Dirty_free_areas,
      RTEMS_SYSINIT_DIRTY_MEMORY,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif

  #ifdef CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY
    const bool _Memory_Zero_before_use = true;

    RTEMS_SYSINIT_ITEM(
      _Memory_Zero_free_areas,
      RTEMS_SYSINIT_ZERO_MEMORY,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
