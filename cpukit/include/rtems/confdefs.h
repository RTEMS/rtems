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
#include <rtems/ioimpl.h>
#include <rtems/sysinit.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/context.h>
#include <rtems/score/wkspace.h>
#include <rtems/rtems/barrierdata.h>
#include <rtems/rtems/dpmemdata.h>
#include <rtems/rtems/messagedata.h>
#include <rtems/rtems/partdata.h>
#include <rtems/rtems/ratemondata.h>
#include <rtems/rtems/regiondata.h>
#include <rtems/rtems/semdata.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/rtems/timerdata.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/shm.h>
#include <rtems/posix/timer.h>
#include <rtems/confdefs/obsolete.h>
#include <rtems/confdefs/bdbuf.h>
#include <rtems/confdefs/clock.h>
#include <rtems/confdefs/extensions.h>
#include <rtems/confdefs/inittask.h>
#include <rtems/confdefs/libio.h>
#include <rtems/confdefs/libpci.h>
#include <rtems/confdefs/malloc.h>
#include <rtems/confdefs/mpci.h>
#include <rtems/confdefs/newlib.h>
#include <rtems/confdefs/percpu.h>
#include <rtems/confdefs/scheduler.h>

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
 * Zero of one returns 0 if the parameter is 0 else 1 is returned.
 */
#define _Configure_Zero_or_One(_number) ((_number) != 0 ? 1 : 0)

/**
 * General helper to align up a value.
 */
#define _Configure_Align_up(_val, _align) \
  (((_val) + (_align) - 1) - ((_val) + (_align) - 1) % (_align))

#define _CONFIGURE_HEAP_MIN_BLOCK_SIZE \
  _Configure_Align_up(sizeof(Heap_Block), CPU_HEAP_ALIGNMENT)

/**
 * This is a helper macro used in calculations in this file.  It is used
 * to noted when an element is allocated from the RTEMS Workspace and adds
 * a factor to account for heap overhead plus an alignment factor that
 * may be applied.
 */
#define _Configure_From_workspace(_size) \
  (uintptr_t) (_Configure_Zero_or_One(_size) * \
    _Configure_Align_up(_size + HEAP_BLOCK_HEADER_SIZE, \
      _CONFIGURE_HEAP_MIN_BLOCK_SIZE))

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
 * @defgroup ConfigurationDriverTable Device Driver Table Configuration
 *
 * @addtogroup Configuration
 *
 * This group contains parameters related to generating a Device Driver
 * Table.
 *
 * Default Device Driver Table.  Each driver needed by the test is explicitly
 * chosen by the application.  There is always a null driver entry.
 */
/**@{*/

/**
 * This is an empty device driver slot.
 */
#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }

#if (defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER)) || \
  (defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER)) || \
  (defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER))
#error "CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER, CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER, and CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER are mutually exclusive"
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #include <rtems/console.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
  #include <rtems/console.h>

  #ifdef CONFIGURE_INIT
    RTEMS_SYSINIT_ITEM(
      _Console_simple_Initialize,
      RTEMS_SYSINIT_DEVICE_DRIVERS,
      RTEMS_SYSINIT_ORDER_SECOND
    );
  #endif
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
  #include <rtems/console.h>

  #ifdef CONFIGURE_INIT
    RTEMS_SYSINIT_ITEM(
      _Console_simple_task_Initialize,
      RTEMS_SYSINIT_DEVICE_DRIVERS,
      RTEMS_SYSINIT_ORDER_SECOND
    );
  #endif
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
  #include <rtems/btimer.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
  #include <rtems/rtc.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
  #include <rtems/watchdogdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
  #include <rtems/framebuffer.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
  #include <rtems/devnull.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
  #include <rtems/devzero.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
  /* the ide driver needs the ATA driver */
  #ifndef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
    #define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  #endif
  #include <libchip/ide_ctrl.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  #include <libchip/ata.h>
#endif

/**
 * This specifies the maximum number of device drivers that
 * can be installed in the system at one time.  It must account
 * for both the statically and dynamically installed drivers.
 */
#ifndef CONFIGURE_MAXIMUM_DRIVERS
  #define CONFIGURE_MAXIMUM_DRIVERS
#endif

