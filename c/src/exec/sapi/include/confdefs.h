/*  confdefs.h
 *
 *  This include file contains the configuration table template that will
 *  be used by the single processor tests to define its default configuration
 *  parameters.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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
 
extern rtems_initialization_tasks_table Initialization_tasks[];
extern rtems_driver_address_table       Device_drivers[];
extern rtems_configuration_table        Configuration;
extern rtems_multiprocessing_table      Multiprocessing_configuration;
#ifdef RTEMS_POSIX_API
extern posix_api_configuration_table    Configuration_POSIX_API;
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

#endif

/*
 *  Default Device Driver Table.  Each driver needed by the test is explicitly
 *  choosen by that test.  There is always a null driver entry.
 */

#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }

#ifdef CONFIGURE_TEST_NEEDS_TIMER_DRIVER
#include <timerdrv.h>
#endif

#ifdef CONFIGURE_TEST_NEEDS_STUB_DRIVER
#include <stubdrv.h>
#endif

#ifndef CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE

#ifdef CONFIGURE_INIT
rtems_driver_address_table Device_drivers[] = {
#ifdef CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
  CONSOLE_DRIVER_TABLE_ENTRY,
#endif
#ifdef CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
  CLOCK_DRIVER_TABLE_ENTRY,
#endif
#ifdef CONFIGURE_TEST_NEEDS_STUB_DRIVER
  STUB_DRIVER_TABLE_ENTRY,
#endif
  NULL_DRIVER_TABLE_ENTRY
};
#endif

#endif  /* CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE */

/*
 *  Default the number of devices per device driver.  This value may be
 *  overridden by the user.
 */

#ifndef CONFIGURE_HAS_OWN_NUMBER_OF_DEVICES

#ifndef CONFIGURE_MAXIMUM_DEVICES
#define CONFIGURE_MAXIMUM_DEVICES   20
#endif

#endif /* CONFIGURE_HAS_OWN_NUMBER_OF_DEVICES */

/*
 *  Default Configuration Table.  This table contains the most values set in
 *  the RTEMS Test Suite.  Each value may be overridden within each test to
 *  customize the environment. 
 */

#ifdef CONFIGURE_MPTEST
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

#endif /* CONFIGURE_MPTEST */
 
/*
 *  Default Configuration Table.  This table contains the most values set in
 *  the RTEMS Test Suite.  Each value may be overridden within each test to
 *  customize the environment. 
 */

#ifndef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE
 
#ifndef CONFIGURE_EXECUTIVE_RAM_WORK_AREA
#define CONFIGURE_EXECUTIVE_RAM_WORK_AREA     NULL
#endif

#ifndef CONFIGURE_MAXIMUM_TASKS
#define CONFIGURE_MAXIMUM_TASKS               10
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

#ifndef CONFIGURE_INITIAL_EXTENSIONS  
#define CONFIGURE_INITIAL_EXTENSIONS         NULL
#endif

/*
 *  POSIX API Configuration Parameters
 */

#ifdef RTEMS_POSIX_API

#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
#define CONFIGURE_MAXIMUM_POSIX_THREADS      10
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

#ifndef CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 0
#endif

#ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE

#ifdef CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE

/*
 *  The user is defining their own table information and setting the
 *  appropriate variables.
 */
 
#else

#ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
#define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT   POSIX_Init 
#endif

