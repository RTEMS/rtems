--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 4 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--  Copyright assigned to U.S. Government, 1994.
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  This array contains the IDs of all RTEMS tasks created by this test.
--

   TASK_ID : array ( RTEMS.UNSIGNED32 
      range 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;

--
--  This variable contains the id of the highest priority task
--  in the system.
-- 

   HIGHEST_ID : RTEMS.ID;

--
--  The following is the ID of the semaphore used for timing operations.
--

   SEMAPHORE_ID : RTEMS.ID;

--
--  The following is used to perform the same operation 
--  on a group of tasks.
--

   TASK_COUNT : RTEMS.UNSIGNED32;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME  : RTEMS.UNSIGNED32;

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TEST_INIT
--
--  DESCRIPTION:
--
--  This subprogram performs test initialization.
--

   procedure TEST_INIT;

--
--  HIGHEST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is created by one of the LOW_TASKS.  The 
--  first time it executes, it performs a blocking SEMAPHORE_OBTAIN
--  which transfers control of the processor to HIGH_TASK.
--  The first time HIGHEST_TASK is restarted, it stops the
--  timer and reports the execution time for a TASK_RESTART
--  for a blocked task which preempts the caller.  The second 
--  time HIGHEST_TASK is restarted, it stops the timer and reports 
--  the execution time for a TASK_RESTART for a ready task which 
--  has its original priority restored and preempts the caller.  
--

   procedure HIGHEST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is created by one of the LOW_TASKS.   This
--  task restarts HIGHEST_TASK twice.  HIGHEST_TASK is blocked the
--  first time and preempts this task when restarted.  HIGHEST_TASK
--  is ready the second time but has lowered its priority.  When 
--  restarted for the second time, HIGHEST_TASK will have its
--  original priority restored and preempt this task.
--  Once control of the processor is returned to this task after
--  HIGHEST_TASK deletes itself, the following execution times
--  are measured and reported.
--
--    + readying SEMAPHORE_RELEASE 
--    + TASK_CREATE
--    + TASK_START
--    + TASK_RESTART of a suspended task
--    + TASK_DELETE of a suspended task
--    + TASK_RESTART of a ready task
--    + TASK_RESTART of a blocked task which does not cause a preempt
--    + TASK_DELETE of a blocked task
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LOW_TASKS
--
--  DESCRIPTION:
--
--  The last of these RTEMS tasks to execute creates HIGH_TASK
--  and HIGHEST_TASK.  All of these tasks perform a blocking 
--  SEMAPHORE_OBTAIN.
--

   procedure LOW_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  RESTART_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task
--

   procedure RESTART_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  This is the Initialization Tasks Table for this test.
--

   INITIALIZATION_TASKS : aliased RTEMS.INITIALIZATION_TASKS_TABLE( 1 .. 1 ) := 
   (1=>
     (
       RTEMS.BUILD_NAME( 'U', 'I', '1', ' ' ),        -- task name
       2048,                                          -- stack size
       1,                                             -- priority
       RTEMS.DEFAULT_ATTRIBUTES,                      -- attributes
       TMTEST.INIT'ACCESS,                            -- entry point
       RTEMS.NO_PREEMPT,                              -- initial mode
       0                                              -- argument list
     )
   );

--
--  This is the Configuration Table for this test.
--

   CONFIGURATION : aliased RTEMS.CONFIGURATION_TABLE := (
      RTEMS.NULL_ADDRESS,        -- will be replaced by BSP
      256 * 1024,                -- executive RAM size
      111,                       -- maximum # tasks
      0,                         -- maximum # timers
      1,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