#ifdef CONFIGURE_INIT
  rtems_driver_address_table
    _IO_Driver_address_table[ CONFIGURE_MAXIMUM_DRIVERS ] = {
    #ifdef CONFIGURE_BSP_PREREQUISITE_DRIVERS
      CONFIGURE_BSP_PREREQUISITE_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS
      CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
      CONSOLE_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
      RTC_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
      WATCHDOG_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
      DEVNULL_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
      DEVZERO_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
      IDE_CONTROLLER_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
      ATA_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
      FRAME_BUFFER_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_EXTRA_DRIVERS
      CONFIGURE_APPLICATION_EXTRA_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER
      NULL_DRIVER_TABLE_ENTRY
    #elif !defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_EXTRA_DRIVERS)
      NULL_DRIVER_TABLE_ENTRY
    #endif
  };

  const size_t _IO_Number_of_drivers =
    RTEMS_ARRAY_SIZE( _IO_Driver_address_table );
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  /*
   * configure the priority of the ATA driver task
   */
  #ifndef CONFIGURE_ATA_DRIVER_TASK_PRIORITY
    #define CONFIGURE_ATA_DRIVER_TASK_PRIORITY ATA_DRIVER_TASK_DEFAULT_PRIORITY
  #endif
  #ifdef CONFIGURE_INIT
    rtems_task_priority rtems_ata_driver_task_priority
      = CONFIGURE_ATA_DRIVER_TASK_PRIORITY;
  #endif /* CONFIGURE_INIT */
#endif
/**@}*/ /* end of Device Driver Table Configuration */

/**
 * This macro specifies that the user wants to use unlimited objects for any
 * classic or posix objects that have not already been given resource limits.
 */
#if defined(CONFIGURE_UNLIMITED_OBJECTS)
  #if !defined(CONFIGURE_UNIFIED_WORK_AREAS) && \
     !defined(CONFIGURE_EXECUTIVE_RAM_SIZE) && \
     !defined(CONFIGURE_MEMORY_OVERHEAD)
     #error "CONFIGURE_UNLIMITED_OBJECTS requires a unified work area, an executive RAM size, or a defined workspace memory overhead"
  #endif

  #if !defined(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  /**
   * This macro specifies a default allocation size for when auto-extending
   * unlimited objects if none was given by the user.
   */
    #define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 8
  #endif
  #if !defined(CONFIGURE_MAXIMUM_TASKS)
    #define CONFIGURE_MAXIMUM_TASKS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_TIMERS)
    #define CONFIGURE_MAXIMUM_TIMERS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_SEMAPHORES)
    #define CONFIGURE_MAXIMUM_SEMAPHORES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_MESSAGE_QUEUES)
    #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PARTITIONS)
    #define CONFIGURE_MAXIMUM_PARTITIONS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_REGIONS)
    #define CONFIGURE_MAXIMUM_REGIONS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PORTS)
    #define CONFIGURE_MAXIMUM_PORTS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PERIODS)
    #define CONFIGURE_MAXIMUM_PERIODS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_BARRIERS)
    #define CONFIGURE_MAXIMUM_BARRIERS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_KEYS)
    #define CONFIGURE_MAXIMUM_POSIX_KEYS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS)
    #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES)
    #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_SEMAPHORES)
    #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_SHMS)
    #define CONFIGURE_MAXIMUM_POSIX_SHMS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_THREADS)
    #define CONFIGURE_MAXIMUM_POSIX_THREADS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif

  #ifdef RTEMS_POSIX_API
    #if !defined(CONFIGURE_MAXIMUM_POSIX_TIMERS)
      #define CONFIGURE_MAXIMUM_POSIX_TIMERS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
  #endif /* RTEMS_POSIX_API */
#endif /* CONFIGURE_UNLIMITED_OBJECTS */


/**
 * @defgroup ConfigurationClassicAPI Classic API Configuration
 *
 * @ingroup Configuration
 *
 * This module contains the parameters related to configuration
 * of the Classic API services.
 */
/**@{*/

/** This configures the maximum number of Classic API tasks. */
#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS               0
#endif

/*
 * This is calculated to account for the maximum number of Classic API
 * tasks used by the application and configured RTEMS capabilities.
 */
#define _CONFIGURE_TASKS \
  (CONFIGURE_MAXIMUM_TASKS + _CONFIGURE_LIBBLOCK_TASKS)

#ifndef CONFIGURE_MAXIMUM_TIMERS
  /** This specifies the maximum number of Classic API timers. */
  #define CONFIGURE_MAXIMUM_TIMERS             0
#endif

#ifndef CONFIGURE_MAXIMUM_SEMAPHORES
  /** This specifies the maximum number of Classic API semaphores. */
  #define CONFIGURE_MAXIMUM_SEMAPHORES                 0
#endif

#ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Message Queues.
   */
  #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES             0
#endif

#ifndef CONFIGURE_MAXIMUM_PARTITIONS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Partitions.
   */
  #define CONFIGURE_MAXIMUM_PARTITIONS                 0
