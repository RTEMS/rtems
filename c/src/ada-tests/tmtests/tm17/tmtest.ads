--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 17 of the RTEMS
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
--  http://www.rtems.com/license/LICENSE.
--
--  $Id$
--

with RTEMS;
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;

package TMTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 
      range 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT ) of RTEMS.NAME;

--
--  These variables are used to index the TASK_ID array and change
--  the priority of each successive task such that the TASK_SET_PRIORITY
--  results in a preemption.
--

   TASK_COUNT     : RTEMS.UNSIGNED32;
   TASK_PRIORITY  : RTEMS.TASK_PRIORITY;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME       : RTEMS.UNSIGNED32;

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
--  FIRST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the first task to execute in the system.  It
--  starts the timer and updates the TASK_COUNT and TASK_PRIORITY 
--  variables so that a preemptive TASK_SET_PRIORITY can be performed.
--

   procedure FIRST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  MIDDLE_TASK
--
--  DESCRIPTION:
--
--  These RTEMS tasks update the TASK_COUNT and TASK_PRIORITY variables
--  so that a preemptive TASK_SET_PRIORITY can be performed.
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  LAST_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is the last task to execute in the system.  It
--  stops the timer and reports the execution time of a preemptive
--  TASK_SET_PRIORITY.
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end TMTEST;
