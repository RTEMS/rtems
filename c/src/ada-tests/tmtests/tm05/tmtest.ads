--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 5 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
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
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the highest priority task in the system.  
--  It starts the timer and performs a TASK_SUSPEND on SELF.
--  When this task executes again, it stops the timer and
--  reports the execution time for a preemptive TASK_RESUME.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  MIDDLE_TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks perform a TASK_SUSPEND on SELF.  When each 
--  task executes again, it performs preemptive TASK_RESUME on
--  a higher priority task.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LOW_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the lowest priority task in the system.  
--  When this task executes, it stops the timer and reports the 
--  execution time for a blocking TASK_SUSPEND.  After this,
--  it starts the timer and performs a TASK_RESUME on a
--  higher priority task.
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end TMTEST;
