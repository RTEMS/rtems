--
--  SPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 22 of the RTEMS
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

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS timers created
--  by this test.
--

   TIMER_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.ID;
   TIMER_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 1 ) of RTEMS.NAME;

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
--  DELAYED_RESUME
--
--  DESCRIPTION:
--
--  This subprogram is scheduled to be fired as a timer service
--  routine.  When fired this subprogram resumes TASK_1.
--

   procedure DELAYED_RESUME (
      IGNORED_ID      : in     RTEMS.ID;  
      IGNORED_ADDRESS : in     RTEMS.ADDRESS 
   );

--
--  PRINT_TIME
--
--  DESCRIPTION:
--
--  This subprogram prints the name of TASK_1 and the current TIME of day.
--

   procedure PRINT_TIME;

--
--  TASK_1
--
--  DESCRIPTION:
--
--  This RTEMS task tests the Timer Manager.
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   );

end SPTEST;