#endif

#ifndef CONFIGURE_MAXIMUM_REGIONS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Regions.
   */
  #define CONFIGURE_MAXIMUM_REGIONS              0
#endif

#ifndef CONFIGURE_MAXIMUM_PORTS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Dual-Ported Memory Ports.
   */
  #define CONFIGURE_MAXIMUM_PORTS            0
#endif

#ifndef CONFIGURE_MAXIMUM_PERIODS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Rate Monotonic Periods.
   */
  #define CONFIGURE_MAXIMUM_PERIODS              0
#endif

/**
 * This configuration parameter specifies the maximum number of
 * Classic API Barriers.
 */
#ifndef CONFIGURE_MAXIMUM_BARRIERS
  #define CONFIGURE_MAXIMUM_BARRIERS               0
#endif

/**@}*/ /* end of Classic API Configuration */

/**
 * @defgroup ConfigurationPOSIXAPI POSIX API Configuration Parameters
 *
 * This module contains the parameters related to configuration
 * of the POSIX API services.
 */
/**@{*/

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API keys.
 *
 * POSIX Keys are available whether or not the POSIX API is enabled.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
  #define CONFIGURE_MAXIMUM_POSIX_KEYS 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API key/value pairs.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
  #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
    (CONFIGURE_MAXIMUM_POSIX_KEYS * \
     (CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_TASKS))
#endif

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API threads.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
  #define CONFIGURE_MAXIMUM_POSIX_THREADS 0
#endif

/*
 * Account for the object control structures plus the name
 * of the object to be duplicated.
 */
#define _Configure_POSIX_Named_Object_RAM(_number, _size) \
  (rtems_resource_maximum_per_allocation(_number) \
    * _Configure_From_workspace(_POSIX_PATH_MAX + 1))

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API message queues.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API message queues.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(_message_queues) \
  _Configure_POSIX_Named_Object_RAM( \
     _message_queues, sizeof(POSIX_Message_queue_Control) )

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API semaphores.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
  #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API semaphores.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES(_semaphores) \
  _Configure_POSIX_Named_Object_RAM( \
     _semaphores, sizeof(POSIX_Semaphore_Control) )

/**
 * Configure the maximum number of POSIX shared memory objects.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_SHMS
  #define CONFIGURE_MAXIMUM_POSIX_SHMS 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API shared memory.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_SHMS(_shms) \
  _Configure_POSIX_Named_Object_RAM(_shms, sizeof(POSIX_Shm_Control) )

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API timers.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_TIMERS
  #define CONFIGURE_MAXIMUM_POSIX_TIMERS 0
#endif

#if !defined(RTEMS_POSIX_API) && CONFIGURE_MAXIMUM_POSIX_TIMERS != 0
  #error "CONFIGURE_MAXIMUM_POSIX_TIMERS must be zero if POSIX API is disabled"
#endif

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API queued signals.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
  #define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 0
#endif

#if !defined(RTEMS_POSIX_API) && CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS != 0
  #error "CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS must be zero if POSIX API is disabled"
#endif

#if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS \
    _Configure_From_workspace( (CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS) * \
      sizeof( POSIX_signals_Siginfo_node ) )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS 0
#endif

#ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE
  #ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
    #define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT   POSIX_Init
  #endif

  #ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
    #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
  #endif
#endif /* CONFIGURE_POSIX_INIT_THREAD_TABLE */

/**
 * This configuration parameter specifies the stack size of the
 * POSIX API Initialization thread (if used).
 */
#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
  #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE    0
#endif
/**@}*/  /* end of POSIX API Configuration */

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
   _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES( \
     CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES) + \
   _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES( \
     CONFIGURE_MAXIMUM_POSIX_SEMAPHORES) + \
   _CONFIGURE_MEMORY_FOR_POSIX_SHMS( \
     CONFIGURE_MAXIMUM_POSIX_SHMS) + \
   _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS + \
   CONFIGURE_MESSAGE_BUFFER_MEMORY + \
   (CONFIGURE_MEMORY_OVERHEAD * 1024) + \
   _CONFIGURE_HEAP_HANDLER_OVERHEAD \
)

/*
 *  Now account for any extra memory that initialization tasks or threads
 *  may have requested.
 */

/*
 * This accounts for any extra memory required by the POSIX API
 * Initialization Thread.
 */
#if (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE > \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE - \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
#else
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART 0
#endif

/*
 * This macro provides a summation of the various initialization task
 * and thread stack requirements.
 */
#define _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS \
    (_CONFIGURE_INIT_TASK_STACK_EXTRA + \
    _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART)

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

