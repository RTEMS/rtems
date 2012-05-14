--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 7 of the RTEMS
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

   TASK_ID : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;

   TASK_INDEX : RTEMS.UNSIGNED32;

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
--  This subprogram performs test initialization.
--

   procedure TEST_INIT;

--
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the highest priority task in the system.  
--  The first time it executes it suspends itself.  When restarted
--  it stops the timer and reports the time for a TASK_RESTART
--  of a suspended task which results in a preemption.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, HIGH_TASK);

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks suspend themselves the first time they execute.
--  When restarted they perform a TASK_RESTART of a suspended 
--  higher priority task which results in a preemption.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, MIDDLE_TASKS);

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the lowest priority task in the system.  
--  When it executes it starts the timer and restarts a higher
--  priority task which immediately preempts this task.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, LOW_TASK);

end TMTEST;
