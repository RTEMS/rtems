/*  confdefs.h
 *
 *  This include file contains the configuration table template that will
 *  be used by the single processor tests to define its default configuration
 *  parameters.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

/*
 *  Default User Initialization Task Table.  This table guarantees that
 *  one user initialization table is defined.
 */

#ifndef CONFIGURE_HAS_OWN_INIT_TASK_TABLE

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

/* Calculate the RAM size based on the maximum number of tasks configured */

#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE
#define CONFIGURE_MEMORY_REQUIRED(_tasks) \
  (_tasks) * ( (sizeof(Thread_Control) + CONTEXT_FP_SIZE + STACK_MINIMUM_SIZE)) 
#define CONFIGURE_EXECUTIVE_RAM_SIZE \
( (CONFIGURE_MEMORY_REQUIRED(CONFIGURE_MAXIMUM_TASKS) + (128*1024)) &0xffff0000)
#endif

#ifdef CONFIGURE_INIT
rtems_configuration_table Configuration = {
  CONFIGURE_EXECUTIVE_RAM_WORK_AREA,
  CONFIGURE_EXECUTIVE_RAM_SIZE,
  CONFIGURE_MAXIMUM_TASKS,
  CONFIGURE_MAXIMUM_TIMERS,
  CONFIGURE_MAXIMUM_SEMAPHORES,
  CONFIGURE_MAXIMUM_MESSAGE_QUEUES,
  CONFIGURE_MAXIMUM_PARTITIONS,
  CONFIGURE_MAXIMUM_REGIONS,
  CONFIGURE_MAXIMUM_PORTS,
  CONFIGURE_MAXIMUM_PERIODS,
  CONFIGURE_MAXIMUM_USER_EXTENSIONS,
  CONFIGURE_MICROSECONDS_PER_TICK,
  CONFIGURE_TICKS_PER_TIMESLICE,
  sizeof (Initialization_tasks)/
   sizeof(rtems_initialization_tasks_table), /* number of init tasks */
  Initialization_tasks,                      /* init task(s) table */
  sizeof (Device_drivers)/
    sizeof(rtems_driver_address_table),      /* number of device drivers */
  CONFIGURE_MAXIMUM_DEVICES,
  Device_drivers,                            /* pointer to driver table */
  CONFIGURE_INITIAL_EXTENSIONS,              /* pointer to initial extensions */
  CONFIGURE_MULTIPROCESSING_TABLE            /* ptr to MP config table */
};
#endif

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

#ifdef __cplusplus
}
#endif
 
#endif
/* end of include file */