#ifndef CONFIGURE_MAXIMUM_THREAD_NAME_SIZE
  #define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE THREAD_DEFAULT_MAXIMUM_NAME_SIZE
#endif

#ifdef CONFIGURE_INIT
  typedef union {
    Scheduler_Node Base;
    #ifdef CONFIGURE_SCHEDULER_CBS
      Scheduler_CBS_Node CBS;
    #endif
    #ifdef CONFIGURE_SCHEDULER_EDF
      Scheduler_EDF_Node EDF;
    #endif
    #ifdef CONFIGURE_SCHEDULER_EDF_SMP
      Scheduler_EDF_SMP_Node EDF_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY
      Scheduler_priority_Node Priority;
    #endif
    #ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
      Scheduler_SMP_Node Simple_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
      Scheduler_priority_SMP_Node Priority_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
      Scheduler_priority_affinity_SMP_Node Priority_affinity_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_STRONG_APA
      Scheduler_strong_APA_Node Strong_APA;
    #endif
    #ifdef CONFIGURE_SCHEDULER_USER_PER_THREAD
      CONFIGURE_SCHEDULER_USER_PER_THREAD User;
    #endif
  } Configuration_Scheduler_node;

  #ifdef RTEMS_SMP
    const size_t _Scheduler_Node_size = sizeof( Configuration_Scheduler_node );
  #endif

  const size_t _Thread_Maximum_name_size = CONFIGURE_MAXIMUM_THREAD_NAME_SIZE;

  struct Thread_Configured_control {
    Thread_Control Control;
    #if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
      void *extensions[ CONFIGURE_MAXIMUM_USER_EXTENSIONS + 1 ];
    #endif
    Configuration_Scheduler_node Scheduler_nodes[ _CONFIGURE_SCHEDULER_COUNT ];
    RTEMS_API_Control API_RTEMS;
    #ifdef RTEMS_POSIX_API
      POSIX_API_Control API_POSIX;
    #endif
    #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
      char name[ CONFIGURE_MAXIMUM_THREAD_NAME_SIZE ];
    #endif
    #ifdef _CONFIGURE_ENABLE_NEWLIB_REENTRANCY
      struct _reent Newlib;
    #else
      struct { /* Empty */ } Newlib;
    #endif
  };

  const Thread_Control_add_on _Thread_Control_add_ons[] = {
    {
      offsetof( Thread_Configured_control, Control.Scheduler.nodes ),
      offsetof( Thread_Configured_control, Scheduler_nodes )
    }, {
      offsetof(
        Thread_Configured_control,
        Control.API_Extensions[ THREAD_API_RTEMS ]
      ),
      offsetof( Thread_Configured_control, API_RTEMS )
    }, {
      offsetof(
        Thread_Configured_control,
        Control.libc_reent
      ),
      offsetof( Thread_Configured_control, Newlib )
    }
    #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
      , {
        offsetof(
          Thread_Configured_control,
          Control.Join_queue.Queue.name
        ),
        offsetof( Thread_Configured_control, name )
      }
    #endif
    #ifdef RTEMS_POSIX_API
      , {
        offsetof(
          Thread_Configured_control,
          Control.API_Extensions[ THREAD_API_POSIX ]
        ),
        offsetof( Thread_Configured_control, API_POSIX )
      }
    #endif
  };

  const size_t _Thread_Control_add_on_count =
    RTEMS_ARRAY_SIZE( _Thread_Control_add_ons );

  #if defined(RTEMS_SMP)
    struct Thread_queue_Configured_heads {
      Thread_queue_Heads Heads;
        Thread_queue_Priority_queue Priority[ _CONFIGURE_SCHEDULER_COUNT ];
    };

    const size_t _Thread_queue_Heads_size =
      sizeof( Thread_queue_Configured_heads );
  #endif

  const size_t _Thread_Initial_thread_count =
    rtems_resource_maximum_per_allocation( _CONFIGURE_TASKS ) +
    rtems_resource_maximum_per_allocation( CONFIGURE_MAXIMUM_POSIX_THREADS );

  THREAD_INFORMATION_DEFINE(
    _Thread,
    OBJECTS_INTERNAL_API,
    OBJECTS_INTERNAL_THREADS,
    _CONFIGURE_MAXIMUM_PROCESSORS + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT
  );

  #if CONFIGURE_MAXIMUM_BARRIERS > 0
    BARRIER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_BARRIERS );
  #endif

  #if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
    MESSAGE_QUEUE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_MESSAGE_QUEUES );
  #endif

  #if CONFIGURE_MAXIMUM_PARTITIONS > 0
    PARTITION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PARTITIONS );
  #endif

  #if CONFIGURE_MAXIMUM_PERIODS > 0
    RATE_MONOTONIC_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PERIODS );
  #endif

  #if CONFIGURE_MAXIMUM_PORTS > 0
    DUAL_PORTED_MEMORY_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PORTS );
  #endif

  #if CONFIGURE_MAXIMUM_REGIONS > 0
    REGION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_REGIONS );
  #endif

  #if CONFIGURE_MAXIMUM_SEMAPHORES > 0
    SEMAPHORE_INFORMATION_DEFINE(
      CONFIGURE_MAXIMUM_SEMAPHORES,
      _CONFIGURE_SCHEDULER_COUNT
    );
  #endif

  #if CONFIGURE_MAXIMUM_TIMERS > 0
    TIMER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_TIMERS );
  #endif

  #if _CONFIGURE_TASKS > 0
    THREAD_INFORMATION_DEFINE(
      _RTEMS_tasks,
      OBJECTS_CLASSIC_API,
      OBJECTS_RTEMS_TASKS,
      _CONFIGURE_TASKS
    );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS > 0
    POSIX_Keys_Key_value_pair _POSIX_Keys_Key_value_pairs[
      rtems_resource_maximum_per_allocation(
        CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
      )
    ];

    const uint32_t _POSIX_Keys_Key_value_pair_maximum =
      CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS;
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_KEYS > 0
    POSIX_KEYS_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_KEYS );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES > 0
    POSIX_MESSAGE_QUEUE_INFORMATION_DEFINE(
      CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
    );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_SEMAPHORES > 0
    POSIX_SEMAPHORE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SEMAPHORES );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_SHMS > 0
    POSIX_SHM_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SHMS );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_THREADS > 0
    THREAD_INFORMATION_DEFINE(
      _POSIX_Threads,
      OBJECTS_POSIX_API,
      OBJECTS_POSIX_THREADS,
      CONFIGURE_MAXIMUM_POSIX_THREADS
    );
  #endif

  #ifdef RTEMS_POSIX_API
    #if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
      const uint32_t _POSIX_signals_Maximum_queued_signals =
        CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS;

      POSIX_signals_Siginfo_node _POSIX_signals_Siginfo_nodes[
        CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
      ];
    #endif

    #if CONFIGURE_MAXIMUM_POSIX_TIMERS > 0
      POSIX_TIMER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_TIMERS );
    #endif
  #endif

  const size_t _POSIX_Threads_Minimum_stack_size =
    CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE;

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

