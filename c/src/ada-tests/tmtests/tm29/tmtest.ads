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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS;
with RTEMS.TASKS;

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
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

--
--  TASKS
--
--  DESCRIPTION:
--
--  The first of these RTEMS tasks starts the timer.  All of these
--  tasks execute a blocking RATE_MONOTONIC_PERIOD.
--

   procedure TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TASKS);

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task stops the timer and reports the execution
--  time for a blocking RATE_MONOTONIC_PERIOD.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LOW_TASK);

end TMTEST;
