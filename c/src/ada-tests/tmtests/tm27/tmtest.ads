--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 27 of the RTEMS
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
--  This array contains the IDs of all RTEMS tasks created by this test.
--

   TASK_ID : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;

--
--  The following variable is set to the execution time returned
--  by the timer.
--

   END_TIME : RTEMS.UNSIGNED32;

--
--  The following variable indicates which interrupt case is
--  currently being timed.
--

   INTERRUPT_NEST : RTEMS.UNSIGNED32;

--
--  The following variables are set to the appropriate execution 
--  times by the ISR.  This is done so the times will be available
--  to be printed by a task.
--

   INTERRUPT_ENTER_TIME  : RTEMS.UNSIGNED32;
   INTERRUPT_RETURN_TIME : RTEMS.UNSIGNED32;

   INTERRUPT_ENTER_NESTED_TIME  : RTEMS.UNSIGNED32;
   INTERRUPT_RETURN_NESTED_TIME : RTEMS.UNSIGNED32;

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
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task reports the following execution times:
--
--    + interrupt entry overhead for a nested interrupt
--    + interrupt exit overhead for a nested interrupt
--    + interrupt entry overhead when no preemption occurs
--    + interrupt exit overhead when no preemption occurs
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASK_2
--
--  DESCRIPTION:
--
--  This RTEMS task reports the following execution times:
--
--    + interrupt entry overhead when preemption occurs
--    + interrupt exit overhead when preemption occurs
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  ISR_HANDLER
--
--  DESCRIPTION:
--
--  This subprogram is the test ISR.  It simply stops the
--  timer and save the execution time for ISR_HANDLER_INNER.
--  This is done to minimize the subroutine linkage overhead
--  for this routine.
--

   procedure ISR_HANDLER (
      VECTOR : in     RTEMS.VECTOR_NUMBER
   );

--
--  ISR_HANDLER_INNER
--
--  DESCRIPTION:
--
--  This subprogram is responsible for storing the execution
--  time for the case currently being timed.
--

   procedure ISR_HANDLER_INNER;

end TMTEST;