/*
 *  If the user has configured a set of POSIX Initialization Threads,
 *  then we need to install the code that runs that loop.
 */
#ifdef CONFIGURE_INIT
  #if defined(CONFIGURE_POSIX_INIT_THREAD_TABLE)
    const posix_initialization_threads_table _POSIX_Threads_User_thread_table = {
      CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT,
      CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
    };

    RTEMS_SYSINIT_ITEM(
      _POSIX_Threads_Initialize_user_thread,
      RTEMS_SYSINIT_POSIX_USER_THREADS,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif

#ifdef __cplusplus
}
#endif

/******************************************************************
 ******************************************************************
 ******************************************************************
 *         CONFIGURATION WARNINGS AND ERROR CHECKING              *
 ******************************************************************
 ******************************************************************
 ******************************************************************
 */

/*
 *  Make sure a task/thread of some sort is configured.
 *
 *  When analyzing RTEMS to find the smallest possible of memory
 *  that must be allocated, you probably do want to configure 0
 *  tasks/threads so there is a smaller set of calls to _Workspace_Allocate
 *  to analyze.
 */
#if !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION) \
  && CONFIGURE_MAXIMUM_TASKS == 0 \
  && CONFIGURE_MAXIMUM_POSIX_THREADS == 0
  #error "CONFIGURATION ERROR: No tasks or threads configured!!"
#endif

#ifndef RTEMS_SCHEDSIM
/*
 *  Make sure at least one of the initialization task/thread
 *  tables was defined.
 */
#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) && \
    !defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) && \
    !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION)
#error "CONFIGURATION ERROR: No initialization tasks or threads configured!!"
#endif
#endif

/*
 * POSIX Key pair shouldn't be less than POSIX Key, which is highly
 * likely to be error.
 */
#if (CONFIGURE_MAXIMUM_POSIX_KEYS != 0) && \
    (CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS != 0)
  #if (CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS < CONFIGURE_MAXIMUM_POSIX_KEYS)
    #error "Fewer POSIX Key pairs than POSIX Key!"
  #endif
#endif

#endif
/* end of include file */
