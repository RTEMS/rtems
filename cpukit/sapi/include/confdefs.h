/*  confdefs.h
 *
 *  This include file contains the configuration table template that will
 *  be instantiated by an application based on the setting of a number
 *  of macros.  The macros are documented in the Configuring a System
 *  chapter of the Classic API User's Guide
 *
 *  The model is to estimate the memory required for each configured item
 *  and sum those estimates.  The estimate can be too high or too low for
 *  a variety of reasons:
 *
 *  Reasons estimate is too high:
 *    + FP contexts (not all tasks are FP)
 *
 *  Reasons estimate is too low:
 *    + stacks greater than minimum size
 *    + messages
 *    + application must account for device driver resources
 *    + application must account for add-on library resource requirements
 *
 *  NOTE:  Eventually this may be able to take into account some of
 *         the above.  This procedure has evolved from just enough to
 *         support the RTEMS Test Suites into something that can be
 *         used remarkably reliably by most applications.
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Include the executive's configuration
 */
#include <rtems/score/cpuopts.h>

extern rtems_initialization_tasks_table Initialization_tasks[];
extern rtems_driver_address_table       Device_drivers[];
extern rtems_configuration_table        Configuration;
extern rtems_multiprocessing_table      Multiprocessing_configuration;
#ifdef RTEMS_POSIX_API
extern posix_api_configuration_table    Configuration_POSIX_API;
#endif
#ifdef RTEMS_ITRON_API
extern itron_api_configuration_table    Configuration_ITRON_API;
#endif

/*
 *  RTEMS C Library and Newlib support
 */

#ifdef RTEMS_NEWLIB
#define CONFIGURE_NEWLIB_EXTENSION 1
#else
#define CONFIGURE_NEWLIB_EXTENSION 0
#endif

#define CONFIGURE_MALLOC_REGION 1

/*
 *  File descriptors managed by libio
 */

#ifndef CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 3
#endif

#define CONFIGURE_LIBIO_SEMAPHORES \
  (CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS + 1)

#ifdef CONFIGURE_INIT
unsigned32 rtems_libio_number_iops = CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS;
#endif

/*
 *  Termios resources
 */

#ifdef CONFIGURE_TERMIOS_DISABLED
#define CONFIGURE_TERMIOS_SEMAPHORES 0
#else

#ifndef CONFIGURE_NUMBER_OF_TERMIOS_PORTS
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 1
#endif

#define CONFIGURE_TERMIOS_SEMAPHORES \
  ((CONFIGURE_NUMBER_OF_TERMIOS_PORTS * 4) + 1)
#endif

/*
 *  Mount Table Configuration
 */

#include <imfs.h>

#ifdef CONFIGURE_INIT

#ifndef CONFIGURE_HAS_OWN_MOUNT_TABLE
rtems_filesystem_mount_table_t configuration_mount_table = {
#ifdef CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
  &IMFS_ops,
#else  /* using miniIMFS as base filesystem */
  &miniIMFS_ops,
#endif
  RTEMS_FILESYSTEM_READ_WRITE,
  NULL,
  NULL
};

rtems_filesystem_mount_table_t 
    *rtems_filesystem_mount_table = &configuration_mount_table;
int rtems_filesystem_mount_table_size = 1;
#endif

#endif


/*
 *  Stack Checker Requirements
 *
 *  NOTE: This does not automatically enable reporting at program exit.
 */

#ifdef STACK_CHECKER_ON
#define CONFIGURE_STACK_CHECKER_EXTENSION 1
#else
#define CONFIGURE_STACK_CHECKER_EXTENSION 0
#endif

/*
 *  Interrupt Stack Space
 *
 *  NOTE: There is currently no way for the application to override
 *        the interrupt stack size set by the BSP.
 */

#if (CPU_ALLOCATE_INTERRUPT_STACK == 0)
#undef CONFIGURE_INTERRUPT_STACK_MEMORY
#define CONFIGURE_INTERRUPT_STACK_MEMORY 0
#else
  #ifndef CONFIGURE_INTERRUPT_STACK_MEMORY
  #define CONFIGURE_INTERRUPT_STACK_MEMORY RTEMS_MINIMUM_STACK_SIZE
  #endif
#endif

/*
 *  Default User Initialization Task Table.  This table guarantees that
 *  one user initialization table is defined.
 */

#ifdef CONFIGURE_RTEMS_INIT_TASKS_TABLE

