/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

rtems_task Task_4(
  rtems_task_argument argument
);

rtems_isr Service_routine(
  rtems_vector_number ignored
);

rtems_timer_service_routine Io_during_interrupt(
  rtems_id ignored
);

void Screen1( void );

void Screen2( void );

void Screen3( void );

void Screen4( void );

void Screen5( void );

void Screen6( void );

void Screen7( void );

void Screen8( void );

void Screen9( void );

void Screen10( void );

void Screen11( void );

void Screen12( void );

void Screen13( void );

void Screen14( void );

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              10
#define CONFIGURE_MAXIMUM_TIMERS              1
#define CONFIGURE_MAXIMUM_SEMAPHORES          2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      1
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#define CONFIGURE_MAXIMUM_REGIONS             1
#define CONFIGURE_MAXIMUM_PERIODS             1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (20 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 11 ];       /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 11 ];     /* array of task names */

TEST_EXTERN rtems_name Timer_name[ 2 ];     /* array of timer names */
TEST_EXTERN rtems_id   Timer_id[ 2 ];       /* array of timer ids */

TEST_EXTERN rtems_name Semaphore_name[ 4 ]; /* array of semaphore names */
TEST_EXTERN rtems_id   Semaphore_id[ 4 ];   /* array of semaphore ids */

TEST_EXTERN rtems_name Queue_name[ 3 ];     /* array of queue names */
TEST_EXTERN rtems_id   Queue_id[ 3 ];       /* array of queue ids */

TEST_EXTERN rtems_name Partition_name[ 2 ]; /* array of partition names */
TEST_EXTERN rtems_id   Partition_id[ 2 ];   /* array of partition ids */

TEST_EXTERN rtems_name Region_name[ 2 ];    /* array of region names */
TEST_EXTERN rtems_id   Region_id[ 2 ];      /* array of region ids */

TEST_EXTERN rtems_name Port_name[ 2 ];      /* array of port names */
TEST_EXTERN rtems_id   Port_id[ 2 ];        /* array of port ids */

TEST_EXTERN rtems_name Period_name[ 2 ];    /* array of period names */
TEST_EXTERN rtems_id   Period_id[ 2 ];      /* array of period ids */

TEST_EXTERN rtems_id   Junk_id;             /* id used to return errors */

#define Internal_port_area (void *) 0x00001000
#define External_port_area (void *) 0x00002000

TEST_EXTERN uint8_t   Partition_good_area[256] CPU_STRUCTURE_ALIGNMENT;
#define Partition_bad_area (void *) 0x00000005

TEST_EXTERN uint32_t   Region_good_area[4096] CPU_STRUCTURE_ALIGNMENT;
#define Region_bad_area    (void *) 0x00000005
#define REGION_START_OFFSET 1024
#define REGION_LENGTH       512

/* end of include file */
