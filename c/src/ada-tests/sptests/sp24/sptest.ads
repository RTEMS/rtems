--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 24 of the RTEMS
--  Single Processor Test Suite.
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

with RTEMS;

package SPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( 1 .. 3 ) of RTEMS.ID;
   TIMER_NAME : array ( 1 .. 3 ) of RTEMS.NAME;

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
--  RESUME_TASK
--
--  DESCRIPTION:
--
--  This subprogram is scheduled as a timer service routine.  When
--  it fires it resumes the task which is mapped to this timer.
--

   procedure RESUME_TASK (
      TIMER_ID        : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );

--
--  TASK_1_THROUGH_3
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Timer Manager.
--

   procedure TASK_1_THROUGH_3 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end SPTEST;