#ifdef CONFIGURE_HAS_OWN_INIT_TASK_TABLE

/*
 *  The user is defining their own table information and setting the 
 *  appropriate variables.
 */

#else

#ifndef CONFIGURE_INIT_TASK_NAME
#define CONFIGURE_INIT_TASK_NAME          rtems_build_name( 'U', 'I', '1', ' ' )
#endif

#ifndef CONFIGURE_INIT_TASK_STACK_SIZE
#define CONFIGURE_INIT_TASK_STACK_SIZE    RTEMS_MINIMUM_STACK_SIZE 
#endif

#ifndef CONFIGURE_INIT_TASK_PRIORITY
#define CONFIGURE_INIT_TASK_PRIORITY      1
#endif

#ifndef CONFIGURE_INIT_TASK_ATTRIBUTES
#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_DEFAULT_ATTRIBUTES 
#endif

#ifndef CONFIGURE_INIT_TASK_ENTRY_POINT
#define CONFIGURE_INIT_TASK_ENTRY_POINT   Init 
#endif

#ifndef CONFIGURE_INIT_TASK_INITIAL_MODES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_PREEMPT 
#endif

#ifndef CONFIGURE_INIT_TASK_ARGUMENTS
#define CONFIGURE_INIT_TASK_ARGUMENTS     0 
#endif

#ifdef CONFIGURE_INIT
rtems_initialization_tasks_table Initialization_tasks[] = {
  { CONFIGURE_INIT_TASK_NAME,
    CONFIGURE_INIT_TASK_STACK_SIZE,
    CONFIGURE_INIT_TASK_PRIORITY,
    CONFIGURE_INIT_TASK_ATTRIBUTES,
    CONFIGURE_INIT_TASK_ENTRY_POINT,
    CONFIGURE_INIT_TASK_INITIAL_MODES,
    CONFIGURE_INIT_TASK_ARGUMENTS
  }
};
#endif

#define CONFIGURE_INIT_TASK_TABLE Initialization_tasks

#define CONFIGURE_INIT_TASK_TABLE_SIZE \
  sizeof(CONFIGURE_INIT_TASK_TABLE) / sizeof(rtems_initialization_tasks_table)

#endif    /* CONFIGURE_HAS_OWN_INIT_TASK_TABLE */

#else     /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

#define CONFIGURE_INIT_TASK_TABLE      NULL
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

#endif

/*
 *  Map obsolete names to current ones
 *
 *  NOTE: These should be obsoleted in a future release.
 */

#ifdef CONFIGURE_TEST_NEEDS_TIMER_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
#endif
#ifdef CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#endif 
#ifdef CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#endif 
#ifdef CONFIGURE_TEST_NEEDS_RTC_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
#endif 
#ifdef CONFIGURE_TEST_NEEDS_STUB_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#endif


/*
 *  Default Device Driver Table.  Each driver needed by the test is explicitly
 *  choosen by that test.  There is always a null driver entry.
 */

#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }

#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#include <console.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#include <clockdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
#include <timerdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
#include <rtc.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
#include <rtems/devnull.h>
#endif

#ifndef CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE

#ifdef CONFIGURE_INIT
rtems_driver_address_table Device_drivers[] = {
#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  CONSOLE_DRIVER_TABLE_ENTRY,
#endif
#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  CLOCK_DRIVER_TABLE_ENTRY,
#endif
#ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
  RTC_DRIVER_TABLE_ENTRY,
#endif
#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
  DEVNULL_DRIVER_TABLE_ENTRY,
#endif
  NULL_DRIVER_TABLE_ENTRY
};
#endif

#endif  /* CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE */

/*
 *  Default the number of drivers per node.  This value may be
 *  overridden by the user.
 */

#ifndef CONFIGURE_MAXIMUM_DRIVERS
#define CONFIGURE_MAXIMUM_DRIVERS   10
#endif

/*
 *  Default the number of devices per device driver.  This value may be
 *  overridden by the user.
 */

#ifndef CONFIGURE_MAXIMUM_DEVICES
#define CONFIGURE_MAXIMUM_DEVICES   20
#endif

/*
 *  Default Multiprocessing Configuration Table.  The defaults are
 *  appropriate for most of the RTEMS Multiprocessor Test Suite.  Each
 *  value may be overridden within each test to customize the environment. 
 */

#ifdef CONFIGURE_MP_APPLICATION
#ifndef CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE

