--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 29 of the RTEMS
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

with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  The following is the name used for all periods created by this test.
--

   PERIOD_NAME : RTEMS.NAME;

--
--  The following is used to allow all tasks to create and start
--  their period timers before the timer is started for timing 
--  the blocking case of RATE_MONOTONIC_PERIOD.
--

   TASK_COUNT  : RTEMS.UNSIGNED32;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME    : RTEMS.UNSIGNED32;

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.  It also measures and
--  reports the following directive execution times:
--
--    + RATE_MONOTONIC_CREATE
--    + RATE_MONOTONIC_PERIOD the initial time
--    + RATE_MONOTONIC_PERIOD for a period's status
--    + RATE_MONOTONIC_CANCEL
--    + RATE_MONOTONIC_DELETE of a cancelled period
--    + RATE_MONOTONIC_DELETE of an active period
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TEST_TASK
--
--  DESCRIPTION:
--
--  The first of these RTEMS tasks starts the timer.  All of these
--  tasks execute a blocking RATE_MONOTONIC_PERIOD.
--

   procedure TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TEST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for a blocking RATE_MONOTONIC_PERIOD.
--

   procedure LOW_TASK (
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
      0,                         -- maximum # semaphores
      0,                         -- maximum # message queues
      0,                         -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      111,                       -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
