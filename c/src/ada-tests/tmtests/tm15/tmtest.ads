--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 15 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 
      TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;

--
--   This variable is set to TRUE if the timer has been initialized
--   and is currently timing an operation.
--

   TIME_SET  : RTEMS.BOOLEAN;

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
--  This subprogram performs test initialization.  Once initialization
--  has been completed, the following operations are timed and reported:
--
--    + event_receive of currently PENDING_EVENTS
--    + event_receive NO_WAIT case
--

   procedure TEST_INIT;

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution time 
--  of a blocking EVENT_RECEIVE.  After this the following
--  operations are timed and the execution time is reported:
--  
--    + EVENT_SEND which does not ready a task
--    + EVENT_RECEIVE with the event condition available
--    + EVENT_SEND which readies a task
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  HIGH_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks executive a blocking EVENT_RECEIVE after
--  initialization is complete.  The first of these tasks to 
--  execute starts the timer.
--

   procedure HIGH_TASKS (
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
      1,                         -- maximum # message queues
      100,                       -- maximum # messages
      0,                         -- maximum # partitions
      0,                         -- maximum # regions
      0,                         -- maximum # dp memory areas
      0,                         -- maximum # periods
      0,                         -- maximum # user extensions
      RTEMS.MILLISECONDS_TO_MICROSECONDS(10), -- # us in a tick
      0                          -- # ticks in a timeslice
  );

end TMTEST;