#ifndef CONFIGURE_MP_NODE_NUMBER
#define CONFIGURE_MP_NODE_NUMBER                NODE_NUMBER
#endif

#ifndef CONFIGURE_MP_MAXIMUM_NODES
#define CONFIGURE_MP_MAXIMUM_NODES              2
#endif

#ifndef CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
#define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS     32
#endif

#ifndef CONFIGURE_MP_MAXIMUM_PROXIES
#define CONFIGURE_MP_MAXIMUM_PROXIES            32
#endif

#ifndef CONFIGURE_MP_MPCI_TABLE_POINTER
#include <mpci.h>
#define CONFIGURE_MP_MPCI_TABLE_POINTER         &MPCI_table
#endif

#ifdef CONFIGURE_INIT
rtems_multiprocessing_table Multiprocessing_configuration = {
  CONFIGURE_MP_NODE_NUMBER,              /* local node number */
  CONFIGURE_MP_MAXIMUM_NODES,            /* maximum # nodes in system */
  CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS,   /* maximum # global objects */
  CONFIGURE_MP_MAXIMUM_PROXIES,          /* maximum # proxies */
  CONFIGURE_MP_MPCI_TABLE_POINTER        /* pointer to MPCI config table */
};
#endif

#define CONFIGURE_MULTIPROCESSING_TABLE    &Multiprocessing_configuration

#endif /* CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE */

#else

#define CONFIGURE_MULTIPROCESSING_TABLE    NULL

#endif /* CONFIGURE_MP_APPLICATION */
 
/*
 *  Default Configuration Table.  
 */

#ifndef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE
 
#ifndef CONFIGURE_EXECUTIVE_RAM_WORK_AREA
#define CONFIGURE_EXECUTIVE_RAM_WORK_AREA     NULL
#endif

#ifndef CONFIGURE_MAXIMUM_TASKS
#define CONFIGURE_MAXIMUM_TASKS               0
#endif

#ifndef CONFIGURE_MAXIMUM_TIMERS
#define CONFIGURE_MAXIMUM_TIMERS              0
#endif

#ifndef CONFIGURE_MAXIMUM_SEMAPHORES
#define CONFIGURE_MAXIMUM_SEMAPHORES          0
#endif

#ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      0
#endif

#ifndef CONFIGURE_MAXIMUM_PARTITIONS
#define CONFIGURE_MAXIMUM_PARTITIONS          0
#endif

#ifndef CONFIGURE_MAXIMUM_REGIONS
#define CONFIGURE_MAXIMUM_REGIONS             0
#endif

#ifndef CONFIGURE_MAXIMUM_PORTS
#define CONFIGURE_MAXIMUM_PORTS               0
#endif

#ifndef CONFIGURE_MAXIMUM_PERIODS
#define CONFIGURE_MAXIMUM_PERIODS             0
#endif

#ifndef CONFIGURE_MAXIMUM_USER_EXTENSIONS
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#endif

#ifndef CONFIGURE_MICROSECONDS_PER_TICK 
#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(10)
#endif

#ifndef CONFIGURE_TICKS_PER_TIMESLICE
#define CONFIGURE_TICKS_PER_TIMESLICE        50
#endif

/*
 *  Initial Extension Set
 */

#ifdef CONFIGURE_INIT
#ifdef STACK_CHECKER_ON
#include <rtems/stackchk.h>
#endif

