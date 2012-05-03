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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with TIME_TEST_SUPPORT;
with RTEMS;
with RTEMS.TASKS;

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

   TIME_SET  : Standard.BOOLEAN;

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LOW_TASK);

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASKS);

end TMTEST;
