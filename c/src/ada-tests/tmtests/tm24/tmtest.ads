--
--  TMTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 24 of the RTEMS
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

with TIMER_DRIVER;
with RTEMS;

package TMTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  This variable is used to determine which task should
--  start the timer and which task should stop the timer
--  and report the execution time.
--

   TASK_COUNT : RTEMS.UNSIGNED32;

--
--  This variable contains the execution time required of the
--  loop used to execute the directive.
--  

   OVERHEAD  : RTEMS.UNSIGNED32;

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
--  HIGH_TASK
--
--  DESCRIPTION:
--
--  This RTEMS task is responsible for measuring and reporting the
--  execution time for an invocation of TASK_WAKE_AFTER which 
--  attempts to yield but does not result in a context switch.
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

--
--  TASKS
--
--  DESCRIPTION:
--
--  These RTEMS tasks are responsible for measuring and reporting the
--  execution time for an invocation of TASK_WAKE_AFTER which 
--  attempts to yield but does result in a context switch.
--

   procedure TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end TMTEST;