rtems_extensions_table Configuration_Initial_Extensions[] = {
#ifdef CONFIGURE_INITIAL_EXTENSIONS
    CONFIGURE_INITIAL_EXTENSIONS,
#endif
#ifdef STACK_CHECKER_ON
    STACK_CHECKER_EXTENSION,
#endif
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

#define CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS \
  ((sizeof(Configuration_Initial_Extensions) / \
    sizeof(rtems_extensions_table)) - 1)

#endif

/*
 *  POSIX API Configuration Parameters
 */

#ifdef RTEMS_POSIX_API

#include <sys/types.h>
#include <signal.h>
#include <mqueue.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/key.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/threadsup.h>

#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
#define CONFIGURE_MAXIMUM_POSIX_THREADS      0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_MUTEXES
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES      0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
#define CONFIGURE_MAXIMUM_POSIX_KEYS         0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_TIMERS
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 0
#endif

#ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE

#ifdef CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE

/*
 *  The user is defining their own table information and setting the
 *  appropriate variables for the POSIX Initialization Thread Table.
 */
 
#else

#ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
#define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT   POSIX_Init 
#endif

#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)
#endif

#ifdef CONFIGURE_INIT
posix_initialization_threads_table POSIX_Initialization_threads[] = {
  { CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT, \
      CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE }
};
#endif

#define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME POSIX_Initialization_threads
 
#define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE \
  sizeof(CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME) / \
      sizeof(posix_initialization_threads_table)

#endif    /* CONFIGURE_POSIX_HAS_OWN_INIT_TASK_TABLE */

#else     /* CONFIGURE_POSIX_INIT_THREAD_TABLE */

#define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME NULL
#define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE 0

#endif

#define CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API \
  ( \
    sizeof (POSIX_API_Control) + \
   (sizeof (void *) * (CONFIGURE_GNAT_KEYS + CONFIGURE_MAXIMUM_POSIX_KEYS)) \
  )

#define CONFIGURE_MEMORY_FOR_POSIX_MUTEXES(_mutexes) \
  ((_mutexes) * \
   ( sizeof(POSIX_Mutex_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES(_condition_variables) \
  ((_condition_variables) * \
   ( sizeof(POSIX_Condition_variables_Control) + \
        CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_POSIX_KEYS(_keys) \
  ((_keys) * \
   ( sizeof(POSIX_Keys_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_POSIX_TIMERS(_timers) \
  ((_timers) * \
   ( 0 ) )

#define CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS(_queued_signals) \
  ((_queued_signals) * \
   ( sizeof(POSIX_signals_Siginfo_node) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(_message_queues) \
  ((_message_queues) * \
   ( sizeof( POSIX_Message_queue_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )
#define CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES(_semaphores) \
  ((_semaphores) * \
   ( sizeof( POSIX_Semaphore_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )
  

#define CONFIGURE_MEMORY_FOR_POSIX \
  ( \
    CONFIGURE_MEMORY_FOR_POSIX_MUTEXES( CONFIGURE_MAXIMUM_POSIX_MUTEXES ) + \
    CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES( \
        CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES ) + \
    CONFIGURE_MEMORY_FOR_POSIX_KEYS( CONFIGURE_MAXIMUM_POSIX_KEYS ) + \
    CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS( \
        CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS ) + \
    CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES( \
        CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES ) + \
    CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES( \
        CONFIGURE_MAXIMUM_POSIX_SEMAPHORES ) + \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE) \
   )


#else

#define CONFIGURE_MAXIMUM_POSIX_THREADS         0
#define CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API 0
#define CONFIGURE_MEMORY_FOR_POSIX              0


#endif    /* RTEMS_POSIX_API */

#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE    0
#endif

/*
 *  ITRON API Configuration Parameters
 */

#ifdef RTEMS_ITRON_API

#include <itron.h>
#include <rtems/itron/config.h>
#include <rtems/itron/eventflags.h>
#include <rtems/itron/fmempool.h>
#include <rtems/itron/mbox.h>
#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/port.h>
#include <rtems/itron/semaphore.h>
#include <rtems/itron/task.h>
#include <rtems/itron/vmempool.h>

#ifndef CONFIGURE_MAXIMUM_ITRON_TASKS
#define CONFIGURE_MAXIMUM_ITRON_TASKS      0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_SEMAPHORES
#define CONFIGURE_MAXIMUM_ITRON_SEMAPHORES   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS
#define CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_MAILBOXES
#define CONFIGURE_MAXIMUM_ITRON_MAILBOXES   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS
#define CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_PORTS
#define CONFIGURE_MAXIMUM_ITRON_PORTS   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS
#define CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS   0
#endif

#ifndef CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS
#define CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS   0
#endif

#ifdef CONFIGURE_ITRON_INIT_TASK_TABLE

#ifdef CONFIGURE_ITRON_HAS_OWN_INIT_TASK_TABLE

/*
 *  The user is defining their own table information and setting the
 *  appropriate variables for the ITRON Initialization Task Table.
 */

#else

#ifndef CONFIGURE_ITRON_INIT_TASK_ENTRY_POINT
#define CONFIGURE_ITRON_INIT_TASK_ENTRY_POINT   ITRON_Init
#endif

#ifndef CONFIGURE_ITRON_INIT_TASK_ATTRIBUTES
#define CONFIGURE_ITRON_INIT_TASK_ATTRIBUTES    TA_HLNG
#endif

#ifndef CONFIGURE_ITRON_INIT_TASK_PRIORITY
#define CONFIGURE_ITRON_INIT_TASK_PRIORITY      1
#endif

#ifndef CONFIGURE_ITRON_INIT_TASK_STACK_SIZE
#define CONFIGURE_ITRON_INIT_TASK_STACK_SIZE    RTEMS_MINIMUM_STACK_SIZE
#endif

#ifdef CONFIGURE_INIT
itron_initialization_tasks_table ITRON_Initialization_tasks[] = {
  { 1,                                    /* ID */
    { (VP) 0,                                /* exinfo */
      CONFIGURE_ITRON_INIT_TASK_ATTRIBUTES,  /* task attributes */
      CONFIGURE_ITRON_INIT_TASK_ENTRY_POINT, /* task start address */
      CONFIGURE_ITRON_INIT_TASK_PRIORITY,    /* initial task priority */
      CONFIGURE_ITRON_INIT_TASK_STACK_SIZE   /* stack size */
    }
  }
};
#endif

#define CONFIGURE_ITRON_INIT_TASK_TABLE_NAME ITRON_Initialization_tasks

#define CONFIGURE_ITRON_INIT_TASK_TABLE_SIZE \
  sizeof(CONFIGURE_ITRON_INIT_TASK_TABLE_NAME) / \
      sizeof(itron_initialization_tasks_table)

#endif    /* CONFIGURE_ITRON_HAS_OWN_INIT_TASK_TABLE */

#else     /* CONFIGURE_ITRON_INIT_TASK_TABLE */

#define CONFIGURE_ITRON_INIT_TASK_TABLE_NAME NULL
#define CONFIGURE_ITRON_INIT_TASK_TABLE_SIZE 0

#endif

#define CONFIGURE_MEMORY_PER_TASK_FOR_ITRON_API \
  ( \
    sizeof (ITRON_API_Control) \
  )

#define CONFIGURE_MEMORY_FOR_ITRON_SEMAPHORES(_semaphores) \
  ((_semaphores) * \
   ( sizeof(ITRON_Semaphore_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_ITRON_EVENTFLAGS(_eventflags) \
  ((_eventflags) * \
   ( sizeof(ITRON_Eventflags_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_ITRON_MAILBOXES(_mailboxes) \
  ((_mailboxes) * \
   ( sizeof(ITRON_Mailbox_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_ITRON_MESSAGE_BUFFERS(_message_buffers) \
  ((_message_buffers) * \
   ( sizeof(ITRON_Message_buffer_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_ITRON_PORTS(_ports) \
  ((_ports) * \
   ( sizeof(ITRON_Port_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_ITRON_MEMORY_POOLS(_memory_pools) \
  ((_memory_pools) * \
   (sizeof(ITRON_Variable_memory_pool_Control) + CONFIGURE_OBJECT_TABLE_STUFF))

#define CONFIGURE_MEMORY_FOR_ITRON_FIXED_MEMORY_POOLS(_fixed_memory_pools) \
  ((_fixed_memory_pools) * \
   ( sizeof(ITRON_Fixed_memory_pool_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#ifndef CONFIGURE_ITRON_INIT_TASK_STACK_SIZE
#define CONFIGURE_ITRON_INIT_TASK_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE * 2)
#endif


#define CONFIGURE_MEMORY_FOR_ITRON \
  ( \
    CONFIGURE_MEMORY_FOR_ITRON_SEMAPHORES( \
        CONFIGURE_MAXIMUM_ITRON_SEMAPHORES ) + \
    CONFIGURE_MEMORY_FOR_ITRON_EVENTFLAGS( \
        CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS ) + \
    CONFIGURE_MEMORY_FOR_ITRON_MAILBOXES( \
        CONFIGURE_MAXIMUM_ITRON_MAILBOXES ) + \
    CONFIGURE_MEMORY_FOR_ITRON_MESSAGE_BUFFERS( \
        CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS ) + \
    CONFIGURE_MEMORY_FOR_ITRON_PORTS( \
        CONFIGURE_MAXIMUM_ITRON_PORTS ) + \
    CONFIGURE_MEMORY_FOR_ITRON_MEMORY_POOLS( \
        CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS ) + \
    CONFIGURE_MEMORY_FOR_ITRON_FIXED_MEMORY_POOLS( \
        CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS ) + \
    CONFIGURE_ITRON_INIT_TASK_STACK_SIZE \
   )


#else

#define CONFIGURE_MAXIMUM_ITRON_TASKS               0
#define CONFIGURE_MAXIMUM_ITRON_SEMAPHORES          0
#define CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS          0
#define CONFIGURE_MAXIMUM_ITRON_MAILBOXES           0
#define CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS     0
#define CONFIGURE_MAXIMUM_ITRON_PORTS               0
#define CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS        0
#define CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS  0
#define CONFIGURE_MEMORY_PER_TASK_FOR_ITRON_API     0
#define CONFIGURE_MEMORY_FOR_ITRON                  0

#endif    /* RTEMS_ITRON_API */


/* 
 *  Calculate the RAM size based on the maximum number of objects configured.
 */

#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

#define CONFIGURE_OBJECT_TABLE_STUFF \
  ( sizeof(Objects_Control *) + sizeof(rtems_name *) + sizeof(rtems_name) )

#define CONFIGURE_MEMORY_FOR_TASKS(_tasks) \
  (((_tasks) + 1 ) * \
   ((sizeof(Thread_Control) + CONTEXT_FP_SIZE + \
      STACK_MINIMUM_SIZE + sizeof( RTEMS_API_Control ) + \
      CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API + \
      CONFIGURE_MEMORY_PER_TASK_FOR_ITRON_API + \
      CONFIGURE_OBJECT_TABLE_STUFF)) \
  )

#define CONFIGURE_MEMORY_FOR_TIMERS(_timers) \
  ((_timers) * ( sizeof(Timer_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_SEMAPHORES(_semaphores) \
  ((_semaphores) * \
   ( sizeof(Semaphore_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(_queues) \
  ( (_queues) * \
    ( sizeof(Message_queue_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_PARTITIONS(_partitions) \
  ( (_partitions) * \
    ( sizeof(Partition_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_REGIONS(_regions) \
  ( (_regions) * \
    ( sizeof(Region_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_PORTS(_ports) \
  ( (_ports) * \
    ( sizeof(Dual_ported_memory_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_PERIODS(_periods) \
  ( (_periods) * \
    ( sizeof(Rate_monotonic_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(_extensions) \
  ( (_extensions) * \
    ( sizeof(Extension_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

#define CONFIGURE_MEMORY_FOR_DEVICES(_devices) \
  (((_devices) + 1) * ( sizeof(rtems_driver_name_t) ) )

#ifdef CONFIGURE_MP_APPLICATION

#ifndef CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE

#define CONFIGURE_MEMORY_FOR_PROXIES(_proxies) \
  ( ((_proxies) + 1) * ( sizeof(Thread_Proxy_control) )  )

#define CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(_global_objects) \
  ((_global_objects)  * ( sizeof(Objects_MP_Control) )  )

#define CONFIGURE_MEMORY_FOR_MP \
  ( CONFIGURE_MEMORY_FOR_PROXIES(CONFIGURE_MP_MAXIMUM_PROXIES) + \
    CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS) + \
    CONFIGURE_MEMORY_FOR_TASKS(1) \
  )

#endif  /* CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE */

#else

#define CONFIGURE_MEMORY_FOR_MP  0

#endif

#ifndef CONFIGURE_MEMORY_OVERHEAD
#define CONFIGURE_MEMORY_OVERHEAD 0
#endif

#ifndef CONFIGURE_EXTRA_TASK_STACKS
#define CONFIGURE_EXTRA_TASK_STACKS 0
#endif

#define CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD \
  ( CONFIGURE_MEMORY_FOR_TASKS(1) +    /* IDLE */ \
    (256 * 12) +                       /* Ready chains */ \
    256        +                       /* name/ptr table overhead */ \
    CONFIGURE_INTERRUPT_STACK_MEMORY   /* interrupt stack */ \
  )

/*
 *  Now account for any extra memory that initialization tasks or threads
 *  may have requested.
 */

#ifdef RTEMS_POSIX_API
#define CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE - RTEMS_MINIMUM_STACK_SIZE)
#else
#define CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART 0
#endif
#ifdef RTEMS_ITRON_API
#define CONFIGURE_INITIALIZATION_THREADS_STACKS_ITRON_PART \
    (CONFIGURE_ITRON_INIT_TASK_STACK_SIZE - RTEMS_MINIMUM_STACK_SIZE)
#else
#define CONFIGURE_INITIALIZATION_THREADS_STACKS_ITRON_PART 0
#endif

#define CONFIGURE_INITIALIZATION_THREADS_STACKS \
   ((CONFIGURE_INIT_TASK_STACK_SIZE - RTEMS_MINIMUM_STACK_SIZE) + \
    CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART + \
    CONFIGURE_INITIALIZATION_THREADS_STACKS_ITRON_PART)

#define CONFIGURE_EXECUTIVE_RAM_SIZE \
(( CONFIGURE_MEMORY_FOR_POSIX + \
   CONFIGURE_MEMORY_FOR_ITRON + \
   CONFIGURE_MEMORY_FOR_TASKS(CONFIGURE_MAXIMUM_TASKS + \
      CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_ADA_TASKS + \
      CONFIGURE_MAXIMUM_ITRON_TASKS ) + \
   CONFIGURE_INITIALIZATION_THREADS_STACKS + \
   CONFIGURE_MEMORY_FOR_TIMERS(CONFIGURE_MAXIMUM_TIMERS) + \
   CONFIGURE_MEMORY_FOR_SEMAPHORES(CONFIGURE_MAXIMUM_SEMAPHORES + \
     CONFIGURE_LIBIO_SEMAPHORES + CONFIGURE_TERMIOS_SEMAPHORES) + \
   CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(CONFIGURE_MAXIMUM_MESSAGE_QUEUES) + \
   CONFIGURE_MEMORY_FOR_PARTITIONS(CONFIGURE_MAXIMUM_PARTITIONS) + \
   CONFIGURE_MEMORY_FOR_REGIONS( \
      CONFIGURE_MAXIMUM_REGIONS + CONFIGURE_MALLOC_REGION ) + \
   CONFIGURE_MEMORY_FOR_PORTS(CONFIGURE_MAXIMUM_PORTS) + \
   CONFIGURE_MEMORY_FOR_PERIODS(CONFIGURE_MAXIMUM_PERIODS) + \
   CONFIGURE_MEMORY_FOR_USER_EXTENSIONS( \
      CONFIGURE_MAXIMUM_USER_EXTENSIONS + CONFIGURE_NEWLIB_EXTENSION + \
      CONFIGURE_STACK_CHECKER_EXTENSION ) + \
   CONFIGURE_MEMORY_FOR_DEVICES(CONFIGURE_MAXIMUM_DEVICES) + \
   CONFIGURE_MEMORY_FOR_MP + \
   CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD + \
   (((CONFIGURE_MEMORY_OVERHEAD)+1) * 1024) + \
   (CONFIGURE_EXTRA_TASK_STACKS) + (CONFIGURE_ADA_TASKS_STACK) \
) & 0xfffffc00)
#endif

#ifdef CONFIGURE_GNAT_RTEMS
#define CONFIGURE_GNAT_MUTEXES 10
#define CONFIGURE_GNAT_KEYS    1

/* 20 are required to run all tests in the ACVC */
#ifndef CONFIGURE_MAXIMUM_ADA_TASKS
#define CONFIGURE_MAXIMUM_ADA_TASKS  20
#endif

/* This is the number of non-Ada tasks which invoked Ada code. */
#ifndef CONFIGURE_MAXIMUM_FAKE_ADA_TASKS
#define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
#endif

/* Ada tasks are allocated twice the minimum stack space */
#define CONFIGURE_ADA_TASKS_STACK \
  (CONFIGURE_MAXIMUM_ADA_TASKS * (RTEMS_MINIMUM_STACK_SIZE + (6 * 1024)))

#else
#define CONFIGURE_GNAT_MUTEXES           0
#define CONFIGURE_GNAT_KEYS              0
#define CONFIGURE_MAXIMUM_ADA_TASKS      0
#define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
#define CONFIGURE_ADA_TASKS_STACK        0
#endif


#ifdef CONFIGURE_INIT

rtems_api_configuration_table Configuration_RTEMS_API = {
  CONFIGURE_MAXIMUM_TASKS,
  CONFIGURE_MAXIMUM_TIMERS,
  CONFIGURE_MAXIMUM_SEMAPHORES + CONFIGURE_LIBIO_SEMAPHORES +
    CONFIGURE_TERMIOS_SEMAPHORES,
  CONFIGURE_MAXIMUM_MESSAGE_QUEUES,
  CONFIGURE_MAXIMUM_PARTITIONS,
  CONFIGURE_MAXIMUM_REGIONS + CONFIGURE_MALLOC_REGION,
  CONFIGURE_MAXIMUM_PORTS,
  CONFIGURE_MAXIMUM_PERIODS,
  CONFIGURE_INIT_TASK_TABLE_SIZE,
  CONFIGURE_INIT_TASK_TABLE
};

#ifdef RTEMS_POSIX_API
posix_api_configuration_table Configuration_POSIX_API = {
  CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_ADA_TASKS,
  CONFIGURE_MAXIMUM_POSIX_MUTEXES + CONFIGURE_GNAT_MUTEXES +
    CONFIGURE_MAXIMUM_ADA_TASKS + CONFIGURE_MAXIMUM_FAKE_ADA_TASKS,
  CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES +
    CONFIGURE_MAXIMUM_ADA_TASKS + CONFIGURE_MAXIMUM_FAKE_ADA_TASKS,
  CONFIGURE_MAXIMUM_POSIX_KEYS + CONFIGURE_GNAT_KEYS,
  CONFIGURE_MAXIMUM_POSIX_TIMERS,
  CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS,
  CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES,
  CONFIGURE_MAXIMUM_POSIX_SEMAPHORES,
  CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE,
  CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME
};
#endif

#ifdef RTEMS_ITRON_API
itron_api_configuration_table Configuration_ITRON_API = {
  CONFIGURE_MAXIMUM_ITRON_TASKS,
  CONFIGURE_MAXIMUM_ITRON_SEMAPHORES,
  CONFIGURE_MAXIMUM_ITRON_EVENTFLAGS,
  CONFIGURE_MAXIMUM_ITRON_MAILBOXES,
  CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS,
  CONFIGURE_MAXIMUM_ITRON_PORTS,
  CONFIGURE_MAXIMUM_ITRON_MEMORY_POOLS,
  CONFIGURE_MAXIMUM_ITRON_FIXED_MEMORY_POOLS,
  CONFIGURE_ITRON_INIT_TASK_TABLE_SIZE,
  CONFIGURE_ITRON_INIT_TASK_TABLE_NAME
};
#endif

rtems_configuration_table Configuration = {
  CONFIGURE_EXECUTIVE_RAM_WORK_AREA,
  CONFIGURE_EXECUTIVE_RAM_SIZE,
  CONFIGURE_MAXIMUM_USER_EXTENSIONS + CONFIGURE_NEWLIB_EXTENSION + 
      CONFIGURE_STACK_CHECKER_EXTENSION,
  CONFIGURE_MICROSECONDS_PER_TICK,
  CONFIGURE_TICKS_PER_TIMESLICE,
  CONFIGURE_MAXIMUM_DEVICES,
  CONFIGURE_MAXIMUM_DRIVERS,
  sizeof (Device_drivers)/
    sizeof(rtems_driver_address_table),      /* number of device drivers */
  Device_drivers,                            /* pointer to driver table */
  CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS,    /* number of initial extensions */
  Configuration_Initial_Extensions,          /* pointer to initial extensions */
  CONFIGURE_MULTIPROCESSING_TABLE,           /* pointer to MP config table */
  &Configuration_RTEMS_API,                  /* pointer to RTEMS API config */
#ifdef RTEMS_POSIX_API
  &Configuration_POSIX_API,                  /* pointer to POSIX API config */
#else
  NULL,                                      /* pointer to POSIX API config */
#endif
#ifdef RTEMS_ITRON_API
  &Configuration_ITRON_API                   /* pointer to ITRON API config */
#else
  NULL                                       /* pointer to ITRON API config */
#endif
};
#endif

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

#ifdef __cplusplus
}
#endif
 
/*
 *  Some warnings and error checking
 */

/*
 *  Make sure a task/thread of some sort is configured
 */

#if (CONFIGURE_MAXIMUM_TASKS == 0) && \
    (CONFIGURE_MAXIMUM_POSIX_THREADS == 0) && \
    (CONFIGURE_MAXIMUM_ADA_TASKS == 0) &&  \
    (CONFIGURE_MAXIMUM_ITRON_TASKS == 0)
#error "CONFIGURATION ERROR: No tasks or threads configured!!"
#endif

/*
 *  Make sure at least one of the initialization task/thread
 *  tables was defined.
 */

#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) && \
    !defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) && \
    !defined(CONFIGURE_ITRON_INIT_TASK_TABLE)
#error "CONFIGURATION ERROR: No initialization tasks or threads configured!!"
#endif

#endif
/* end of include file */