#ifdef CONFIGURE_INIT
posix_initialization_threads_table POSIX_Initialization_threads[] = {
  { CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT }
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

#endif    /* RTEMS_POSIX_API */

/* 
 *  Calculate the RAM size based on the maximum number of objects configured.
 *  The model is to estimate the memory required for each configured item,
 *  sum the memory requirements and insure that there is at least 32K greater 
 *  than that for things not directly addressed such as:
 *
 *    + stacks greater than minimum size
 *    + FP contexts
 *    + API areas (should be optional)
 *    + messages
 *    + object name and local pointer table overhead
 *    + per node memory requirements
 *    + executive fixed requirements (including at least internal threads
 *       and the Ready chains)
 *
 *  NOTE:  Eventually this should take into account some of the above.
 *         Basically, this is a "back of the envelope" estimate for
 *         memory requirements.  It could be more accurate.
 */

#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

#define CONFIGURE_OBJECT_TABLE_STUFF \
  ( sizeof(Objects_Control *) + sizeof(rtems_name *) + sizeof(rtems_name) )

#define CONFIGURE_MEMORY_FOR_TASKS(_tasks) \
  ((_tasks) * \
   ((sizeof(Thread_Control) + CONTEXT_FP_SIZE + \
      STACK_MINIMUM_SIZE + sizeof( RTEMS_API_Control ) + \
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

#ifdef CONFIGURE_MPTEST

#ifndef CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE

#define CONFIGURE_MEMORY_FOR_PROXIES(_proxies) \
  ( ((_proxies) + 1) * ( sizeof(Thread_Proxy_control) )  )

#define CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(_global_objects) \
  ((_global_objects)  * ( sizeof(Objects_MP_Control) )  )

#define CONFIGURE_MEMORY_FOR_MP \
  ( CONFIGURE_MEMORY_FOR_PROXIES(CONFIGURE_MP_MAXIMUM_PROXIES) + \
    CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS) \
  )

#endif  /* CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE */

#else

#define CONFIGURE_MEMORY_FOR_MP  0

#endif
#ifndef CONFIGURE_MEMORY_OVERHEAD
#define CONFIGURE_MEMORY_OVERHEAD 0
#endif

#define CONFIGURE_EXECUTIVE_RAM_SIZE \
(( CONFIGURE_MEMORY_FOR_TASKS(CONFIGURE_MAXIMUM_TASKS) + \
   CONFIGURE_MEMORY_FOR_TIMERS(CONFIGURE_MAXIMUM_TIMERS) + \
   CONFIGURE_MEMORY_FOR_SEMAPHORES(CONFIGURE_MAXIMUM_SEMAPHORES) + \
   CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(CONFIGURE_MAXIMUM_MESSAGE_QUEUES) + \
   CONFIGURE_MEMORY_FOR_PARTITIONS(CONFIGURE_MAXIMUM_PARTITIONS) + \
   CONFIGURE_MEMORY_FOR_REGIONS(CONFIGURE_MAXIMUM_REGIONS) + \
   CONFIGURE_MEMORY_FOR_PORTS(CONFIGURE_MAXIMUM_PORTS) + \
   CONFIGURE_MEMORY_FOR_PERIODS(CONFIGURE_MAXIMUM_PERIODS) + \
   CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(CONFIGURE_MAXIMUM_USER_EXTENSIONS) + \
   CONFIGURE_MEMORY_FOR_DEVICES(CONFIGURE_MAXIMUM_DEVICES) + \
   CONFIGURE_MEMORY_FOR_MP + \
   (((CONFIGURE_MEMORY_OVERHEAD)+1) * 1024) \
) & 0xfffffc00)
#endif

#ifdef CONFIGURE_GNAT_RTEMS
#define CONFIGURE_GNAT_MUTEXES 10
#define CONFIGURE_GNAT_KEYS    1

/* 20 are required to run all tests in the ACVC */
#ifndef CONFIGURE_MAXIMUM_ADA_TASKS
#define CONFIGURE_MAXIMUM_ADA_TASKS  20
#endif

#else
#define CONFIGURE_GNAT_MUTEXES           0
#define CONFIGURE_GNAT_KEYS              0
#define CONFIGURE_MAXIMUM_ADA_TASKS      0
#define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
#endif


#ifdef CONFIGURE_INIT

rtems_api_configuration_table Configuration_RTEMS_API = {
  CONFIGURE_MAXIMUM_TASKS,
  CONFIGURE_MAXIMUM_TIMERS,
  CONFIGURE_MAXIMUM_SEMAPHORES,
  CONFIGURE_MAXIMUM_MESSAGE_QUEUES,
  CONFIGURE_MAXIMUM_PARTITIONS,
  CONFIGURE_MAXIMUM_REGIONS,
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
  CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS,
  CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE,
  CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME
};
#endif

rtems_configuration_table Configuration = {
  CONFIGURE_EXECUTIVE_RAM_WORK_AREA,
  CONFIGURE_EXECUTIVE_RAM_SIZE,
  CONFIGURE_MAXIMUM_USER_EXTENSIONS,
  CONFIGURE_MICROSECONDS_PER_TICK,
  CONFIGURE_TICKS_PER_TIMESLICE,
  CONFIGURE_MAXIMUM_DEVICES,
  sizeof (Device_drivers)/
    sizeof(rtems_driver_address_table),      /* number of device drivers */
  Device_drivers,                            /* pointer to driver table */
  CONFIGURE_INITIAL_EXTENSIONS,              /* pointer to initial extensions */
  CONFIGURE_MULTIPROCESSING_TABLE,           /* pointer to MP config table */
  &Configuration_RTEMS_API,                  /* pointer to RTEMS API config */
#ifdef RTEMS_POSIX_API
  &Configuration_POSIX_API                   /* pointer to POSIX API config */
#else
  NULL                                       /* pointer to POSIX API config */
#endif
};
#endif

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

#ifdef __cplusplus
}
#endif
 
#endif
/* end of include file */
